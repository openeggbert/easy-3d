// SPDX-License-Identifier: MIT
#pragma once

/// @file CubeMeshRenderer.hpp
/// @brief CNA renderer adapter (Roadmap Phase 4): uploads a CubeMesh's
/// vertex/index arrays to GPU buffers once, then issues indexed
/// TriangleList draw calls on demand. Knows nothing about tiles, gameplay,
/// or cameras — the caller owns and configures the BasicEffect
/// (World/View/Projection/Texture) before calling Draw().

#include <cstdint>
#include <vector>

#include <Microsoft/Xna/Framework/Graphics/BasicEffect.hpp>
#include <Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp>
#include <Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp>
#include <Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp>

#include "Easy3D/CubeMesh.hpp"

namespace Easy3D
{
    class CubeMeshRenderer
    {
    public:
        /// @brief Uploads @p vertices / @p indices to new GPU buffers.
        /// @p indices.size() must be a multiple of 3 (whole triangles).
        CubeMeshRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                         const std::vector<CubeVertex>& vertices,
                         const std::vector<std::uint32_t>& indices);

        /// @brief Draws the uploaded mesh once per pass of @p effect's
        /// current technique. @p effect's World/View/Projection and, if
        /// texturing is wanted, TextureEnabled/Texture must already be set.
        void Draw(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                  Microsoft::Xna::Framework::Graphics::BasicEffect& effect) const;

        [[nodiscard]] int VertexCount() const noexcept { return m_vertexCount; }
        [[nodiscard]] int PrimitiveCount() const noexcept { return m_primitiveCount; }

    private:
        Microsoft::Xna::Framework::Graphics::VertexBuffer m_vertexBuffer;
        Microsoft::Xna::Framework::Graphics::IndexBuffer m_indexBuffer;
        int m_vertexCount = 0;
        int m_primitiveCount = 0;
    };
}
