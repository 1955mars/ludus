#include "platform.hpp"

questart::Platform questart::getCurrentPlatform()
{
#if defined WIN32
	return questart::Platform::windows;
#elif __ANDROID__
	return questart::Platform::android;
#endif

}