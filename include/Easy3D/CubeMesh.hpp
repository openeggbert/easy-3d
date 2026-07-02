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
}
