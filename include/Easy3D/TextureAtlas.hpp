// SPDX-License-Identifier: MIT
#pragma once

/// @file TextureAtlas.hpp
/// @brief Named sub-rectangles within a texture, plus normalized UV lookup.

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Easy3D
{
    /// @brief A pixel rectangle inside an atlas texture.
    struct AtlasRect
    {
        int X = 0;
        int Y = 0;
        int Width = 0;
        int Height = 0;
    };

    /// @brief Normalized texture coordinates: (U0,V0) top-left, (U1,V1) bottom-right.
    struct UvRect
    {
        float U0 = 0.0f;
        float V0 = 0.0f;
        float U1 = 0.0f;
        float V1 = 0.0f;
    };

    /// @brief Maps names to sub-rectangles of a single atlas texture.
    ///
    /// Deliberately small and CNA-free: it deals only in plain pixel rectangles
    /// and normalized UVs (`AtlasRect`/`UvRect` are Easy3D PODs, since pixel/UV
    /// rectangles are not CNA concepts). Loading the actual texture is the
    /// caller's job (via CNA); this class only remembers where each region lives.
    class TextureAtlas
    {
    public:
        TextureAtlas() = default;
        TextureAtlas(int atlasWidth, int atlasHeight) noexcept
            : m_atlasWidth(atlasWidth), m_atlasHeight(atlasHeight) {}

        void SetAtlasSize(int width, int height) noexcept
        {
            m_atlasWidth = width;
            m_atlasHeight = height;
        }

        [[nodiscard]] int AtlasWidth() const noexcept { return m_atlasWidth; }
        [[nodiscard]] int AtlasHeight() const noexcept { return m_atlasHeight; }

        /// @brief Register (or overwrite) a named region.
        void Add(std::string name, const AtlasRect& rect);

        /// @brief Register a grid of named regions from a spritesheet, e.g.
        /// `prefix + "_0"`, `prefix + "_1"`, ... in row-major order (first row
        /// left to right, then the next row).
        ///
        /// Frame @c i sits at pixel `(startX + col*(frameWidth+spacingX),
        /// startY + row*(frameHeight+spacingY))` with size `frameWidth x
        /// frameHeight`, where `col = i % columns`, `row = i / columns`.
        /// Internally calls `Add()` for each frame, so it shares `Add()`'s
        /// overwrite-on-duplicate-name behavior.
        /// @throws std::invalid_argument if @p prefix is empty, @p frameWidth,
        ///         @p frameHeight, @p columns, or @p rows is <= 0, or
        ///         @p startX/@p startY/@p spacingX/@p spacingY is negative.
        void AddGrid(std::string_view prefix,
                     int frameWidth, int frameHeight,
                     int columns, int rows,
                     int startX = 0, int startY = 0,
                     int spacingX = 0, int spacingY = 0);

        [[nodiscard]] bool Contains(std::string_view name) const noexcept;

        /// @brief Look up a region by name.
        /// @throws std::out_of_range if @p name is unknown.
        [[nodiscard]] const AtlasRect& GetRect(std::string_view name) const;

        /// @brief Normalized UVs for a region, using the current atlas size.
        /// @throws std::out_of_range if @p name is unknown.
        [[nodiscard]] UvRect GetUv(std::string_view name) const;

        /// @brief Normalized UVs for a region, or @p fallback if @p name is
        /// unknown. Never throws.
        [[nodiscard]] UvRect GetUvOrDefault(std::string_view name, const UvRect& fallback = {}) const noexcept;

        [[nodiscard]] std::size_t Count() const noexcept { return m_regions.size(); }

        [[nodiscard]] bool Empty() const noexcept { return m_regions.empty(); }

    private:
        int m_atlasWidth = 0;
        int m_atlasHeight = 0;
        std::unordered_map<std::string, AtlasRect> m_regions;
    };
}
