// SPDX-License-Identifier: MIT
#pragma once

/// @file Version.hpp
/// @brief Compile-time and run-time version information for Easy3D.

#define EASY3D_VERSION_MAJOR 0
#define EASY3D_VERSION_MINOR 1
#define EASY3D_VERSION_PATCH 0

namespace Easy3D
{
    /// @brief Major version component.
    inline constexpr int VersionMajor = EASY3D_VERSION_MAJOR;
    /// @brief Minor version component.
    inline constexpr int VersionMinor = EASY3D_VERSION_MINOR;
    /// @brief Patch version component.
    inline constexpr int VersionPatch = EASY3D_VERSION_PATCH;

    /// @brief A single comparable integer, e.g. 0.1.0 -> 100.
    inline constexpr int VersionNumber() noexcept
    {
        return VersionMajor * 10000 + VersionMinor * 100 + VersionPatch;
    }

    /// @brief Human-readable version string, e.g. "0.1.0".
    /// @note Defined in Version.cpp (does not require linking CNA).
    const char* VersionString() noexcept;
}
