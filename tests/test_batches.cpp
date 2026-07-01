// SPDX-License-Identifier: MIT
//
// Tests for the non-rendering item-storage batches: BillboardBatch, CubeBatch,
// DebugDraw. Requires linking CNA (-DEASY3D_LINK_CNA=ON): constructing
// Vector3(x,y,z)/Vector2(x,y) values calls CNA's compiled constructors.

#include "Easy3D/BillboardBatch.hpp"
#include "Easy3D/CubeBatch.hpp"
#include "Easy3D/DebugDraw.hpp"

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
    using Vector3 = Easy3D::BillboardBatch::Vector3;
    using Vector2 = Easy3D::BillboardBatch::Vector2;

    // ==================== BillboardBatch ====================

    // --- New batch is empty ------------------------------------------------
    Easy3D::BillboardBatch billboards;
    CHECK(billboards.Count() == 0);
    CHECK(billboards.Empty());
    CHECK(billboards.Items().empty());

    // --- Add(position, size) stores one item with defaults ------------------
    billboards.Add(Vector3(1.0f, 2.0f, 3.0f), Vector2(4.0f, 5.0f));
    CHECK(billboards.Count() == 1);
    CHECK(!billboards.Empty());
    {
        const auto& item = billboards.Items()[0];
        CHECK(approx(item.Position.X, 1.0f) && approx(item.Position.Y, 2.0f) && approx(item.Position.Z, 3.0f));
        CHECK(approx(item.Size.X, 4.0f) && approx(item.Size.Y, 5.0f));
        // Default UV is full texture.
        CHECK(approx(item.Uv.U0, 0.0f) && approx(item.Uv.V0, 0.0f));
        CHECK(approx(item.Uv.U1, 1.0f) && approx(item.Uv.V1, 1.0f));
        // Default origin is center.
        CHECK(approx(item.Origin.X, 0.5f) && approx(item.Origin.Y, 0.5f));
        // Default rotation is 0.
        CHECK(approx(item.RotationRadians, 0.0f));
    }

    // --- Add(position, size, customUv) stores the custom UV -----------------
    const Easy3D::UvRect customUv{0.25f, 0.0f, 0.5f, 1.0f};
    billboards.Add(Vector3(6.0f, 7.0f, 8.0f), Vector2(9.0f, 10.0f), customUv);
    CHECK(billboards.Count() == 2);
    {
        const auto& item = billboards.Items()[1];
        CHECK(approx(item.Position.X, 6.0f));
        CHECK(approx(item.Uv.U0, 0.25f) && approx(item.Uv.U1, 0.5f));
        // Origin/rotation still default even with an explicit UV.
        CHECK(approx(item.Origin.X, 0.5f) && approx(item.Origin.Y, 0.5f));
        CHECK(approx(item.RotationRadians, 0.0f));
    }

    // --- Add(BillboardItem) preserves origin and rotation -------------------
    Easy3D::BillboardItem fullItem;
    fullItem.Position = Vector3(11.0f, 12.0f, 13.0f);
    fullItem.Size = Vector2(14.0f, 15.0f);
    fullItem.Uv = Easy3D::UvRect{0.0f, 0.5f, 0.5f, 1.0f};
    fullItem.Origin = Vector2(0.0f, 1.0f);
    fullItem.RotationRadians = 1.5f;
    billboards.Add(fullItem);
    CHECK(billboards.Count() == 3);
    {
        const auto& item = billboards.Items()[2];
        CHECK(approx(item.Position.X, 11.0f));
        CHECK(approx(item.Origin.X, 0.0f) && approx(item.Origin.Y, 1.0f));
        CHECK(approx(item.RotationRadians, 1.5f));
    }

    // --- Items() preserves insertion order -----------------------------------
    CHECK(approx(billboards.Items()[0].Position.X, 1.0f));
    CHECK(approx(billboards.Items()[1].Position.X, 6.0f));
    CHECK(approx(billboards.Items()[2].Position.X, 11.0f));

    // --- Begin() clears old items --------------------------------------------
    billboards.Begin();
    CHECK(billboards.Count() == 0);
    CHECK(billboards.Empty());
    CHECK(billboards.Items().empty());

    // --- Clear() clears old items --------------------------------------------
    billboards.Add(Vector3(1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));
    CHECK(billboards.Count() == 1);
    billboards.Clear();
    CHECK(billboards.Count() == 0);
    CHECK(billboards.Empty());

    // End() is a documented no-op; just confirm it doesn't crash or mutate state.
    billboards.Add(Vector3(2.0f, 2.0f, 2.0f), Vector2(1.0f, 1.0f));
    billboards.End();
    CHECK(billboards.Count() == 1);

    // ==================== CubeBatch ====================

    using CubeVector3 = Easy3D::CubeBatch::Vector3;

    // --- New batch is empty --------------------------------------------------
    Easy3D::CubeBatch cubes;
    CHECK(cubes.Count() == 0);
    CHECK(cubes.Empty());
    CHECK(cubes.Items().empty());

    // --- Add(center, size) stores one item with default full-texture UV -----
    cubes.Add(CubeVector3(1.0f, 2.0f, 3.0f), CubeVector3(1.0f, 1.0f, 1.0f));
    CHECK(cubes.Count() == 1);
    CHECK(!cubes.Empty());
    {
        const auto& item = cubes.Items()[0];
        CHECK(approx(item.Center.X, 1.0f) && approx(item.Center.Y, 2.0f) && approx(item.Center.Z, 3.0f));
        CHECK(approx(item.Size.X, 1.0f));
        CHECK(approx(item.Uv.U0, 0.0f) && approx(item.Uv.V0, 0.0f));
        CHECK(approx(item.Uv.U1, 1.0f) && approx(item.Uv.V1, 1.0f));
    }

    // --- Add(center, size, customUv) stores the custom UV --------------------
    const Easy3D::UvRect cubeUv{0.0f, 0.0f, 0.5f, 0.5f};
    cubes.Add(CubeVector3(4.0f, 5.0f, 6.0f), CubeVector3(2.0f, 2.0f, 2.0f), cubeUv);
    CHECK(cubes.Count() == 2);
    {
        const auto& item = cubes.Items()[1];
        CHECK(approx(item.Center.X, 4.0f));
        CHECK(approx(item.Size.X, 2.0f));
        CHECK(approx(item.Uv.U1, 0.5f) && approx(item.Uv.V1, 0.5f));
    }

    // --- Add(CubeItem) preserves all values -----------------------------------
    Easy3D::CubeItem fullCube;
    fullCube.Center = CubeVector3(7.0f, 8.0f, 9.0f);
    fullCube.Size = CubeVector3(3.0f, 3.0f, 3.0f);
    fullCube.Uv = Easy3D::UvRect{0.5f, 0.5f, 1.0f, 1.0f};
    cubes.Add(fullCube);
    CHECK(cubes.Count() == 3);
    {
        const auto& item = cubes.Items()[2];
        CHECK(approx(item.Center.X, 7.0f));
        CHECK(approx(item.Size.X, 3.0f));
        CHECK(approx(item.Uv.U0, 0.5f) && approx(item.Uv.U1, 1.0f));
    }

    // --- Items() preserves insertion order -------------------------------------
    CHECK(approx(cubes.Items()[0].Center.X, 1.0f));
    CHECK(approx(cubes.Items()[1].Center.X, 4.0f));
    CHECK(approx(cubes.Items()[2].Center.X, 7.0f));

    // --- Begin() clears old items ------------------------------------------------
    cubes.Begin();
    CHECK(cubes.Count() == 0);
    CHECK(cubes.Empty());
    CHECK(cubes.Items().empty());

    // --- Clear() clears old items ------------------------------------------------
    cubes.Add(CubeVector3(1.0f, 1.0f, 1.0f), CubeVector3(1.0f, 1.0f, 1.0f));
    CHECK(cubes.Count() == 1);
    cubes.Clear();
    CHECK(cubes.Count() == 0);
    CHECK(cubes.Empty());

    // ==================== DebugDraw ====================

    using DebugVector3 = Easy3D::DebugDraw::Vector3;

    // --- New DebugDraw has zero lines and boxes -----------------------------
    Easy3D::DebugDraw debug;
    CHECK(debug.LineCount() == 0);
    CHECK(debug.BoxCount() == 0);
    CHECK(debug.PrimitiveCount() == 0);
    CHECK(debug.Lines().empty());
    CHECK(debug.Boxes().empty());

    // --- Line() adds one line -------------------------------------------------
    debug.Line(DebugVector3(0.0f, 0.0f, 0.0f), DebugVector3(1.0f, 0.0f, 0.0f));
    CHECK(debug.LineCount() == 1);
    CHECK(debug.BoxCount() == 0);

    // --- Box() adds one box ---------------------------------------------------
    debug.Box(DebugVector3(2.0f, 0.0f, 0.0f), DebugVector3(1.0f, 1.0f, 1.0f));
    CHECK(debug.LineCount() == 1);
    CHECK(debug.BoxCount() == 1);

    // --- PrimitiveCount() == LineCount() + BoxCount() ---------------------------
    CHECK(debug.PrimitiveCount() == debug.LineCount() + debug.BoxCount());
    CHECK(debug.PrimitiveCount() == 2);

    // --- Lines()/Boxes() preserve insertion values ------------------------------
    debug.Line(DebugVector3(5.0f, 5.0f, 5.0f), DebugVector3(6.0f, 6.0f, 6.0f));
    const auto& lines = debug.Lines();
    const auto& boxes = debug.Boxes();
    CHECK(lines.size() == 2 && boxes.size() == 1);
    CHECK(approx(lines[0].From.X, 0.0f) && approx(lines[0].To.X, 1.0f));
    CHECK(approx(lines[1].From.X, 5.0f) && approx(lines[1].To.X, 6.0f));
    CHECK(approx(boxes[0].Center.X, 2.0f) && approx(boxes[0].Size.X, 1.0f));

    // --- Clear() clears both lines and boxes ------------------------------------
    debug.Clear();
    CHECK(debug.LineCount() == 0);
    CHECK(debug.BoxCount() == 0);
    CHECK(debug.PrimitiveCount() == 0);
    CHECK(debug.Lines().empty());
    CHECK(debug.Boxes().empty());

    if (g_failures == 0) {
        std::printf("easy3d batches test: OK\n");
    }
    return g_failures == 0 ? 0 : 1;
}
