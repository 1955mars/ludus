#pragma once

#include "../../core/internal_ptr.hpp"
#include "../application.hpp"

namespace questart
{
	struct OpenGLApplication : public questart::Application
    {
        OpenGLApplication();
        void render() override;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart