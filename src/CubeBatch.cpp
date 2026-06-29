// SPDX-License-Identifier: MIT
#include "Easy3D/CubeBatch.hpp"

namespace Easy3D
{
    void CubeBatch::Add(const Vector3& center, const Vector3& size)
    {
        // TODO: queue a cube/tile mesh once the CNA draw path is wired up.
        (void)center;
        (void)size;
        ++m_count;
    }
}
