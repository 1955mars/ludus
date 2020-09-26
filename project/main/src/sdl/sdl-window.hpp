#pragma once

#include "../core/internal_ptr.hpp"
#include "sdl-wrapper.hpp"

namespace questart
{
    struct SDLWindow
    {
        SDLWindow(const uint32_t& windowFlags);

        SDL_Window* getWindow() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart