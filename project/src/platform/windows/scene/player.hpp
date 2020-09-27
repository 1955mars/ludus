#pragma once

#include "ludus.hpp"

namespace questart
{
    struct Player
    {
        Player(const glm::vec3& position);

        void moveForward(const float& delta);

        void moveBackward(const float& delta);

        void moveUp(const float& delta);

        void moveDown(const float& delta);

        void turnLeft(const float& delta);

        void turnRight(const float& delta);

        glm::vec3 getPosition() const;

        glm::vec3 getDirection() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart