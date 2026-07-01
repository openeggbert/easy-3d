// SPDX-License-Identifier: MIT
#pragma once

/// @file BillboardBatch.hpp
/// @brief Collects camera-facing billboard quads for later drawing.

#include <cstddef>
#include <vector>

#include <Microsoft/Xna/Framework/Vector3.hpp>
#include <Microsoft/Xna/Framework/Vector2.hpp>

#include "Easy3D/TextureAtlas.hpp"

namespace Easy3D
{
    /// @brief One queued camera-facing quad: world position, size, and the
    /// texture-atlas region to draw on it (see `TextureAtlas::GetUv`).
    struct BillboardItem
    {
        Microsoft::Xna::Framework::Vector3 Position;
        Microsoft::Xna::Framework::Vector2 Size;
        UvRect Uv;
    };

    /// @brief Collects billboards (camera-facing quads) for later drawing.
    ///
    /// The intended use is rendering sprites such as Blupi as billboards from
    /// existing 2D frames (docs/QUESTIONS.md Q7). Add() stores one
    /// `BillboardItem` per call; Items() exposes them so a future CNA draw path
    /// can build vertex/index buffers once the CNA draw-path decision is made
    /// (see docs/ROADMAP.md). This class still does no GPU work itself.
    class BillboardBatch
    {
    public:
        using Vector3 = Microsoft::Xna::Framework::Vector3;
        using Vector2 = Microsoft::Xna::Framework::Vector2;

        BillboardBatch() = default;

        /// @brief Discard any queued billboards.
        void Begin() noexcept { m_items.clear(); }

        /// @brief Queue one billboard at @p worldPosition with the given
        /// @p size, sampling the atlas region @p uv.
        /// @note No GPU work yet; only records the item.
        void Add(const Vector3& worldPosition, const Vector2& size, const UvRect& uv);

        /// @brief Flush queued billboards (no-op; no GPU work yet).
        void End() noexcept {}

        [[nodiscard]] std::size_t Count() const noexcept { return m_items.size(); }

        /// @brief The queued billboards, in `Add()` order.
        [[nodiscard]] const std::vector<BillboardItem>& Items() const noexcept { return m_items; }

    private:
        std::vector<BillboardItem> m_items;
    };
}
