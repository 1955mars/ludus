#pragma once

#if defined WIN32
#define GLEW_STATIC
#include <GL/glew.h>
#elif __ANDROID__
#include <GLES3/gl3.h>
#define USING_GLES
#endif