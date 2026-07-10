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

        // The +Z face is emitted first, corner order top-left, top-right,
        // bottom-right, bottom-left as seen from outside (+Z looking -Z) --
        // the XNA clockwise-from-outside front-face convention (see
        // ComputeFaceCorners' comment): (-1,1,1), (1,1,1), (1,-1,1), (-1,-1,1).
        CHECK(approx(vertices[0].Position.X, -1.0f) && approx(vertices[0].Position.Y, 1.0f) && approx(vertices[0].Position.Z, 1.0f));
        CHECK(approx(vertices[1].Position.X, 1.0f) && approx(vertices[1].Position.Y, 1.0f) && approx(vertices[1].Position.Z, 1.0f));
        CHECK(approx(vertices[2].Position.X, 1.0f) && approx(vertices[2].Position.Y, -1.0f) && approx(vertices[2].Position.Z, 1.0f));
        CHECK(approx(vertices[3].Position.X, -1.0f) && approx(vertices[3].Position.Y, -1.0f) && approx(vertices[3].Position.Z, 1.0f));

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

    // --- AppendDirectionalCubeMesh: all 6 faces visible, default UV --------
    // behaves like AppendCubeMesh with a full-texture UV on every face.
    {
        Easy3D::DirectionalCubeItem item;
        item.Center = Vector3(0.0f, 0.0f, 0.0f);
        item.Size = Vector3(2.0f, 2.0f, 2.0f);
        // Faces[] default-construct to {Visible=true, Uv={0,0,1,1}}.

        std::vector<Easy3D::CubeVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::AppendDirectionalCubeMesh(item, vertices, indices);

        CHECK(vertices.size() == 24);
        CHECK(indices.size() == 36);
        for (const auto& v : vertices) {
            CHECK(v.Position.X >= -1.0001f && v.Position.X <= 1.0001f);
            CHECK(v.Position.Y >= -1.0001f && v.Position.Y <= 1.0001f);
            CHECK(v.Position.Z >= -1.0001f && v.Position.Z <= 1.0001f);
        }
    }

    // --- AppendDirectionalCubeMesh: DirectionalCube's actual use case ------
    // (icon 200 "Platform"/grate: 4 side faces textured, top/bottom
    // genuinely open -- omitted, not just untextured) -- 4 faces emitted,
    // not 6, and top/bottom contribute no geometry at all.
    {
        Easy3D::DirectionalCubeItem item;
        item.Center = Vector3(0.0f, 0.0f, 0.0f);
        item.Size = Vector3(1.0f, 1.0f, 1.0f);
        const Easy3D::UvRect sideUv{0.1f, 0.2f, 0.3f, 0.4f};
        for (int face = 0; face < 6; ++face) {
            const bool isTopOrBottom =
                face == static_cast<int>(Easy3D::CubeFace::PosY) ||
                face == static_cast<int>(Easy3D::CubeFace::NegY);
            item.Faces[face].Visible = !isTopOrBottom;
            item.Faces[face].Uv = sideUv;
        }

        std::vector<Easy3D::CubeVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::AppendDirectionalCubeMesh(item, vertices, indices);

        // 4 visible faces x 4 vertices, 4 faces x 6 indices -- not 6 faces.
        CHECK(vertices.size() == 16);
        CHECK(indices.size() == 24);

        // No emitted face is a whole top/bottom face -- side faces legally
        // touch Y=+-0.5 at two of their four corners, so the real invariant
        // is "no group of 4 consecutive vertices (one face) has ALL FOUR
        // corners at a constant Y" -- that pattern only occurs for a true
        // top/bottom face, which must be entirely absent here.
        CHECK(vertices.size() % 4 == 0);
        for (std::size_t face = 0; face * 4 < vertices.size(); ++face) {
            const float y0 = vertices[face * 4 + 0].Position.Y;
            const bool allSameY = approx(vertices[face * 4 + 1].Position.Y, y0) &&
                                   approx(vertices[face * 4 + 2].Position.Y, y0) &&
                                   approx(vertices[face * 4 + 3].Position.Y, y0);
            CHECK(!(allSameY && (approx(y0, 0.5f) || approx(y0, -0.5f))));
        }

        // Every emitted vertex carries the side faces' UV.
        for (const auto& v : vertices) {
            CHECK((approx(v.Uv.X, 0.1f) || approx(v.Uv.X, 0.3f)));
            CHECK((approx(v.Uv.Y, 0.2f) || approx(v.Uv.Y, 0.4f)));
        }

        // Every index refers to a valid vertex.
        for (auto i : indices) {
            CHECK(i < vertices.size());
        }
    }

    // --- AppendDirectionalCubeMesh: all faces invisible produces nothing ---
    {
        Easy3D::DirectionalCubeItem item;
        item.Center = Vector3(0.0f, 0.0f, 0.0f);
        item.Size = Vector3(1.0f, 1.0f, 1.0f);
        for (auto& face : item.Faces) {
            face.Visible = false;
        }

        std::vector<Easy3D::CubeVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::AppendDirectionalCubeMesh(item, vertices, indices);
        CHECK(vertices.empty());
        CHECK(indices.empty());
    }

    // --- AppendDirectionalCubeMesh: per-face UV, one face at a time --------
    // (each face can carry a genuinely different UV, unlike CubeItem's
    // single shared Uv).
    {
        Easy3D::DirectionalCubeItem item;
        item.Center = Vector3(0.0f, 0.0f, 0.0f);
        item.Size = Vector3(1.0f, 1.0f, 1.0f);
        for (auto& face : item.Faces) {
            face.Visible = false;
        }
        item.Faces[static_cast<int>(Easy3D::CubeFace::PosZ)].Visible = true;
        item.Faces[static_cast<int>(Easy3D::CubeFace::PosZ)].Uv = Easy3D::UvRect{0.6f, 0.7f, 0.8f, 0.9f};

        std::vector<Easy3D::CubeVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::AppendDirectionalCubeMesh(item, vertices, indices);

        CHECK(vertices.size() == 4);
        CHECK(indices.size() == 6);
        for (const auto& v : vertices) {
            CHECK((approx(v.Uv.X, 0.6f) || approx(v.Uv.X, 0.8f)));
            CHECK((approx(v.Uv.Y, 0.7f) || approx(v.Uv.Y, 0.9f)));
            // +Z face sits at Z=+0.5.
            CHECK(approx(v.Position.Z, 0.5f));
        }
    }

    // --- AppendPlateMesh: Z axis -- double-sided, 8 vertices/12 indices ----
    {
        Easy3D::PlateItem item;
        item.Center = Vector3(0.0f, 0.0f, 0.0f);
        item.Width = 1.0f;
        item.Height = 2.0f;
        item.Axis = Easy3D::PlateAxis::Z;
        item.Uv = Easy3D::UvRect{0.0f, 0.0f, 1.0f, 1.0f};

        std::vector<Easy3D::CubeVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::AppendPlateMesh(item, vertices, indices);

        CHECK(vertices.size() == 8);
        CHECK(indices.size() == 12);
        // All vertices lie in the Z=0 plane; X within [-0.5,0.5], Y within [-1,1].
        for (const auto& v : vertices) {
            CHECK(approx(v.Position.Z, 0.0f));
            CHECK(v.Position.X >= -0.5001f && v.Position.X <= 0.5001f);
            CHECK(v.Position.Y >= -1.0001f && v.Position.Y <= 1.0001f);
        }
        for (auto i : indices) {
            CHECK(i < vertices.size());
        }
        // Both windings present: first triangle uses base+0,1,2, second
        // (back) copy uses base+4,6,5 (reversed) for the duplicated verts.
        CHECK(indices[0] == 0 && indices[1] == 1 && indices[2] == 2);
        CHECK(indices[6] == 4 && indices[7] == 6 && indices[8] == 5);
    }

    // --- AppendPlateMesh: X axis -- lies in the X=0 plane instead ----------
    {
        Easy3D::PlateItem item;
        item.Center = Vector3(3.0f, 0.0f, 0.0f);
        item.Width = 1.0f;
        item.Height = 1.0f;
        item.Axis = Easy3D::PlateAxis::X;

        std::vector<Easy3D::CubeVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::AppendPlateMesh(item, vertices, indices);

        CHECK(vertices.size() == 8);
        for (const auto& v : vertices) {
            CHECK(approx(v.Position.X, 3.0f));
        }
    }

    // --- AppendPlateMesh: Y axis -- horizontal, lies in the Y=const plane --
    {
        Easy3D::PlateItem item;
        item.Center = Vector3(0.0f, 2.0f, 0.0f);
        item.Width = 1.0f;
        item.Height = 1.0f;
        item.Axis = Easy3D::PlateAxis::Y;

        std::vector<Easy3D::CubeVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::AppendPlateMesh(item, vertices, indices);

        CHECK(vertices.size() == 8);
        CHECK(indices.size() == 12);
        for (const auto& v : vertices) {
            CHECK(approx(v.Position.Y, 2.0f));
            CHECK(v.Position.X >= -0.5001f && v.Position.X <= 0.5001f);
            CHECK(v.Position.Z >= -0.5001f && v.Position.Z <= 0.5001f);
        }
    }

    // --- AppendTripleCrossMesh: 3 double-sided planes, 24 verts/36 indices -
    {
        Easy3D::TripleCrossItem item;
        item.Center = Vector3(0.0f, 0.0f, 0.0f);
        item.Width = 1.0f;
        item.Height = 1.0f;

        std::vector<Easy3D::CubeVertex> vertices;
        std::vector<std::uint32_t> indices;
        Easy3D::AppendTripleCrossMesh(item, vertices, indices);

        CHECK(vertices.size() == 24);
        CHECK(indices.size() == 36);
        for (auto i : indices) {
            CHECK(i < vertices.size());
        }
        // Every vertex is within the block's half-extent radius (half-width
        // horizontally, half-height vertically) -- no plane extends beyond
        // the block.
        for (const auto& v : vertices) {
            const float horizDist = std::sqrt(v.Position.X * v.Position.X + v.Position.Z * v.Position.Z);
            CHECK(horizDist <= 0.5001f);
            CHECK(v.Position.Y >= -0.5001f && v.Position.Y <= 0.5001f);
        }
        // The first plane (0 deg) lies in the Z=0 plane, same as a PlateAxis::Z plate.
        for (std::size_t i = 0; i < 8; ++i) {
            CHECK(approx(vertices[i].Position.Z, 0.0f));
        }
        // The second plane (60 deg) does NOT lie flat in Z=0 or X=0 --
        // confirms it's actually rotated, not a duplicate of the first.
        bool anyOffAxis = false;
        for (std::size_t i = 8; i < 16; ++i) {
            if (!approx(vertices[i].Position.Z, 0.0f) && !approx(vertices[i].Position.X, 0.0f)) {
                anyOffAxis = true;
            }
        }
        CHECK(anyOffAxis);
    }

    // --- AppendDirectionalCubeMesh: each face's winding matches XNA's
    // front-face convention -- regression test, root-caused 2026-07-10
    // (superseding this test's earlier "empirically fixed, not
    // root-caused" state where only +Y/-Y had been reversed).
    //
    // ALL SIX faces are wound CLOCKWISE as seen from outside the cube: the
    // first triangle's cross(v1-v0, v2-v0) points INWARD, toward the cube's
    // center. Rationale (see ComputeFaceCorners' comment in CubeMesh.cpp
    // for the full writeup): CNA implements genuine XNA culling semantics
    // -- under the default RasterizerState::CullCounterClockwise, the
    // triangles that survive are the ones that appear visually CLOCKWISE
    // on screen (the same convention FNA SpriteBatch quads and XNA's
    // canonical tutorial triangle use). The OpenGL-textbook
    // CCW-from-outside winding this file originally used is the exact
    // OPPOSITE, making every face invisible from outside -- scenes only
    // "looked right" by showing the mirrored interiors of each cube's far
    // faces, which broke down visibly wherever no opposite face existed
    // (galaxy-eggbert 2026-07-10: pillar front faces missing, staircases
    // see-through to the background). A "helpful" cleanup back to the GL
    // textbook convention reintroduces exactly that bug -- keep all 6
    // expected normals INWARD.
    {
        constexpr Easy3D::CubeFace kFaces[6] = {
            Easy3D::CubeFace::PosZ, Easy3D::CubeFace::NegZ,
            Easy3D::CubeFace::PosX, Easy3D::CubeFace::NegX,
            Easy3D::CubeFace::PosY, Easy3D::CubeFace::NegY,
        };
        const Vector3 kExpectedNormal[6] = {
            Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, 1.0f),
            Vector3(-1.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f),
            Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f),
        };
        for (int i = 0; i < 6; ++i) {
            Easy3D::DirectionalCubeItem item;
            item.Center = Vector3(0.0f, 0.0f, 0.0f);
            item.Size = Vector3(1.0f, 1.0f, 1.0f);
            for (auto& face : item.Faces) face.Visible = false;
            item.Faces[static_cast<int>(kFaces[i])].Visible = true;

            std::vector<Easy3D::CubeVertex> vertices;
            std::vector<std::uint32_t> indices;
            Easy3D::AppendDirectionalCubeMesh(item, vertices, indices);

            CHECK(vertices.size() == 4);
            CHECK(indices.size() == 6);
            if (vertices.size() != 4 || indices.size() != 6) {
                continue;
            }

            const Vector3& v0 = vertices[indices[0]].Position;
            const Vector3& v1 = vertices[indices[1]].Position;
            const Vector3& v2 = vertices[indices[2]].Position;
            const Vector3 a = v1 - v0;
            const Vector3 b = v2 - v0;
            const Vector3 normal(a.Y * b.Z - a.Z * b.Y,
                                  a.Z * b.X - a.X * b.Z,
                                  a.X * b.Y - a.Y * b.X);
            const Vector3& expected = kExpectedNormal[i];
            CHECK(approx(normal.X, expected.X) &&
                  approx(normal.Y, expected.Y) &&
                  approx(normal.Z, expected.Z));
        }
    }

    if (g_failures == 0) {
        std::printf("easy3d cube mesh test: OK\n");
    }
    return g_failures == 0 ? 0 : 1;
}
