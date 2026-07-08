// SPDX-License-Identifier: MIT
#pragma once

/// @file CubeMesh.hpp
/// @brief Turns queued `CubeBatch` items into plain CPU-side vertex/index
/// arrays (Roadmap Phase 3). No GPU work: no `GraphicsDevice`, no vertex/index
/// *buffers*, no shaders — just `std::vector` data a caller can later upload.

#include <cstdint>
#include <vector>

#include <Microsoft/Xna/Framework/Vector2.hpp>
#include <Microsoft/Xna/Framework/Vector3.hpp>

#include "Easy3D/CubeBatch.hpp"

namespace Easy3D
{
    /// @brief One mesh vertex: world position + normalized texture coordinate.
    struct CubeVertex
    {
        Microsoft::Xna::Framework::Vector3 Position;
        Microsoft::Xna::Framework::Vector2 Uv;
    };

    /// @brief Appends one cube's triangle mesh (24 vertices, 36 indices — 6
    /// faces x 4 vertices, so each face can carry its own UV corners) to
    /// existing output arrays. Indices are offset by the vertex count already
    /// present in @p vertices, so results from repeated calls concatenate
    /// correctly into one combined mesh.
    ///
    /// Every face uses @p item's single `Uv` region on all four of its
    /// corners (U0,V0)-(U1,V0)-(U1,V1)-(U0,V1) — `CubeItem` stores one atlas
    /// region per cube, not one per face. Faces are wound counter-clockwise
    /// as seen from outside the cube, matching CNA/XNA's right-handed
    /// coordinate system (outward face normal = (v1-v0) x (v2-v0)).
    void AppendCubeMesh(const CubeItem& item,
                        std::vector<CubeVertex>& vertices,
                        std::vector<std::uint32_t>& indices);

    /// @brief Builds a combined triangle mesh for every item in @p batch, in
    /// `CubeBatch::Items()` order. Equivalent to calling `AppendCubeMesh` for
    /// each item in turn against the same @p vertices / @p indices.
    void BuildCubeMesh(const CubeBatch& batch,
                       std::vector<CubeVertex>& vertices,
                       std::vector<std::uint32_t>& indices);

    /// @brief Face index into `DirectionalCubeItem::Faces`, in the same order
    /// `AppendCubeMesh` emits faces internally: +Z, -Z, +X, -X, +Y (top), -Y
    /// (bottom).
    enum class CubeFace : int { PosZ = 0, NegZ = 1, PosX = 2, NegX = 3, PosY = 4, NegY = 5 };

    /// @brief One face of a `DirectionalCubeItem` — the "DirectionalCube"
    /// render mode identified by mobile-eggbert-reference/
    /// questionnaire-all-remaining-tiles.md: a cube where only some faces
    /// carry a real texture; a `Visible == false` face is a genuine geometric
    /// hole (e.g. an open grate), not just untextured. Deliberately has no
    /// notion of "flat fallback color" — a caller wanting that effect (e.g.
    /// "same blue as the icon's own background") picks a suitable `Uv`
    /// sub-rect of the existing atlas texture (such as a corner swatch of the
    /// tile's own icon) rather than Easy3D growing a second, color-only
    /// vertex/shader path.
    struct DirectionalCubeFace
    {
        bool Visible = true;
        UvRect Uv{0.0f, 0.0f, 1.0f, 1.0f};
    };

    /// @brief A cube where each of the 6 faces independently chooses its own
    /// UV region and whether it's emitted at all (see `DirectionalCubeFace`).
    struct DirectionalCubeItem
    {
        Microsoft::Xna::Framework::Vector3 Center;
        Microsoft::Xna::Framework::Vector3 Size;
        DirectionalCubeFace Faces[6];
    };

    /// @brief Appends @p item's visible faces to existing output arrays, same
    /// concatenation/offset behavior as `AppendCubeMesh`. Faces with
    /// `Visible == false` are omitted entirely (no vertices/indices emitted
    /// for them) rather than drawn with a degenerate/invisible material.
    void AppendDirectionalCubeMesh(const DirectionalCubeItem& item,
                                   std::vector<CubeVertex>& vertices,
                                   std::vector<std::uint32_t>& indices);
}
