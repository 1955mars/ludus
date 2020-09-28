#include "engine.hpp"
#include "log.hpp"
#include "ovr-wrapper.hpp"
#include "graphics-wrapper.hpp"
#include "application.hpp"
#include "vulkan-application.hpp"

#include "android/window.h"
#include <thread>
#include <sys/prctl.h>
#include <scene.hpp>

using questart::Engine;


namespace
{
    ovrJava initOvrApp(struct android_app* app)
    {
        ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);

        ovrJava java;
        java.Vm = app->activity->vm;
        java.ActivityObject = app->activity->clazz;
        app->activity->vm->AttachCurrentThread(&java.Env, nullptr);

        prctl(PR_SET_NAME, (long)"Main", 0, 0, 0);

        if(!InitVulkan())
        {
            questart::log("InitOvrApp: ", "Failed to Init Vulkan!");
            throw std::runtime_error("Failed to Init Vulkan!");
        }

        ovrInitParms initParms = vrapi_DefaultInitParms(&java);
        initParms.GraphicsAPI = VRAPI_GRAPHICS_API_VULKAN_1;
        int32_t initResult = vrapi_Initialize(&initParms);

        if(initResult != VRAPI_INITIALIZE_SUCCESS)
        {
            questart::log("InitOvrApp: ", "Failed to init VrApi");
            throw std::runtime_error("Failed to init VRAPI");
        }
        questart::log("InitOvrApp: ", "Successfully initialized Vulkan & VrApi");

        return java;
    }

}

android_app* questart::Engine::appHandle;
ovrJava questart::Engine::appJava;
ovrMobile* questart::Engine::ovrApp;
long long questart::Engine::frameIndex;
VkQueue questart::Engine::graphicsQueue;


struct Engine::Internal
{
    ANativeWindow* nativeWindow = nullptr;
    long long frameIndex = 1;
    double previousTime = 0;
    double displayTime = 0;
    bool isResumed = false;
    int cpuLevel = 2;
    int gpuLevel = 3;
    std::thread::id mainThreadID;
    //std::thread::id renderThreadID;
    bool backButtonDownLastFrame = false;

    std::unique_ptr<questart::Application> application;

    Internal(struct android_app* app)
    {
        questart::Engine::appHandle = app;
        questart::Engine::appJava = initOvrApp(app);
        application = std::make_unique<questart::VulkanApplication>();
        mainThreadID = std::this_thread::get_id();
    }

    void run()
    {
        appHandle->onAppCmd = app_handle_cmd;
        appHandle->userData = this;

        runMainLoop();
    }

    bool runMainLoop()
    {
        //questart::log("runMainLoop: ", "Entering!");
        while(appHandle->destroyRequested == 0)
        {
            //questart::log("runMainLoop: ", "Inside Loop!");
            //Read all pending eventsquestart::log("runMainLoop: ", "Inside Loop!");
            for(;;)
            {
                int events;
                struct android_poll_source* source;
                const int timeoutMilliseconds =
                        (questart::Engine::ovrApp == nullptr && questart::Engine::appHandle->destroyRequested == 0) ? -1 : 0;
                if (ALooper_pollAll(timeoutMilliseconds, nullptr, &events, (void**)&source) < 0)
                {
                    //questart::log("runMainLoop: ", "No pending events!");
                    break;
                }
                // Process this event.
                if (source != nullptr) {
                    source->process(questart::Engine::appHandle, source);
                }

                handleVRModeChanges();
            }

            handleVRApiEvents();

            handleInput();

            if(questart::Engine::ovrApp == nullptr)
            {
                //questart::log("runMainLoop: ", "ovrApp has still not entered VR Mode!");
                continue;
            }


            //Simulation
            //questart::log("Simulation:", "Entering!");
            frameIndex++;

            const double predictedDisplayTime = vrapi_GetPredictedDisplayTime(questart::Engine::ovrApp, frameIndex);
            ovrTracking2 tracking = vrapi_GetPredictedTracking2(questart::Engine::ovrApp, predictedDisplayTime);


            displayTime = predictedDisplayTime;

            //questart::log("Simulation: displayTime: ", std::to_string(displayTime));

            application->update(predictedDisplayTime - previousTime, (void*)&tracking);
            application->render(displayTime, (void*)&tracking, frameIndex);

            if(displayTime > previousTime)
                previousTime = displayTime;
        }

        vrapi_DestroySystemVulkan();
        vrapi_Shutdown();
        questart::Engine::appJava.Vm->DetachCurrentThread();

        return true;
    }

    void handleVRModeChanges()
    {
        questart::log("handleVRModeChanges: ", "Entering!");
        if(isResumed && nativeWindow != nullptr)
        {
            questart::log("handleVRModeChanges: ", "is Resumed and got Native Window!");
            if(questart::Engine::ovrApp == nullptr)
            {
                questart::log("handleVRModeChanges: ", "ovrApp is null!");
                ovrModeParmsVulkan parmsVulkan = vrapi_DefaultModeParmsVulkan(&questart::Engine::appJava,
                                                                              (unsigned  long long) *reinterpret_cast<const VkQueue*>(&questart::Engine::graphicsQueue));

                // No need to reset the FLAG_FULLSCREEN window flag when using a View
                parmsVulkan.ModeParms.Flags &= ~VRAPI_MODE_FLAG_RESET_WINDOW_FULLSCREEN;

                parmsVulkan.ModeParms.Flags |= VRAPI_MODE_FLAG_NATIVE_WINDOW;
                parmsVulkan.ModeParms.WindowSurface = (size_t)nativeWindow;

                // Leave explicit egl objects defaulted.
                parmsVulkan.ModeParms.Display = 0;
                parmsVulkan.ModeParms.ShareContext = 0;

                questart::Engine::ovrApp = vrapi_EnterVrMode((ovrModeParms*)&parmsVulkan);

                if(questart::Engine::ovrApp == nullptr)
                {
                    questart::log("handleVRModeChanges:", "Invalid Native Window!");
                    nativeWindow = nullptr;
                }

                if(ovrApp != nullptr)
                {
                    vrapi_SetClockLevels(questart::Engine::ovrApp, cpuLevel, gpuLevel);
                    vrapi_SetPerfThread(questart::Engine::ovrApp, VRAPI_PERF_THREAD_TYPE_MAIN, *reinterpret_cast<const uint32_t*>(&mainThreadID));
                    //vrapi_SetPerfThread(ovrApp, VRAPI_PERF_THREAD_TYPE_RENDERER, *reinterpret_cast<const uint32_t*>(&renderThreadID));
                }
            } else{
                questart::log("handleVRModeChanges: ", "OVRApp is not null!");
            }
        } else
        {

            if(questart::Engine::ovrApp != nullptr)
            {
                vrapi_LeaveVrMode(ovrApp);
                questart::Engine::ovrApp = nullptr;
            }
        }
    }

    void handleInput()
    {
        //questart::log("handleInput: ", "Entering!");
        bool backButtonDownThisFrame = false;

        for(int i=0; ; i++)
        {
            ovrInputCapabilityHeader cap;
            ovrResult  result = vrapi_EnumerateInputDevices(questart::Engine::ovrApp, i, &cap);

            if(result < 0)
                break;

            if(cap.Type == ovrControllerType_TrackedRemote)
            {
                ovrInputStateTrackedRemote trackedRemoteState;
                trackedRemoteState.Header.ControllerType = ovrControllerType_TrackedRemote;
                result = vrapi_GetCurrentInputState(questart::Engine::ovrApp, cap.DeviceID, &trackedRemoteState.Header);
                if(result == ovrSuccess)
                {
                    backButtonDownThisFrame |= trackedRemoteState.Buttons & ovrButton_Back;
                    backButtonDownThisFrame |= trackedRemoteState.Buttons & ovrButton_B;
                    backButtonDownThisFrame |= trackedRemoteState.Buttons & ovrButton_Y;
                }
            }
        }

        bool _backButtonDownLastFrame = backButtonDownLastFrame;
        backButtonDownLastFrame = backButtonDownThisFrame;

        if(_backButtonDownLastFrame && !backButtonDownThisFrame)
        {
            questart::log("handleInput: ", "back button short press");
            questart::log("handleInput: ", "vrapi_ShowSystemUI( confirmQuit )");
            vrapi_ShowSystemUI(&questart::Engine::appJava, VRAPI_SYS_UI_CONFIRM_QUIT_MENU);
        }
    }

    static void handleVRApiEvents()
    {
        //questart::log("handleVRApiEvents: ", "Entering!");
        ovrEventDataBuffer eventDataBuffer = {};

        // Poll for VrApi events
        for (;;) {
            auto* eventHeader = (ovrEventHeader*)(&eventDataBuffer);
            ovrResult res = vrapi_PollEvent(eventHeader);
            if (res != ovrSuccess) {
                break;
            }

            switch (eventHeader->EventType) {
                case VRAPI_EVENT_DATA_LOST:
                    questart::log("handleVRApiEvents: ", "vrapi_PollEvent: Received VRAPI_EVENT_DATA_LOST");
                    break;
                case VRAPI_EVENT_VISIBILITY_GAINED:
                    questart::log("handleVRApiEvents: ", "vrapi_PollEvent: Received VRAPI_EVENT_VISIBILITY_GAINED");
                    break;
                case VRAPI_EVENT_VISIBILITY_LOST:
                    questart::log("handleVRApiEvents: ", "vrapi_PollEvent: Received VRAPI_EVENT_VISIBILITY_LOST");
                    break;
                case VRAPI_EVENT_FOCUS_GAINED:
                    // FOCUS_GAINED is sent when the application is in the foreground and has
                    // input focus. This may be due to a system overlay relinquishing focus
                    // back to the application.
                    questart::log("handleVRApiEvents: ", "vrapi_PollEvent: Received VRAPI_EVENT_FOCUS_GAINED");
                    break;
                case VRAPI_EVENT_FOCUS_LOST:
                    // FOCUS_LOST is sent when the application is no longer in the foreground and
                    // therefore does not have input focus. This may be due to a system overlay taking
                    // focus from the application. The application should take appropriate action when
                    // this occurs.
                    questart::log("handleVRApiEvents: ", "vrapi_PollEvent: Received VRAPI_EVENT_FOCUS_LOST");
                    break;
                default:
                    questart::log("handleVRApiEvents: ", "vrapi_PollEvent: Unknown event");
                    break;
            }
        }
    }

};


void Engine::app_handle_cmd(struct android_app* app, int32_t cmd) {

    Engine::Internal& engine = *reinterpret_cast<Engine::Internal*>(app->userData);

    switch (cmd) {
        // There is no APP_CMD_CREATE. The ANativeActivity creates the
        // application thread from onCreate(). The application thread
        // then calls android_main().
        case APP_CMD_START: {
            questart::log("app_handle_cmd: ", "App Command Start!");
            break;
        }
        case APP_CMD_RESUME: {
            questart::log("app_handle_cmd: ", "App Command Resume!");
            engine.isResumed = true;
            break;
        }
        case APP_CMD_PAUSE: {
            questart::log("app_handle_cmd: ", "App Command Pause!");
            engine.isResumed = false;
            break;
        }
        case APP_CMD_STOP: {
            questart::log("app_handle_cmd: ", "App Command Stop!");
            break;
        }
        case APP_CMD_DESTROY: {
            questart::log("app_handle_cmd: ", "App Command Destroy!");
            engine.nativeWindow = nullptr;
            break;
        }
        case APP_CMD_INIT_WINDOW: {
            questart::log("app_handle_cmd: ", "App Command Surface Created!");
            questart::log("app_handle_cmd: ", "App Command InitWindow!");
            if(app->window == nullptr)
            {
                questart::log("app_handle_cmd: ", "app window is null!");
            }
            engine.nativeWindow = app->window;
            break;
        }
        case APP_CMD_TERM_WINDOW: {
            questart::log("app_handle_cmd: ", "App Command Surface Destroyed!");
            questart::log("app_handle_cmd: ", "App Command Term Window!");
            engine.nativeWindow = nullptr;
            break;
        }

        default:
            break;
    }
}


Engine::Engine(struct android_app* app)
        :internal(make_internal_ptr<Internal>(app))
{

}

void Engine::run()
{
    internal->run();
}