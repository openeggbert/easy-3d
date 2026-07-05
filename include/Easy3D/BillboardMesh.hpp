// SPDX-License-Identifier: MIT
#pragma once

/// @file BillboardMesh.hpp
/// @brief Turns queued `BillboardBatch` items into plain CPU-side vertex/index
/// arrays, camera-facing (mirrors CubeMesh.hpp's role for CubeBatch). No GPU
/// work: no `GraphicsDevice`, no vertex/index *buffers*, no shaders — just
/// `std::vector` data a caller can later upload. Unlike a cube, a billboard's
/// vertex positions depend on the camera (right/up basis), so the caller must
/// rebuild this mesh whenever the camera moves (typically once per frame) --
/// there is no camera-independent cache like CubeMesh has.

#include <cstdint>
#include <vector>

#include <Microsoft/Xna/Framework/Vector2.hpp>
#include <Microsoft/Xna/Framework/Vector3.hpp>

#include "Easy3D/BillboardBatch.hpp"

namespace Easy3D
{
    /// @brief One mesh vertex: world position + normalized texture coordinate.
    struct BillboardVertex
    {
        Microsoft::Xna::Framework::Vector3 Position;
        Microsoft::Xna::Framework::Vector2 Uv;
    };

    /// @brief Appends one billboard's quad (4 vertices, 6 indices) to existing
    /// output arrays, facing the camera described by @p cameraRight / @p
    /// cameraUp (both expected normalized — e.g. a view matrix's inverse
    /// Right/Up rows). Indices are offset by the vertex count already present
    /// in @p vertices, so results from repeated calls concatenate correctly
    /// into one combined mesh.
    ///
    /// @p item.Origin (normalized, {0,0}=bottom-left, {1,1}=top-right,
    /// {0.5,0.5}=centered) places the quad relative to @p item.Position;
    /// @p item.RotationRadians rotates the quad within the camera's
    /// right/up plane before that placement.
    void AppendBillboardMesh(const BillboardItem& item,
                             const Microsoft::Xna::Framework::Vector3& cameraRight,
                             const Microsoft::Xna::Framework::Vector3& cameraUp,
                             std::vector<BillboardVertex>& vertices,
                             std::vector<std::uint32_t>& indices);

    /// @brief Builds a combined triangle mesh for every item in @p batch, in
    /// `BillboardBatch::Items()` order, all facing the same camera basis.
    /// Equivalent to calling `AppendBillboardMesh` for each item in turn
    /// against the same @p vertices / @p indices.
    void BuildBillboardMesh(const BillboardBatch& batch,
                            const Microsoft::Xna::Framework::Vector3& cameraRight,
                            const Microsoft::Xna::Framework::Vector3& cameraUp,
                            std::vector<BillboardVertex>& vertices,
                            std::vector<std::uint32_t>& indices);
}
