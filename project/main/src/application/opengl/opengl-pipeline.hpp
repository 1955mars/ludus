#pragma once

#include "../../core/graphics-wrapper.hpp"
#include "../../core/graphics-wrapper.hpp"
#include "../../core/internal_ptr.hpp"
#include "opengl-mesh.hpp"
#include <string>

namespace questart
{
	struct OpenGLPipeline
    {
        OpenGLPipeline(const std::string& shaderName);
        void render(const questart::OpenGLMesh& mesh, const glm::mat4& mvp) const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart
