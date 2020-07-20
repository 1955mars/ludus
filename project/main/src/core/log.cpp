#ifndef  NDEBUG
#ifdef __ANDROID__
#include <android/log.h>
#endif // __ANDROID__
#endif // ! NDEBUG

#include "log.hpp"
#include <iostream>

void questart::log(const std::string& tag, const std::string& message)
{
#ifndef NDEBUG
#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_DEBUG, "A Simple Triangle", "%s: %s", tag.c_str(), message.c_str());
    #else
    std::cout << tag << ": " << message << std::endl;
#endif
#endif
}

void questart::log(const std::string& tag, const std::string& message, const std::exception& error)
{
#ifndef  NDEBUG
    std::string output = message + "Exception message was: " + std::string{error.what()};
    questart::log(tag, output);
#endif // ! NDEBUG
}

