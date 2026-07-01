// SPDX-License-Identifier: MIT
#include "Easy3D/CubeBatch.hpp"

namespace Easy3D
{
    void CubeBatch::Add(const Vector3& center, const Vector3& size)
    {
        m_items.push_back(CubeItem{center, size});
    }
}
