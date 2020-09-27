#pragma once

#include "internal_ptr.hpp"
#include "application.hpp"

namespace questart
{
    struct VulkanApplication : public questart::Application
    {
        VulkanApplication();

        void update(const float& delta) override;

        void render() override;

        inline void onWindowResized() override {}

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };

} //namespace questart