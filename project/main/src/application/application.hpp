#pragma once
#include "../core/internal_ptr.hpp"

namespace questart
{
	struct Application
    {
        Application();

        virtual ~Application() = default;

        void startApplication();

        bool runMainLoop();

        virtual void update(const float& delta) = 0;

        virtual void render() = 0;

        virtual void onWindowResized() = 0;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    }; // struct Application
} // namespace questart