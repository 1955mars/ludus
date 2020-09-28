#include "engine.hpp"
#include "log.hpp"

void android_main(struct android_app* app)
{
    questart::log("Main", "Entering");
    questart::Engine(app).run();
}