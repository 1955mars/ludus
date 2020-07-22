#pragma once

#include "../../core/bitmap.hpp"
#include "../../core/internal_ptr.hpp"

namespace questart
{
    struct OpenGLTexture
    {
        OpenGLTexture(const questart::Bitmap& bitmap);

        void bind() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart