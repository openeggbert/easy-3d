// SPDX-License-Identifier: MIT
#pragma once

/// @file CubeBatch.hpp
/// @brief Stub for future cube / tile rendering.

#include <cstddef>

#include <Microsoft/Xna/Framework/Vector3.hpp>

namespace Easy3D
{
    /// @brief Collects axis-aligned cubes / tiles for later drawing.
    ///
    /// Stub only. The intended use is simple cube/tile terrain (e.g. a Blupi
    /// level rendered as blocks). Real rendering will be wired to a CNA graphics
    /// device later — see docs/ROADMAP.md. For now it only records how many
    /// cubes were queued.
    class CubeBatch
    {
    public:
        using Vector3 = Microsoft::Xna::Framework::Vector3;

        CubeBatch() = default;

        /// @brief Discard any queued cubes.
        void Begin() noexcept { m_count = 0; }

        /// @brief Queue one cube centred at @p center with the given @p size.
        /// @note Stub: records the request only; no GPU work yet.
        void Add(const Vector3& center, const Vector3& size);

        /// @brief Flush queued cubes (no-op stub).
        void End() noexcept {}

        [[nodiscard]] std::size_t Count() const noexcept { return m_count; }

    private:
        std::size_t m_count = 0;
    };
}
