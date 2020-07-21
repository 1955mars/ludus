#pragma once

#include "../../core/graphics-wrapper.hpp"
#include "../../core/internal_ptr.hpp"
#include "../../core/mesh.hpp"

namespace questart
{
	struct OpenGLMesh
    {
        OpenGLMesh(const questart::Mesh& mesh);

        const GLuint& getVertexBufferId() const;

        const GLuint& getIndexBufferId() const;

        const uint32_t& getNumIndices() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart