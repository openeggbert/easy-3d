// SPDX-License-Identifier: MIT
#include "Easy3D/CubeMesh.hpp"

#include <cmath>

namespace Easy3D
{
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;

    namespace
    {
        // Four corners per face, one array per face, {+Z, -Z, +X, -X, +Y, -Y}
        // — matches the CubeFace enum's ordering. ALL SIX faces are wound
        // CLOCKWISE as seen from outside the cube (the right-hand-rule cross
        // product (v1-v0) x (v2-v0) points INTO the cube) — this is the XNA
        // front-face convention, NOT the OpenGL-textbook CCW one. Root cause
        // finally identified 2026-07-10 (galaxy-eggbert "front/side faces
        // don't render" report): CNA implements genuine XNA culling — under
        // the default RasterizerState::CullCounterClockwise, triangles that
        // appear visually CLOCKWISE on screen survive (verified three ways:
        // CNA's own contrast-checked cullmode golden test, FNA SpriteBatch's
        // visually-CW quads, and live in galaxy-eggbert where billboards
        // only became visible once re-wound to visually-CW). This file
        // originally used the OpenGL CCW-from-outside convention for all 6
        // faces, so every face was invisible from outside and scenes
        // "worked" only by showing the mirrored interiors of opposite
        // faces; the 2026-07-10 +Y/-Y reversal ("empirically fixed, not
        // root-caused") was the first half of this fix, and the 4 side
        // faces are now reversed to match. The corner order is
        // top-left, top-right, bottom-right, bottom-left AS SEEN FROM
        // OUTSIDE each face, which under AppendFace's fixed corner->UV
        // pairing also puts U0 at the left and V0 at the top of the visible
        // face (upright, unmirrored textures).
        void ComputeFaceCorners(const Vector3& min, const Vector3& max, Vector3 outCorners[6][4])
        {
            const Vector3 faceCorners[6][4] = {
                {{min.X, max.Y, max.Z}, {max.X, max.Y, max.Z}, {max.X, min.Y, max.Z}, {min.X, min.Y, max.Z}}, // +Z
                {{max.X, max.Y, min.Z}, {min.X, max.Y, min.Z}, {min.X, min.Y, min.Z}, {max.X, min.Y, min.Z}}, // -Z
                {{max.X, max.Y, max.Z}, {max.X, max.Y, min.Z}, {max.X, min.Y, min.Z}, {max.X, min.Y, max.Z}}, // +X
                {{min.X, max.Y, min.Z}, {min.X, max.Y, max.Z}, {min.X, min.Y, max.Z}, {min.X, min.Y, min.Z}}, // -X
                {{min.X, max.Y, min.Z}, {max.X, max.Y, min.Z}, {max.X, max.Y, max.Z}, {min.X, max.Y, max.Z}}, // +Y
                {{min.X, min.Y, max.Z}, {max.X, min.Y, max.Z}, {max.X, min.Y, min.Z}, {min.X, min.Y, min.Z}}, // -Y
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

        // Appends @p corners (CCW as seen from the intended "front" normal,
        // same 4-corner/UV convention as AppendFace) twice: once wound so
        // it's visible from the front, once with reversed triangle order
        // (same vertex positions/UVs, so the texture isn't mirrored) so it's
        // also visible from the back — genuinely double-sided regardless of
        // the renderer's cull state, unlike a single quad.
        void AppendDoubleSidedQuad(const Vector3 corners[4], const Vector2 uvs[4],
                                   std::vector<CubeVertex>& vertices,
                                   std::vector<std::uint32_t>& indices)
        {
            auto base = static_cast<std::uint32_t>(vertices.size());
            for (int i = 0; i < 4; ++i)
            {
                vertices.push_back(CubeVertex{corners[i], uvs[i]});
            }
            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 2);
            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 3);

            base = static_cast<std::uint32_t>(vertices.size());
            for (int i = 0; i < 4; ++i)
            {
                vertices.push_back(CubeVertex{corners[i], uvs[i]});
            }
            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 1);
            indices.push_back(base + 0);
            indices.push_back(base + 3);
            indices.push_back(base + 2);
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

    void AppendPlateMesh(const PlateItem& item,
                         std::vector<CubeVertex>& vertices,
                         std::vector<std::uint32_t>& indices)
    {
        const float halfW = item.Width * 0.5f;
        const float halfH = item.Height * 0.5f;

        // Corner order matches AppendFace's convention: bottom-left,
        // bottom-right, top-right, top-left as seen from the plate's
        // "front" normal (+Z for PlateAxis::Z, +X for PlateAxis::X, +Y for
        // PlateAxis::Y -- for Y, "bottom-left"/"top-right" etc. read as
        // -X-Z/+X+Z corners instead, there being no true up/down on a
        // horizontal plate).
        Vector3 corners[4];
        if (item.Axis == PlateAxis::Z)
        {
            corners[0] = Vector3(item.Center.X - halfW, item.Center.Y - halfH, item.Center.Z);
            corners[1] = Vector3(item.Center.X + halfW, item.Center.Y - halfH, item.Center.Z);
            corners[2] = Vector3(item.Center.X + halfW, item.Center.Y + halfH, item.Center.Z);
            corners[3] = Vector3(item.Center.X - halfW, item.Center.Y + halfH, item.Center.Z);
        }
        else if (item.Axis == PlateAxis::X)
        {
            corners[0] = Vector3(item.Center.X, item.Center.Y - halfH, item.Center.Z - halfW);
            corners[1] = Vector3(item.Center.X, item.Center.Y - halfH, item.Center.Z + halfW);
            corners[2] = Vector3(item.Center.X, item.Center.Y + halfH, item.Center.Z + halfW);
            corners[3] = Vector3(item.Center.X, item.Center.Y + halfH, item.Center.Z - halfW);
        }
        else // PlateAxis::Y
        {
            corners[0] = Vector3(item.Center.X - halfW, item.Center.Y, item.Center.Z - halfH);
            corners[1] = Vector3(item.Center.X + halfW, item.Center.Y, item.Center.Z - halfH);
            corners[2] = Vector3(item.Center.X + halfW, item.Center.Y, item.Center.Z + halfH);
            corners[3] = Vector3(item.Center.X - halfW, item.Center.Y, item.Center.Z + halfH);
        }

        const Vector2 uvs[4] = {
            {item.Uv.U0, item.Uv.V0},
            {item.Uv.U1, item.Uv.V0},
            {item.Uv.U1, item.Uv.V1},
            {item.Uv.U0, item.Uv.V1},
        };
        AppendDoubleSidedQuad(corners, uvs, vertices, indices);
    }

    void AppendTripleCrossMesh(const TripleCrossItem& item,
                               std::vector<CubeVertex>& vertices,
                               std::vector<std::uint32_t>& indices)
    {
        constexpr float kAnglesDeg[3] = {0.0f, 60.0f, 120.0f};
        constexpr float kPi = 3.14159265358979323846f;
        const float halfW = item.Width * 0.5f;
        const float halfH = item.Height * 0.5f;
        const Vector2 uvs[4] = {
            {item.Uv.U0, item.Uv.V0},
            {item.Uv.U1, item.Uv.V0},
            {item.Uv.U1, item.Uv.V1},
            {item.Uv.U0, item.Uv.V1},
        };

        for (float deg : kAnglesDeg)
        {
            const float rad = deg * kPi / 180.0f;
            const float dx = std::cos(rad) * halfW;
            const float dz = std::sin(rad) * halfW;
            const Vector3 corners[4] = {
                Vector3(item.Center.X - dx, item.Center.Y - halfH, item.Center.Z - dz),
                Vector3(item.Center.X + dx, item.Center.Y - halfH, item.Center.Z + dz),
                Vector3(item.Center.X + dx, item.Center.Y + halfH, item.Center.Z + dz),
                Vector3(item.Center.X - dx, item.Center.Y + halfH, item.Center.Z - dz),
            };
            AppendDoubleSidedQuad(corners, uvs, vertices, indices);
        }
    }
}
