// SPDX-License-Identifier: MIT
#include "Easy3D/DebugDraw.hpp"

namespace Easy3D
{
    void DebugDraw::Line(const Vector3& from, const Vector3& to)
    {
        m_lines.push_back(DebugLine{from, to});
    }

    void DebugDraw::Box(const Vector3& center, const Vector3& size)
    {
        m_boxes.push_back(DebugBox{center, size});
    }
}
