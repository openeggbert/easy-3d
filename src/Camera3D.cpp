// SPDX-License-Identifier: MIT
#include "Easy3D/Camera3D.hpp"

namespace Easy3D
{
    namespace
    {
        // 45 degrees in radians; avoids pulling in a CNA math-helper constant.
        constexpr float kDefaultFieldOfView = 0.785398163f;
    }

    Camera3D::Camera3D()
        : m_position(0.0f, 2.0f, 5.0f)
        , m_target(Vector3::Zero)
        , m_up(Vector3::Up)
        , m_fieldOfView(kDefaultFieldOfView)
        , m_aspectRatio(16.0f / 9.0f)
        , m_nearPlane(0.1f)
        , m_farPlane(1000.0f)
    {
    }

    Camera3D::Matrix Camera3D::GetViewMatrix() const
    {
        return Matrix::CreateLookAt(m_position, m_target, m_up);
    }

    Camera3D::Matrix Camera3D::GetProjectionMatrix() const
    {
        return Matrix::CreatePerspectiveFieldOfView(
            m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane);
    }
}
