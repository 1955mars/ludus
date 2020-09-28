#pragma once
#include "internal_ptr.hpp"

namespace questart
{
	struct Application
    {
        virtual void update(const float& delta, void* tracking = nullptr) = 0;
        virtual void render(const double& displayTime = 0.0, void* tracking = nullptr, long long frameIndex = 0) = 0;

        virtual void onWindowResized() = 0;
    }; // struct Application
} // namespace questart