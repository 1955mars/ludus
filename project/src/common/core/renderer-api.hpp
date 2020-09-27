#pragma once

namespace questart
{
	struct RendererAPI
    {
        enum class API
        {
            None = 0,
            OpenGL,
            Vulkan
        };

        static API sAPI;
    };
}