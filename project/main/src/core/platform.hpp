#pragma once

namespace questart
{
	enum class Platform
	{
		android,
		windows
	};

	Platform getCurrentPlatform();
}