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

        [[nodiscard]] bool Contains(std::string_view name) const;

        /// @brief Look up a region by name.
        /// @throws std::out_of_range if @p name is unknown.
        [[nodiscard]] const AtlasRect& GetRect(std::string_view name) const;

        /// @brief Normalized UVs for a region, using the current atlas size.
        /// @throws std::out_of_range if @p name is unknown.
        [[nodiscard]] UvRect GetUv(std::string_view name) const;

        [[nodiscard]] std::size_t Count() const noexcept { return m_regions.size(); }

    private:
        int m_atlasWidth = 0;
        int m_atlasHeight = 0;
        std::unordered_map<std::string, AtlasRect> m_regions;
    };
}
