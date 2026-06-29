// SPDX-License-Identifier: MIT
#include "Easy3D/BillboardBatch.hpp"

namespace Easy3D
{
    void BillboardBatch::Add(const Vector3& worldPosition, const Vector2& size)
    {
        // TODO: queue a camera-facing quad once the CNA draw path is wired up.
        (void)worldPosition;
        (void)size;
        ++m_count;
    }
}
