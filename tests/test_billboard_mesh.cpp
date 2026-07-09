// SPDX-License-Identifier: MIT
//
// Tests for Easy3D::AppendBillboardMesh/BuildBillboardMesh. Requires linking
// CNA (-DEASY3D_LINK_CNA=ON): constructing Vector3(x,y,z) values and doing
// Vector3 arithmetic calls CNA's compiled operators.

#include "Easy3D/BillboardMesh.hpp"

#include <cmath>
#include <cstdio>

static int g_failures = 0;

#define CHECK(cond)                                                       \
    do {                                                                  \
        if (!(cond)) {                                                    \
            std::printf("FAIL: %s (line %d)\n", #cond, __LINE__);         \
            ++g_failures;                                                 \
        }                                                                 \
    } while (0)

static bool approx(float a, float b) { return std::fabs(a - b) < 1e-4f; }

int main()
{
    using Vector2 = Microsoft::Xna::Framework::Vector2;
    using Vector3 = Microsoft::Xna::Framework::Vector3;

    const Vector3 right(1.0f, 0.0f, 0.0f);
    const Vector3 up(0.0f, 1.0f, 0.0f);

    // --- AppendBillboardMesh: centered 2x2 quad at the origin, default UV --
    {
        Easy3D::BillboardItem item;
        item.Position = Vector3(0.0f, 0.0f, 0.0f);
        item.Size = Vector2(2.0f, 2.0f);
        // Origin defaults to {0.5,0.5} (centered); Uv defaults to {0,0,1,1}.

        std::vector<Easy3D::BillboardVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::AppendBillboardMesh(item, right, up, vertices, indices);

        CHECK(vertices.size() == 4);
        CHECK(indices.size() == 6);

        // Corner order: bottom-left, bottom-right, top-right, top-left.
        CHECK(approx(vertices[0].Position.X, -1.0f) && approx(vertices[0].Position.Y, -1.0f));
        CHECK(approx(vertices[1].Position.X, 1.0f) && approx(vertices[1].Position.Y, -1.0f));
        CHECK(approx(vertices[2].Position.X, 1.0f) && approx(vertices[2].Position.Y, 1.0f));
        CHECK(approx(vertices[3].Position.X, -1.0f) && approx(vertices[3].Position.Y, 1.0f));
        // All on the camera plane (no right/up component along the implicit
        // forward axis, since right/up here don't touch Z).
        for (const auto& v : vertices) {
            CHECK(approx(v.Position.Z, 0.0f));
        }

        // Winding reversed 2026-07-09 (0,2,1 / 0,3,2, not 0,1,2 / 0,2,3) --
        // the old order was back-facing under the common default
        // CullCounterClockwise rasterizer state, see BillboardMesh.cpp.
        CHECK(indices[0] == 0 && indices[1] == 2 && indices[2] == 1);
        CHECK(indices[3] == 0 && indices[4] == 3 && indices[5] == 2);

        // World "up" (top corners) maps to texture V0 (top of a top-down
        // image), world "down" maps to V1 -- matches a sprite's usual
        // row-0-is-top convention (see BlockTypes::tileUV's same convention).
        CHECK(approx(vertices[0].Uv.Y, 1.0f)); // bottom-left -> V1
        CHECK(approx(vertices[2].Uv.Y, 0.0f)); // top-right -> V0
        CHECK(approx(vertices[0].Uv.X, 0.0f) && approx(vertices[1].Uv.X, 1.0f));
    }

    // --- Origin (pivot) support: bottom-center anchor, like a standing ------
    // --- character sprite whose feet should sit at Position ----------------
    {
        Easy3D::BillboardItem item;
        item.Position = Vector3(5.0f, 0.0f, -2.0f);
        item.Size = Vector2(2.0f, 4.0f);
        item.Origin = Vector2(0.5f, 0.0f); // bottom-center

        std::vector<Easy3D::BillboardVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::AppendBillboardMesh(item, right, up, vertices, indices);

        // Bottom corners sit exactly at Position.Y (Origin.Y=0 -> dv=0 there).
        CHECK(approx(vertices[0].Position.Y, 0.0f));
        CHECK(approx(vertices[1].Position.Y, 0.0f));
        // Top corners are a full Size.Y (4) above Position.Y.
        CHECK(approx(vertices[2].Position.Y, 4.0f));
        CHECK(approx(vertices[3].Position.Y, 4.0f));
        // Horizontally still centered (Origin.X=0.5).
        CHECK(approx(vertices[0].Position.X, 4.0f) && approx(vertices[1].Position.X, 6.0f));
    }

    // --- BuildBillboardMesh: concatenates every item, with correctly -------
    // --- offset indices ------------------------------------------------------
    {
        Easy3D::BillboardBatch batch;
        batch.Add(Vector3(0.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f));
        batch.Add(Vector3(10.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f));

        std::vector<Easy3D::BillboardVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::BuildBillboardMesh(batch, right, up, vertices, indices);

        CHECK(vertices.size() == 8);
        CHECK(indices.size() == 12);

        for (std::size_t i = 6; i < indices.size(); ++i) {
            CHECK(indices[i] >= 4 && indices[i] < 8);
        }
        for (std::size_t i = 0; i < 4; ++i) {
            CHECK(vertices[i].Position.X >= -0.5001f && vertices[i].Position.X <= 0.5001f);
        }
        for (std::size_t i = 4; i < 8; ++i) {
            CHECK(vertices[i].Position.X >= 9.4999f && vertices[i].Position.X <= 10.5001f);
        }
    }

    // --- BuildBillboardMesh on an empty batch produces empty output --------
    {
        Easy3D::BillboardBatch batch;
        std::vector<Easy3D::BillboardVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::BuildBillboardMesh(batch, right, up, vertices, indices);
        CHECK(vertices.empty());
        CHECK(indices.empty());
    }

    if (g_failures == 0) {
        std::printf("easy3d billboard mesh test: OK\n");
    }
    return g_failures == 0 ? 0 : 1;
}
