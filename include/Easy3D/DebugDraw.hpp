// SPDX-License-Identifier: MIT
#pragma once

/// @file DebugDraw.hpp
/// @brief Stub for future immediate-mode debug drawing (lines, boxes).

#include <cstddef>

#include <Microsoft/Xna/Framework/Vector3.hpp>

namespace Easy3D
{
    /// @brief Queues debug primitives (lines, boxes) for development overlays.
    ///
    /// Stub only. Real drawing will be wired to a CNA graphics device later
    /// (see docs/ROADMAP.md). For now it records how many primitives were
    /// queued so call sites can be written and exercised ahead of the renderer.
    class DebugDraw
    {
    public:
        using Vector3 = Microsoft::Xna::Framework::Vector3;

        DebugDraw() = default;

        /// @brief Drop all queued primitives.
        void Clear() noexcept { m_primitiveCount = 0; }

        /// @brief Queue a line segment. @note Stub: records the request only.
        void Line(const Vector3& from, const Vector3& to);

        /// @brief Queue an axis-aligned box. @note Stub: records the request only.
        void Box(const Vector3& center, const Vector3& size);

        [[nodiscard]] std::size_t PrimitiveCount() const noexcept { return m_primitiveCount; }

    private:
        std::size_t m_primitiveCount = 0;
    };
}
