#pragma once

#include "../core/glm-wrapper.hpp"
#include "../core/internal_ptr.hpp"

namespace questart
{
    struct PerspectiveCamera
    {
        PerspectiveCamera(const float& width, const float& height);

        void configure(const glm::vec3& position, const glm::vec3& direction);

        glm::mat4 getProjectionMatrix() const;

        glm::mat4 getViewMatrix() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart