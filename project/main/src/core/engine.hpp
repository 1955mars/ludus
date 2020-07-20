#pragma once

#include "internal_ptr.hpp"

namespace questart
{
	struct Engine
    {
        Engine();

        void run();

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart