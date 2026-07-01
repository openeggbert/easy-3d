// SPDX-License-Identifier: MIT
#pragma once

/// @file DebugDraw.hpp
/// @brief Collects debug primitives (lines, boxes) for development overlays.

#include <cstddef>
#include <vector>

#include <Microsoft/Xna/Framework/Vector3.hpp>

namespace Easy3D
{
    /// @brief One queued debug line segment.
    struct LineItem
    {
        Microsoft::Xna::Framework::Vector3 From;
        Microsoft::Xna::Framework::Vector3 To;
    };

    /// @brief One queued axis-aligned debug box.
    struct BoxItem
    {
        Microsoft::Xna::Framework::Vector3 Center;
        Microsoft::Xna::Framework::Vector3 Size;
    };

    /// @brief Queues debug primitives (lines, boxes) for development overlays.
    ///
    /// Line()/Box() store one `LineItem`/`BoxItem` per call; Lines()/Boxes()
    /// expose them so a future CNA draw path can render them once the CNA
    /// draw-path decision is made (see docs/ROADMAP.md). This class still does
    /// no GPU work itself.
    class DebugDraw
    {
    public:
        using Vector3 = Microsoft::Xna::Framework::Vector3;

        DebugDraw() = default;

        /// @brief Drop all queued primitives.
        void Clear() noexcept { m_lines.clear(); m_boxes.clear(); }

        /// @brief Queue a line segment.
        /// @note No GPU work yet; only records the item.
        void Line(const Vector3& from, const Vector3& to);

        /// @brief Queue an axis-aligned box.
        /// @note No GPU work yet; only records the item.
        void Box(const Vector3& center, const Vector3& size);

        [[nodiscard]] std::size_t PrimitiveCount() const noexcept { return m_lines.size() + m_boxes.size(); }

        /// @brief The queued line segments, in `Line()` call order.
        [[nodiscard]] const std::vector<LineItem>& Lines() const noexcept { return m_lines; }

        /// @brief The queued boxes, in `Box()` call order.
        [[nodiscard]] const std::vector<BoxItem>& Boxes() const noexcept { return m_boxes; }

    private:
        std::vector<LineItem> m_lines;
        std::vector<BoxItem> m_boxes;
    };
}
