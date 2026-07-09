// SPDX-License-Identifier: MIT
#include "Easy3D/BillboardMesh.hpp"

#include <cmath>

namespace Easy3D
{
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;

    void AppendBillboardMesh(const BillboardItem& item,
                             const Vector3& cameraRight,
                             const Vector3& cameraUp,
                             std::vector<BillboardVertex>& vertices,
                             std::vector<std::uint32_t>& indices)
    {
        // Corner order (u,v): bottom-left, bottom-right, top-right, top-left --
        // same winding convention as CubeMesh's per-face corners.
        const float corners[4][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

        const float cosR = std::cos(item.RotationRadians);
        const float sinR = std::sin(item.RotationRadians);

        const Vector2 faceUvs[4] = {
            {item.Uv.U0, item.Uv.V1},
            {item.Uv.U1, item.Uv.V1},
            {item.Uv.U1, item.Uv.V0},
            {item.Uv.U0, item.Uv.V0},
        };

        const auto base = static_cast<std::uint32_t>(vertices.size());
        for (int i = 0; i < 4; ++i)
        {
            const float du = (corners[i][0] - item.Origin.X) * item.Size.X;
            const float dv = (corners[i][1] - item.Origin.Y) * item.Size.Y;
            const float rdu = du * cosR - dv * sinR;
            const float rdv = du * sinR + dv * cosR;
            const Vector3 pos = item.Position + cameraRight * rdu + cameraUp * rdv;
            vertices.push_back(BillboardVertex{pos, faceUvs[i]});
        }
        // Winding fixed 2026-07-09 (galaxy-eggbert NEXT.md section 8 task 0): the
        // previous order (0,1,2 / 0,2,3) is back-facing under the common
        // default CullCounterClockwise rasterizer state for a billboard
        // whose quad sits in front of a camera looking toward it -- found
        // live when a background billboard rendered invisible until a
        // per-draw CullNone override was applied. This reversed order
        // (0,2,1 / 0,3,2) makes billboards visible under that default
        // without any caller-side cull-state workaround. Vertex data/UVs
        // are unchanged, only triangle assembly order.
        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 1);
        indices.push_back(base + 0);
        indices.push_back(base + 3);
        indices.push_back(base + 2);
    }

    void BuildBillboardMesh(const BillboardBatch& batch,
                            const Vector3& cameraRight,
                            const Vector3& cameraUp,
                            std::vector<BillboardVertex>& vertices,
                            std::vector<std::uint32_t>& indices)
    {
        for (const BillboardItem& item : batch.Items())
        {
            AppendBillboardMesh(item, cameraRight, cameraUp, vertices, indices);
        }
    }
}
