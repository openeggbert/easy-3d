// SPDX-License-Identifier: MIT
#include "Easy3D/CubeMeshRenderer.hpp"

#include <stdexcept>

namespace Easy3D
{
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::BufferUsage;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::IndexElementSize;
    using Microsoft::Xna::Framework::Graphics::PrimitiveType;
    using Microsoft::Xna::Framework::Graphics::VertexPositionTexture;

    namespace
    {
        std::vector<VertexPositionTexture> ToVertexPositionTexture(const std::vector<CubeVertex>& vertices)
        {
            std::vector<VertexPositionTexture> out;
            out.reserve(vertices.size());
            for (const auto& v : vertices)
            {
                out.emplace_back(v.Position, v.Uv);
            }
            return out;
        }
    }

    CubeMeshRenderer::CubeMeshRenderer(GraphicsDevice& device,
                                       const std::vector<CubeVertex>& vertices,
                                       const std::vector<std::uint32_t>& indices)
        : m_vertexBuffer(device, static_cast<int>(vertices.size())),
          m_indexBuffer(device, IndexElementSize::ThirtyTwoBits,
                        static_cast<int>(indices.size()), BufferUsage::None),
          m_vertexCount(static_cast<int>(vertices.size())),
          m_primitiveCount(static_cast<int>(indices.size() / 3))
    {
        if (indices.size() % 3 != 0)
        {
            throw std::invalid_argument("CubeMeshRenderer: index count must be a multiple of 3");
        }

        const auto cnaVertices = ToVertexPositionTexture(vertices);
        m_vertexBuffer.SetData(cnaVertices.data(), static_cast<int>(cnaVertices.size()));
        m_indexBuffer.SetData(indices.data(), static_cast<int>(indices.size()));
    }

    void CubeMeshRenderer::Draw(GraphicsDevice& device, BasicEffect& effect) const
    {
        for (auto& pass : effect.getCurrentTechniqueProperty()->getPassesProperty())
        {
            pass.Apply();
            device.SetVertexBuffer(&m_vertexBuffer);
            device.Indices(&m_indexBuffer);
            device.DrawIndexedPrimitives(PrimitiveType::TriangleList,
                                         /*baseVertex=*/0, /*minVertexIndex=*/0,
                                         m_vertexCount, /*startIndex=*/0, m_primitiveCount);
        }
    }
}
