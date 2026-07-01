// SPDX-License-Identifier: MIT
#pragma once

/// @file CubeBatch.hpp
/// @brief Collects axis-aligned cube/tile items for later drawing.

#include <cstddef>
#include <vector>

#include <Microsoft/Xna/Framework/Vector3.hpp>

#include "Easy3D/TextureAtlas.hpp"

namespace Easy3D
{
    /// @brief One queued axis-aligned cube/tile: world center, size, and
    /// texture-atlas region (see `TextureAtlas::GetUv`/`GetUvOrDefault`).
    ///
    /// `Uv` defaults to the full texture (`{0,0,1,1}`).
    struct CubeItem
    {
        Microsoft::Xna::Framework::Vector3 Center;
        Microsoft::Xna::Framework::Vector3 Size;
        UvRect Uv{0.0f, 0.0f, 1.0f, 1.0f};
    };

    /// @brief Collects axis-aligned cubes / tiles for later drawing.
    ///
    /// The intended use is simple cube/tile terrain (e.g. a Blupi level
    /// rendered as blocks). Add() stores one `CubeItem` per call; Items()
    /// exposes them so a future CPU-side vertex builder / CNA draw path can
    /// consume them once that phase starts (see docs/ROADMAP.md). This class
    /// still does no GPU work itself.
    class CubeBatch
    {
    public:
        using Vector3 = Microsoft::Xna::Framework::Vector3;

        CubeBatch() = default;

        /// @brief Discard any queued cubes.
        void Begin() noexcept { m_items.clear(); }

        /// @brief Queue one cube centred at @p center with the given @p size
        /// and a full-texture UV.
        /// @note No GPU work yet; only records the item.
        void Add(const Vector3& center, const Vector3& size);

        /// @brief Queue one cube centred at @p center with the given @p size,
        /// sampling the atlas region @p uv.
        /// @note No GPU work yet; only records the item.
        void Add(const Vector3& center, const Vector3& size, const UvRect& uv);

        /// @brief Queue a fully specified cube item.
        /// @note No GPU work yet; only records the item.
        void Add(const CubeItem& item);

        /// @brief Flush queued cubes (no-op; no GPU work yet).
        void End() noexcept {}

        /// @brief Discard any queued cubes (equivalent to `Begin()`).
        void Clear() noexcept { m_items.clear(); }

        [[nodiscard]] bool Empty() const noexcept { return m_items.empty(); }

        [[nodiscard]] std::size_t Count() const noexcept { return m_items.size(); }

        /// @brief The queued cubes, in `Add()` order.
        [[nodiscard]] const std::vector<CubeItem>& Items() const noexcept { return m_items; }

    private:
        std::vector<CubeItem> m_items;
    };
}
