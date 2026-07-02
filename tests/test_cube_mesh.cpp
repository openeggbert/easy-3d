// SPDX-License-Identifier: MIT
//
// Tests for Easy3D::AppendCubeMesh/BuildCubeMesh (Roadmap Phase 3 vertex
// builder). Requires linking CNA (-DEASY3D_LINK_CNA=ON): constructing
// Vector3(x,y,z) values and doing Vector3 arithmetic calls CNA's compiled
// operators.

#include "Easy3D/CubeMesh.hpp"

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
    using Vector3 = Microsoft::Xna::Framework::Vector3;

    // --- AppendCubeMesh: one unit cube at the origin, default UV -----------
    {
        Easy3D::CubeItem item;
        item.Center = Vector3(0.0f, 0.0f, 0.0f);
        item.Size = Vector3(2.0f, 2.0f, 2.0f);
        // Uv defaults to {0,0,1,1}.

        std::vector<Easy3D::CubeVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::AppendCubeMesh(item, vertices, indices);

        // 6 faces x 4 vertices, 6 faces x 6 indices (2 triangles each).
        CHECK(vertices.size() == 24);
        CHECK(indices.size() == 36);

        // Every position component is within [-1, 1] (half-extent of a 2x2x2
        // cube centered at the origin).
        for (const auto& v : vertices) {
            CHECK(v.Position.X >= -1.0001f && v.Position.X <= 1.0001f);
            CHECK(v.Position.Y >= -1.0001f && v.Position.Y <= 1.0001f);
            CHECK(v.Position.Z >= -1.0001f && v.Position.Z <= 1.0001f);
        }

        // Every index refers to a valid vertex.
        for (auto i : indices) {
            CHECK(i < vertices.size());
        }

        // The +Z face is emitted first: (-1,-1,1), (1,-1,1), (1,1,1), (-1,1,1).
        CHECK(approx(vertices[0].Position.X, -1.0f) && approx(vertices[0].Position.Y, -1.0f) && approx(vertices[0].Position.Z, 1.0f));
        CHECK(approx(vertices[1].Position.X, 1.0f) && approx(vertices[1].Position.Y, -1.0f) && approx(vertices[1].Position.Z, 1.0f));
        CHECK(approx(vertices[2].Position.X, 1.0f) && approx(vertices[2].Position.Y, 1.0f) && approx(vertices[2].Position.Z, 1.0f));
        CHECK(approx(vertices[3].Position.X, -1.0f) && approx(vertices[3].Position.Y, 1.0f) && approx(vertices[3].Position.Z, 1.0f));

        // First triangle of the +Z face: base+0, base+1, base+2.
        CHECK(indices[0] == 0 && indices[1] == 1 && indices[2] == 2);
        CHECK(indices[3] == 0 && indices[4] == 2 && indices[5] == 3);

        // Default UV corners: (0,0), (1,0), (1,1), (0,1), repeated per face.
        for (int face = 0; face < 6; ++face) {
            const auto& a = vertices[face * 4 + 0].Uv;
            const auto& b = vertices[face * 4 + 1].Uv;
            const auto& c = vertices[face * 4 + 2].Uv;
            const auto& d = vertices[face * 4 + 3].Uv;
            CHECK(approx(a.X, 0.0f) && approx(a.Y, 0.0f));
            CHECK(approx(b.X, 1.0f) && approx(b.Y, 0.0f));
            CHECK(approx(c.X, 1.0f) && approx(c.Y, 1.0f));
            CHECK(approx(d.X, 0.0f) && approx(d.Y, 1.0f));
        }
    }

    // --- AppendCubeMesh: custom UV, off-center cube -------------------------
    {
        Easy3D::CubeItem item;
        item.Center = Vector3(5.0f, 1.0f, -2.0f);
        item.Size = Vector3(1.0f, 1.0f, 1.0f);
        item.Uv = Easy3D::UvRect{0.25f, 0.0f, 0.5f, 0.5f};

        std::vector<Easy3D::CubeVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::AppendCubeMesh(item, vertices, indices);

        CHECK(vertices.size() == 24);
        for (const auto& v : vertices) {
            CHECK(v.Position.X >= 4.4999f && v.Position.X <= 5.5001f);
            CHECK(v.Position.Y >= 0.4999f && v.Position.Y <= 1.5001f);
            CHECK(v.Position.Z >= -2.5001f && v.Position.Z <= -1.4999f);
        }
        // The custom UV rect is used on every face.
        for (const auto& v : vertices) {
            CHECK((approx(v.Uv.X, 0.25f) || approx(v.Uv.X, 0.5f)));
            CHECK((approx(v.Uv.Y, 0.0f) || approx(v.Uv.Y, 0.5f)));
        }
    }

    // --- BuildCubeMesh: concatenates every item in Items() order, with -----
    // --- correctly offset indices -------------------------------------------
    {
        Easy3D::CubeBatch batch;
        batch.Add(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
        batch.Add(Vector3(10.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));

        std::vector<Easy3D::CubeVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::BuildCubeMesh(batch, vertices, indices);

        CHECK(vertices.size() == 48);
        CHECK(indices.size() == 72);

        // Second cube's indices are offset past the first cube's 24 vertices.
        for (std::size_t i = 36; i < indices.size(); ++i) {
            CHECK(indices[i] >= 24 && indices[i] < 48);
        }

        // First cube is centered at X=0, second at X=10 — vertex sets don't overlap.
        for (std::size_t i = 0; i < 24; ++i) {
            CHECK(vertices[i].Position.X >= -0.5001f && vertices[i].Position.X <= 0.5001f);
        }
        for (std::size_t i = 24; i < 48; ++i) {
            CHECK(vertices[i].Position.X >= 9.4999f && vertices[i].Position.X <= 10.5001f);
        }
    }

    // --- BuildCubeMesh on an empty batch produces empty output -------------
    {
        Easy3D::CubeBatch batch;
        std::vector<Easy3D::CubeVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::BuildCubeMesh(batch, vertices, indices);
        CHECK(vertices.empty());
        CHECK(indices.empty());
    }

    if (g_failures == 0) {
        std::printf("easy3d cube mesh test: OK\n");
    }
    return g_failures == 0 ? 0 : 1;
}
