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

    /// @brief Which plane a `PlateItem` lies in. `Z` spans X (`Width`) and Y
    /// (`Height`), facing +Z/-Z (a "north-south wall" reading). `X` spans Z
    /// (`Width`) and Y (`Height`), facing +X/-X (an "east-west wall"
    /// reading). `Y` spans X (`Width`) and Z (`Height`), facing +Y/-Y (a
    /// horizontal "floor/ceiling/tabletop" reading — e.g. a grass-top
    /// surface sitting above an otherwise ordinary block).
    enum class PlateAxis { Z, X, Y };

    /// @brief The "InnerFlatPlate" render mode identified by
    /// mobile-eggbert-reference/questionnaire-all-remaining-tiles.md: a
    /// single flat double-sided plate centered inside an otherwise fully
    /// transparent block (e.g. a signpost, thin post, or screen) — the
    /// block's outer 6 faces are simply never drawn at all by the caller,
    /// this only builds the plate itself. `PlateAxis::Y` (horizontal) is
    /// also used standalone for surface-only effects (e.g. a grass top),
    /// not just `InnerFlatPlate`.
    struct PlateItem
    {
        Microsoft::Xna::Framework::Vector3 Center;
        float Width = 1.0f;  // extent along the plane's first in-plane axis (X for Z/Y axis, Z for X axis)
        float Height = 1.0f; // extent along the plane's second in-plane axis (Y for Z/X axis, Z for Y axis)
        UvRect Uv{0.0f, 0.0f, 1.0f, 1.0f};
        PlateAxis Axis = PlateAxis::Z;
    };

    /// @brief Appends @p item as two coincident quads with opposite winding
    /// (8 vertices, 12 indices) — genuinely double-sided regardless of the
    /// renderer's backface-culling state, unlike a single quad which would
    /// only be visible from one side under standard CCW culling.
    void AppendPlateMesh(const PlateItem& item,
                         std::vector<CubeVertex>& vertices,
                         std::vector<std::uint32_t>& indices);

    /// @brief The "TripleCrossBillboard" render mode identified by
    /// mobile-eggbert-reference/questionnaire-all-remaining-tiles.md: the
    /// same texture drawn on 3 vertical double-sided planes through the
    /// block's center, each 60° apart around Y, forming a triangle in plan
    /// view (a 3-plane generalization of the classic 2-plane "cross"
    /// billboard used for plants). Rotationally symmetric by construction —
    /// no per-icon facing decision is needed, unlike `PlateItem`/
    /// `DirectionalCubeItem`.
    struct TripleCrossItem
    {
        Microsoft::Xna::Framework::Vector3 Center;
        float Width = 1.0f;
        float Height = 1.0f;
        UvRect Uv{0.0f, 0.0f, 1.0f, 1.0f};
    };

    /// @brief Appends @p item as 3 `PlateItem`-style double-sided planes (24
    /// vertices, 36 indices total), each pair of adjacent planes 60° apart
    /// around Y.
    void AppendTripleCrossMesh(const TripleCrossItem& item,
                               std::vector<CubeVertex>& vertices,
                               std::vector<std::uint32_t>& indices);
}
