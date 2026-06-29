// SPDX-License-Identifier: MIT
#pragma once

/// @file BillboardBatch.hpp
/// @brief Stub for future camera-facing billboard rendering.

#include <cstddef>

#include <Microsoft/Xna/Framework/Vector3.hpp>
#include <Microsoft/Xna/Framework/Vector2.hpp>

namespace Easy3D
{
    /// @brief Collects billboards (camera-facing quads) for later drawing.
    ///
    /// Stub only. The intended use is rendering sprites such as Blupi as
    /// billboards from existing 2D frames. Real rendering will be wired to a CNA
    /// graphics device once the CNA draw path is settled — see docs/ROADMAP.md.
    /// For now it just records how many billboards were queued so the API shape
    /// can be exercised without a renderer.
    class BillboardBatch
    {
    public:
        using Vector3 = Microsoft::Xna::Framework::Vector3;
        using Vector2 = Microsoft::Xna::Framework::Vector2;

        BillboardBatch() = default;

        /// @brief Discard any queued billboards.
        void Begin() noexcept { m_count = 0; }

        /// @brief Queue one billboard at @p worldPosition with the given size.
        /// @note Stub: records the request only; no GPU work yet.
        void Add(const Vector3& worldPosition, const Vector2& size);

        /// @brief Flush queued billboards (no-op stub).
        void End() noexcept {}

        [[nodiscard]] std::size_t Count() const noexcept { return m_count; }

    private:
        std::size_t m_count = 0;
    };
}
