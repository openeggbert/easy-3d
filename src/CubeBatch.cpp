// SPDX-License-Identifier: MIT
#include "Easy3D/CubeBatch.hpp"

namespace Easy3D
{
    void CubeBatch::Add(const Vector3& center, const Vector3& size)
    {
        m_items.push_back(CubeItem{center, size});
    }

    void CubeBatch::Add(const Vector3& center, const Vector3& size, const UvRect& uv)
    {
        m_items.push_back(CubeItem{center, size, uv});
    }

    void CubeBatch::Add(const CubeItem& item)
    {
        m_items.push_back(item);
    }
}
