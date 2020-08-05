#pragma once

#include "../../core/internal_ptr.hpp"
#include "../application.hpp"

namespace questart
{
    struct VulkanApplication : public questart::Application
    {
        VulkanApplication();

        void update(const float& delta) override;

        void render() override;

        void onWindowResized() override;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };

} //namespace questart