#pragma once

#include "internal_ptr.hpp"
#include "sdl-wrapper.hpp"

namespace questart
{
    struct Bitmap
    {
        Bitmap(SDL_Surface* surface);

        uint16_t getWidth() const;

        uint16_t getHeight() const;

        void* getPixelData() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart