#pragma once

#include "internal_ptr.hpp"
#include "android_native_app_glue.h"
#include "ovr-wrapper.hpp"

namespace questart
{
    struct Engine
    {
        Engine(struct android_app* app);

        void run();

        static void app_handle_cmd(struct android_app* app, int32_t cmd);

        static android_app* appHandle;
        static ovrJava appJava;
        static ovrMobile* ovrApp;
        static long long frameIndex;
        static VkQueue graphicsQueue;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart