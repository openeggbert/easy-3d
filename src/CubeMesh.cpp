// SPDX-License-Identifier: MIT
#include "Easy3D/CubeMesh.hpp"

namespace Easy3D
{
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;

    namespace
    {
        // Four corners per face, CCW as seen from outside (outward normal =
        // (v1-v0) x (v2-v0)); one array per face, {+Z, -Z, +X, -X, +Y, -Y} —
        // matches the CubeFace enum's ordering.
        void ComputeFaceCorners(const Vector3& min, const Vector3& max, Vector3 outCorners[6][4])
        {
            const Vector3 faceCorners[6][4] = {
                {{min.X, min.Y, max.Z}, {max.X, min.Y, max.Z}, {max.X, max.Y, max.Z}, {min.X, max.Y, max.Z}}, // +Z
                {{max.X, min.Y, min.Z}, {min.X, min.Y, min.Z}, {min.X, max.Y, min.Z}, {max.X, max.Y, min.Z}}, // -Z
                {{max.X, min.Y, max.Z}, {max.X, min.Y, min.Z}, {max.X, max.Y, min.Z}, {max.X, max.Y, max.Z}}, // +X
                {{min.X, min.Y, min.Z}, {min.X, min.Y, max.Z}, {min.X, max.Y, max.Z}, {min.X, max.Y, min.Z}}, // -X
                {{min.X, max.Y, max.Z}, {max.X, max.Y, max.Z}, {max.X, max.Y, min.Z}, {min.X, max.Y, min.Z}}, // +Y
                {{min.X, min.Y, min.Z}, {max.X, min.Y, min.Z}, {max.X, min.Y, max.Z}, {min.X, min.Y, max.Z}}, // -Y
            };
            for (int face = 0; face < 6; ++face)
            {
                for (int corner = 0; corner < 4; ++corner)
                {
                    outCorners[face][corner] = faceCorners[face][corner];
                }
            }
        }

        void AppendFace(const Vector3 corners[4], const UvRect& uv,
                        std::vector<CubeVertex>& vertices,
                        std::vector<std::uint32_t>& indices)
        {
            const Vector2 faceUvs[4] = {
                {uv.U0, uv.V0},
                {uv.U1, uv.V0},
                {uv.U1, uv.V1},
                {uv.U0, uv.V1},
            };

            const auto base = static_cast<std::uint32_t>(vertices.size());
            for (int i = 0; i < 4; ++i)
            {
                vertices.push_back(CubeVertex{corners[i], faceUvs[i]});
            }
            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 2);
            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 3);
        }
    }

    void AppendCubeMesh(const CubeItem& item,
                        std::vector<CubeVertex>& vertices,
                        std::vector<std::uint32_t>& indices)
    {
        const Vector3 half = item.Size * 0.5f;
        const Vector3 min = item.Center - half;
        const Vector3 max = item.Center + half;

        Vector3 faceCorners[6][4];
        ComputeFaceCorners(min, max, faceCorners);

        for (const auto& corners : faceCorners)
        {
            AppendFace(corners, item.Uv, vertices, indices);
        }
    }

    void BuildCubeMesh(const CubeBatch& batch,
                       std::vector<CubeVertex>& vertices,
                       std::vector<std::uint32_t>& indices)
    {
        for (const CubeItem& item : batch.Items())
        {
            AppendCubeMesh(item, vertices, indices);
        }
    }

    void AppendDirectionalCubeMesh(const DirectionalCubeItem& item,
                                   std::vector<CubeVertex>& vertices,
                                   std::vector<std::uint32_t>& indices)
    {
        const Vector3 half = item.Size * 0.5f;
        const Vector3 min = item.Center - half;
        const Vector3 max = item.Center + half;

        Vector3 faceCorners[6][4];
        ComputeFaceCorners(min, max, faceCorners);

        for (int face = 0; face < 6; ++face)
        {
            const DirectionalCubeFace& faceSpec = item.Faces[face];
            if (!faceSpec.Visible)
            {
                continue;
            }
            AppendFace(faceCorners[face], faceSpec.Uv, vertices, indices);
        }
    }
}
