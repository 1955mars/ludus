#pragma once

#include "../core/glm-wrapper.hpp"
#include "../core/internal_ptr.hpp"

namespace questart
{
	struct PerspectiveCamera
    {
        PerspectiveCamera(const float& width, const float& height);
        const glm::mat4& getProjectionMatrix() const;
        const glm::mat4& getViewMatrix() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart