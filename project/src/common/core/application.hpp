#pragma once
#include "internal_ptr.hpp"

namespace questart
{
	struct Application
    {
        virtual void update(const float& delta) = 0;
        virtual void render() = 0;

        virtual void onWindowResized() = 0;
    }; // struct Application
} // namespace questart