#pragma once

#include "../../core/internal_ptr.hpp"

#include <string>

namespace questart
{
    struct OpenGLTexture
    {
        OpenGLTexture(const std::string& texturePath);

        void bind() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart