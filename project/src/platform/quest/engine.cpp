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

        questart::Engine::appJava = java;
        return java;
    }

}

struct Engine::Internal
{
    struct android_app* app;
    ovrJava java;
    ovrMobile* ovrApp = nullptr;
    ANativeWindow* nativeWindow = nullptr;
    long long frameIndex = 1;
    double preDisplayTime = 0;
    double displayTime = 0;
    bool isResumed = false;
    int cpuLevel = 2;
    int gpuLevel = 3;
    std::thread::id mainThreadID;
    //std::thread::id renderThreadID;
    bool backButtonDownLastFrame = false;

    std::unique_ptr<questart::Application> application;
    std::unique_ptr<questart::Scene> scene = nullptr;

    Internal(struct android_app* app)
            :app(app),
             java(initOvrApp(app))
    {

    }

    const double timeStep()
    {

    }

    void run()
    {
        app->onAppCmd = app_handle_cmd;
        app->userData = this;
    }

    bool runMainLoop()
    {
        //questart::log("runMainLoop: ", "Entering!");
        while(app->destroyRequested == 0)
        {
            //questart::log("runMainLoop: ", "Inside Loop!");
            //Read all pending eventsquestart::log("runMainLoop: ", "Inside Loop!");
            for(;;)
            {
                int events;
                struct android_poll_source* source;
                const int timeoutMilliseconds =
                        (ovrApp == nullptr && app->destroyRequested == 0) ? -1 : 0;
                if (ALooper_pollAll(timeoutMilliseconds, nullptr, &events, (void**)&source) < 0)
                {
                    //questart::log("runMainLoop: ", "No pending events!");
                    break;
                }
                // Process this event.
                if (source != nullptr) {
                    source->process(app, source);
                }

                handleVRModeChanges();
            }

            handleVRApiEvents();

            handleInput();

            if(ovrApp == nullptr)
            {
                //questart::log("runMainLoop: ", "ovrApp has still not entered VR Mode!");
                continue;
            }

            if(scene == nullptr)
            {
                // Show a loading icon
                int frameFlags = 0;
                frameFlags |= VRAPI_FRAME_FLAG_FLUSH;

                ovrLayerProjection2 blackLayer = vrapi_DefaultLayerBlackProjection2();
                blackLayer.Header.Flags |= VRAPI_FRAME_LAYER_FLAG_INHIBIT_SRGB_FRAMEBUFFER;

                ovrLayerLoadingIcon2 iconLayer = vrapi_DefaultLayerLoadingIcon2();
                iconLayer.Header.Flags |= VRAPI_FRAME_LAYER_FLAG_INHIBIT_SRGB_FRAMEBUFFER;

                const ovrLayerHeader2* layers[] = {
                        &blackLayer.Header,
                        &iconLayer.Header,
                };

                ovrSubmitFrameDescription2 frameDesc{0};
                frameDesc.Flags = frameFlags;
                frameDesc.SwapInterval = 1;
                frameDesc.FrameIndex = frameIndex;
                frameDesc.DisplayTime = displayTime;
                frameDesc.LayerCount = 2;
                frameDesc.Layers = layers;

                vrapi_SubmitFrame2(ovrApp, &frameDesc);

                scene = ::createOVRScene(vulkanContext);

                if(scene!= nullptr)
                    questart::log("runMainLoop:", "Created OVRScene successfully!");

                preDisplayTime = displayTime;
            }

            //Simulation
            //questart::log("Simulation:", "Entering!");
            frameIndex++;

            const double predictedDisplayTime = vrapi_GetPredictedDisplayTime(ovrApp, frameIndex);
            const ovrTracking2 tracking = vrapi_GetPredictedTracking2(ovrApp, predictedDisplayTime);

            displayTime = predictedDisplayTime;

            //questart::log("Simulation: displayTime: ", std::to_string(displayTime));

            scene->update(displayTime-preDisplayTime, tracking);

            render();

        }

        vrapi_DestroySystemVulkan();
        vrapi_Shutdown();
        java.Vm->DetachCurrentThread();

        return true;
    }

    void handleVRModeChanges()
    {
        questart::log("handleVRModeChanges: ", "Entering!");
        if(isResumed && nativeWindow != nullptr)
        {
            questart::log("handleVRModeChanges: ", "is Resumed and got Native Window!");
            if(ovrApp == nullptr)
            {
                questart::log("handleVRModeChanges: ", "ovrApp is null!");
                ovrModeParmsVulkan parmsVulkan = vrapi_DefaultModeParmsVulkan(&java,
                                                                              (unsigned  long long) *reinterpret_cast<const VkQueue*>(&vulkanSystem.getDevice().getWorkQueue()));

                // No need to reset the FLAG_FULLSCREEN window flag when using a View
                parmsVulkan.ModeParms.Flags &= ~VRAPI_MODE_FLAG_RESET_WINDOW_FULLSCREEN;

                parmsVulkan.ModeParms.Flags |= VRAPI_MODE_FLAG_NATIVE_WINDOW;
                parmsVulkan.ModeParms.WindowSurface = (size_t)nativeWindow;

                // Leave explicit egl objects defaulted.
                parmsVulkan.ModeParms.Display = 0;
                parmsVulkan.ModeParms.ShareContext = 0;

                ovrApp = vrapi_EnterVrMode((ovrModeParms*)&parmsVulkan);

                if(ovrApp == nullptr)
                {
                    questart::log("handleVRModeChanges:", "Invalid Native Window!");
                    nativeWindow = nullptr;
                }

                if(ovrApp != nullptr)
                {
                    vrapi_SetClockLevels(ovrApp, cpuLevel, gpuLevel);
                    vrapi_SetPerfThread(ovrApp, VRAPI_PERF_THREAD_TYPE_MAIN, *reinterpret_cast<const uint32_t*>(&mainThreadID));
                    //vrapi_SetPerfThread(ovrApp, VRAPI_PERF_THREAD_TYPE_RENDERER, *reinterpret_cast<const uint32_t*>(&renderThreadID));
                }
            } else{
                questart::log("handleVRModeChanges: ", "OVRApp is not null!");
            }
        } else
        {

            if(ovrApp != nullptr)
            {
                vrapi_LeaveVrMode(ovrApp);
                ovrApp = nullptr;
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
            ovrResult  result = vrapi_EnumerateInputDevices(ovrApp, i, &cap);

            if(result < 0)
                break;

            if(cap.Type == ovrControllerType_TrackedRemote)
            {
                ovrInputStateTrackedRemote trackedRemoteState;
                trackedRemoteState.Header.ControllerType = ovrControllerType_TrackedRemote;
                result = vrapi_GetCurrentInputState(ovrApp, cap.DeviceID, &trackedRemoteState.Header);
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
            vrapi_ShowSystemUI(&java, VRAPI_SYS_UI_CONFIRM_QUIT_MENU);
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

    Engine::Internal& ovrApp = *reinterpret_cast<Engine::Internal*>(app->userData);

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
            ovrApp.isResumed = true;
            break;
        }
        case APP_CMD_PAUSE: {
            questart::log("app_handle_cmd: ", "App Command Pause!");
            ovrApp.isResumed = false;
            break;
        }
        case APP_CMD_STOP: {
            questart::log("app_handle_cmd: ", "App Command Stop!");
            break;
        }
        case APP_CMD_DESTROY: {
            questart::log("app_handle_cmd: ", "App Command Destroy!");
            ovrApp.nativeWindow = nullptr;
            break;
        }
        case APP_CMD_INIT_WINDOW: {
            questart::log("app_handle_cmd: ", "App Command Surface Created!");
            questart::log("app_handle_cmd: ", "App Command InitWindow!");
            if(app->window == nullptr)
            {
                questart::log("app_handle_cmd: ", "app window is null!");
            }
            ovrApp.nativeWindow = app->window;
            break;
        }
        case APP_CMD_TERM_WINDOW: {
            questart::log("app_handle_cmd: ", "App Command Surface Destroyed!");
            questart::log("app_handle_cmd: ", "App Command Term Window!");
            ovrApp.nativeWindow = nullptr;
            break;
        }

        default:
            break;
    }
}
