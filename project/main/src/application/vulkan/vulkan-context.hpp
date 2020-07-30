#pragma once

#include "../../core/internal_ptr.hpp"

namespace questart
{
    struct VulkanContext
    {
        VulkanContext();

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart