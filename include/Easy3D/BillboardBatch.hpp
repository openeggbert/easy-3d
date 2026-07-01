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
    /// @brief One queued camera-facing quad: world position, size, texture-atlas
    /// region (see `TextureAtlas::GetUv`/`GetUvOrDefault`), pivot origin, and
    /// in-plane rotation.
    ///
    /// `Uv` defaults to the full texture (`{0,0,1,1}`); `Origin` defaults to
    /// the quad's center (`{0.5f,0.5f}`, normalized); `RotationRadians`
    /// defaults to `0`.
    struct BillboardItem
    {
        Microsoft::Xna::Framework::Vector3 Position;
        Microsoft::Xna::Framework::Vector2 Size;
        UvRect Uv{0.0f, 0.0f, 1.0f, 1.0f};
        Microsoft::Xna::Framework::Vector2 Origin{0.5f, 0.5f};
        float RotationRadians = 0.0f;
    };

    /// @brief Collects billboards (camera-facing quads) for later drawing.
    ///
    /// The intended use is rendering sprites such as Blupi as billboards from
    /// existing 2D frames (docs/QUESTIONS.md Q7). Add() stores one
    /// `BillboardItem` per call; Items() exposes them so a future CPU-side
    /// vertex builder / CNA draw path can consume them once that phase starts
    /// (see docs/ROADMAP.md). This class still does no GPU work itself.
    class BillboardBatch
    {
    public:
        using Vector3 = Microsoft::Xna::Framework::Vector3;
        using Vector2 = Microsoft::Xna::Framework::Vector2;

        BillboardBatch() = default;

        /// @brief Discard any queued billboards.
        void Begin() noexcept { m_items.clear(); }

        /// @brief Queue one billboard at @p position with the given @p size,
        /// full-texture UV, centered origin, and no rotation.
        /// @note No GPU work yet; only records the item.
        void Add(const Vector3& position, const Vector2& size);

        /// @brief Queue one billboard at @p position with the given @p size,
        /// sampling the atlas region @p uv (centered origin, no rotation).
        /// @note No GPU work yet; only records the item.
        void Add(const Vector3& position, const Vector2& size, const UvRect& uv);

        /// @brief Queue a fully specified billboard item.
        /// @note No GPU work yet; only records the item.
        void Add(const BillboardItem& item);

        /// @brief Flush queued billboards (no-op; no GPU work yet).
        void End() noexcept {}

        /// @brief Discard any queued billboards (equivalent to `Begin()`).
        void Clear() noexcept { m_items.clear(); }

        [[nodiscard]] bool Empty() const noexcept { return m_items.empty(); }

        [[nodiscard]] std::size_t Count() const noexcept { return m_items.size(); }

        /// @brief The queued billboards, in `Add()` order.
        [[nodiscard]] const std::vector<BillboardItem>& Items() const noexcept { return m_items; }

    private:
        std::vector<BillboardItem> m_items;
    };
}
