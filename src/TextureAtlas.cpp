// SPDX-License-Identifier: MIT
#include "Easy3D/TextureAtlas.hpp"

#include <stdexcept>

namespace Easy3D
{
    void TextureAtlas::Add(std::string name, const AtlasRect& rect)
    {
        m_regions[std::move(name)] = rect;
    }

    void TextureAtlas::AddGrid(std::string_view prefix,
                               int frameWidth, int frameHeight,
                               int columns, int rows,
                               int startX, int startY,
                               int spacingX, int spacingY)
    {
        if (prefix.empty() || frameWidth <= 0 || frameHeight <= 0 || columns <= 0 || rows <= 0 ||
            startX < 0 || startY < 0 || spacingX < 0 || spacingY < 0)
        {
            throw std::invalid_argument("TextureAtlas::AddGrid: invalid grid parameters");
        }

        const std::string prefixStr(prefix);
        for (int i = 0; i < columns * rows; ++i)
        {
            const int col = i % columns;
            const int row = i / columns;
            Add(prefixStr + "_" + std::to_string(i),
                AtlasRect{
                    startX + col * (frameWidth + spacingX),
                    startY + row * (frameHeight + spacingY),
                    frameWidth,
                    frameHeight,
                });
        }
    }

    bool TextureAtlas::Contains(std::string_view name) const noexcept
    {
        return m_regions.find(std::string(name)) != m_regions.end();
    }

    const AtlasRect& TextureAtlas::GetRect(std::string_view name) const
    {
        const auto it = m_regions.find(std::string(name));
        if (it == m_regions.end())
        {
            throw std::out_of_range("TextureAtlas: unknown region '" + std::string(name) + "'");
        }
        return it->second;
    }

    UvRect TextureAtlas::GetUv(std::string_view name) const
    {
        const AtlasRect& r = GetRect(name);
        if (m_atlasWidth <= 0 || m_atlasHeight <= 0)
        {
            return UvRect{};
        }

        const float w = static_cast<float>(m_atlasWidth);
        const float h = static_cast<float>(m_atlasHeight);
        return UvRect{
            static_cast<float>(r.X) / w,
            static_cast<float>(r.Y) / h,
            static_cast<float>(r.X + r.Width) / w,
            static_cast<float>(r.Y + r.Height) / h,
        };
    }

    UvRect TextureAtlas::GetUvOrDefault(std::string_view name, const UvRect& fallback) const noexcept
    {
        const auto it = m_regions.find(std::string(name));
        if (it == m_regions.end())
        {
            return fallback;
        }
        if (m_atlasWidth <= 0 || m_atlasHeight <= 0)
        {
            return UvRect{};
        }

        const AtlasRect& r = it->second;
        const float w = static_cast<float>(m_atlasWidth);
        const float h = static_cast<float>(m_atlasHeight);
        return UvRect{
            static_cast<float>(r.X) / w,
            static_cast<float>(r.Y) / h,
            static_cast<float>(r.X + r.Width) / w,
            static_cast<float>(r.Y + r.Height) / h,
        };
    }
}
