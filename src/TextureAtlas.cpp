// SPDX-License-Identifier: MIT
#include "Easy3D/TextureAtlas.hpp"

#include <stdexcept>

namespace Easy3D
{
    void TextureAtlas::Add(std::string name, const AtlasRect& rect)
    {
        m_regions[std::move(name)] = rect;
    }

    bool TextureAtlas::Contains(std::string_view name) const
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
}
