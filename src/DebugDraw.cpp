// SPDX-License-Identifier: MIT
#include "Easy3D/DebugDraw.hpp"

namespace Easy3D
{
    void DebugDraw::Line(const Vector3& from, const Vector3& to)
    {
        // TODO: queue a debug line once the CNA draw path is wired up.
        (void)from;
        (void)to;
        ++m_primitiveCount;
    }

    void DebugDraw::Box(const Vector3& center, const Vector3& size)
    {
        // TODO: queue a debug box once the CNA draw path is wired up.
        (void)center;
        (void)size;
        ++m_primitiveCount;
    }
}
