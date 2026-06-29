// SPDX-License-Identifier: MIT
#include "Easy3D/FollowCamera.hpp"
#include "Easy3D/Camera3D.hpp"

#include <cmath>

namespace Easy3D
{
    FollowCamera::FollowCamera()
        : m_position(Vector3::Zero)
        , m_offset(0.0f, 5.0f, 10.0f)
        , m_smoothing(0.15f)
    {
    }

    void FollowCamera::Update(const Vector3& targetPosition, float deltaSeconds)
    {
        const Vector3 desired(
            targetPosition.X + m_offset.X,
            targetPosition.Y + m_offset.Y,
            targetPosition.Z + m_offset.Z);

        // Treat m_smoothing as the fraction of the gap closed in one 1/60s
        // frame, then make it frame-rate independent via deltaSeconds.
        float t;
        if (m_smoothing >= 1.0f)
        {
            t = 1.0f;
        }
        else if (m_smoothing <= 0.0f)
        {
            t = 0.0f;
        }
        else
        {
            t = 1.0f - std::pow(1.0f - m_smoothing, deltaSeconds * 60.0f);
        }

        m_position = Vector3::Lerp(m_position, desired, t);
    }

    void FollowCamera::ApplyTo(Camera3D& camera, const Vector3& targetPosition) const
    {
        camera.SetPosition(m_position);
        camera.SetTarget(targetPosition);
    }
}
