#pragma once

namespace questart
{
	struct Application
    {
        Application() = default;

        virtual ~Application() = default;

        void startApplication();

        bool runMainLoop();

        virtual void render() = 0;
    }; // struct Application
} // namespace questart