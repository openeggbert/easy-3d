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
    struct DebugLine
    {
        Microsoft::Xna::Framework::Vector3 From;
        Microsoft::Xna::Framework::Vector3 To;
    };

    /// @brief One queued axis-aligned debug box.
    struct DebugBox
    {
        Microsoft::Xna::Framework::Vector3 Center;
        Microsoft::Xna::Framework::Vector3 Size;
    };

    /// @brief Queues debug primitives (lines, boxes) for development overlays.
    ///
    /// Line()/Box() store one `DebugLine`/`DebugBox` per call; Lines()/Boxes()
    /// expose them so a future CPU-side vertex builder / CNA draw path can
    /// consume them once that phase starts (see docs/ROADMAP.md). This class
    /// still does no GPU work itself.
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

        [[nodiscard]] std::size_t LineCount() const noexcept { return m_lines.size(); }

        [[nodiscard]] std::size_t BoxCount() const noexcept { return m_boxes.size(); }

        [[nodiscard]] std::size_t PrimitiveCount() const noexcept { return m_lines.size() + m_boxes.size(); }

        /// @brief The queued line segments, in `Line()` call order.
        [[nodiscard]] const std::vector<DebugLine>& Lines() const noexcept { return m_lines; }

        /// @brief The queued boxes, in `Box()` call order.
        [[nodiscard]] const std::vector<DebugBox>& Boxes() const noexcept { return m_boxes; }

    private:
        std::vector<DebugLine> m_lines;
        std::vector<DebugBox> m_boxes;
    };
}
