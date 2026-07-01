// SPDX-License-Identifier: MIT
#pragma once

/// @file CubeBatch.hpp
/// @brief Collects axis-aligned cube/tile items for later drawing.

#include <cstddef>
#include <vector>

#include <Microsoft/Xna/Framework/Vector3.hpp>

namespace Easy3D
{
    /// @brief One queued axis-aligned cube/tile: world center and size.
    struct CubeItem
    {
        Microsoft::Xna::Framework::Vector3 Center;
        Microsoft::Xna::Framework::Vector3 Size;
    };

    /// @brief Collects axis-aligned cubes / tiles for later drawing.
    ///
    /// The intended use is simple cube/tile terrain (e.g. a Blupi level
    /// rendered as blocks). Add() stores one `CubeItem` per call; Items()
    /// exposes them so a future CNA draw path can build vertex/index buffers
    /// once the CNA draw-path decision is made (see docs/ROADMAP.md). This
    /// class still does no GPU work itself.
    class CubeBatch
    {
    public:
        using Vector3 = Microsoft::Xna::Framework::Vector3;

        CubeBatch() = default;

        /// @brief Discard any queued cubes.
        void Begin() noexcept { m_items.clear(); }

        /// @brief Queue one cube centred at @p center with the given @p size.
        /// @note No GPU work yet; only records the item.
        void Add(const Vector3& center, const Vector3& size);

        /// @brief Flush queued cubes (no-op; no GPU work yet).
        void End() noexcept {}

        [[nodiscard]] std::size_t Count() const noexcept { return m_items.size(); }

        /// @brief The queued cubes, in `Add()` order.
        [[nodiscard]] const std::vector<CubeItem>& Items() const noexcept { return m_items; }

    private:
        std::vector<CubeItem> m_items;
    };
}
