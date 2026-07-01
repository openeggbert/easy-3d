// SPDX-License-Identifier: MIT
#include "Easy3D/BillboardBatch.hpp"

namespace Easy3D
{
    void BillboardBatch::Add(const Vector3& position, const Vector2& size)
    {
        m_items.push_back(BillboardItem{position, size});
    }

    void BillboardBatch::Add(const Vector3& position, const Vector2& size, const UvRect& uv)
    {
        m_items.push_back(BillboardItem{position, size, uv});
    }

    void BillboardBatch::Add(const BillboardItem& item)
    {
        m_items.push_back(item);
    }
}
