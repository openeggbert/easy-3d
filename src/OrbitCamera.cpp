// SPDX-License-Identifier: MIT
#include "Easy3D/OrbitCamera.hpp"
#include "Easy3D/Camera3D.hpp"

#include <cmath>

namespace Easy3D
{
    OrbitCamera::OrbitCamera()
        : m_target(Vector3::Zero)
        , m_yaw(0.0f)
        , m_pitch(0.3f)
        , m_distance(10.0f)
    {
    }

    OrbitCamera::Vector3 OrbitCamera::ComputePosition() const
    {
        const float cosPitch = std::cos(m_pitch);
        const float sinPitch = std::sin(m_pitch);
        const float cosYaw = std::cos(m_yaw);
        const float sinYaw = std::sin(m_yaw);

        return Vector3(
            m_target.X + m_distance * cosPitch * sinYaw,
            m_target.Y + m_distance * sinPitch,
            m_target.Z + m_distance * cosPitch * cosYaw);
    }

    void OrbitCamera::ApplyTo(Camera3D& camera) const
    {
        camera.SetTarget(m_target);
        camera.SetPosition(ComputePosition());
    }
}
