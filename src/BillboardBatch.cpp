// SPDX-License-Identifier: MIT
#include "Easy3D/BillboardBatch.hpp"

namespace Easy3D
{
    void BillboardBatch::Add(const Vector3& worldPosition, const Vector2& size, const UvRect& uv)
    {
        m_items.push_back(BillboardItem{worldPosition, size, uv});
    }
}
