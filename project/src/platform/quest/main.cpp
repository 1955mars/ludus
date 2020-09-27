#pragma once

#include "engine.hpp"
#include "../../../../../main/src/core/log.hpp"

void android_main(struct android_app* app)
{
    questart::log("ehelo", "world");
}