#pragma once

#include "../../../core/internal_ptr.hpp"
#include "../../../core/application.hpp"

namespace questart
{
    struct VulkanApplication : public questart::Application
    {
        VulkanApplication();

        void update(const float& delta, void* tracking) override;

        void render(const double& displayTime = 0.0, void* tracking = nullptr, long long frameIndex = 0) override;

        void onWindowResized() override;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };

} //namespace questart