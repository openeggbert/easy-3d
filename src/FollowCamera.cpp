// SPDX-License-Identifier: MIT
#include "Easy3D/FollowCamera.hpp"
#include "Easy3D/Camera3D.hpp"

#include <cmath>

namespace Easy3D
{
    namespace
    {
        /// @brief The frame rate `m_smoothing` is calibrated against.
        ///
        /// `m_smoothing` means "fraction of the gap closed per 1/60 s", not
        /// "fraction of the gap closed per frame" — this is what makes Update()
        /// frame-rate independent (see the derivation below). The value 60 is
        /// an arbitrary but fixed reference point, kept for continuity with
        /// tuning done when this project's game loop targeted 60 fps; changing
        /// it would change the meaning of every existing `SetSmoothing()` call
        /// site, so treat it as part of the smoothing value's contract, not a
        /// tunable.
        constexpr float kSmoothingReferenceFps = 60.0f;
    }

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

        // Exponential decay re-derived for elapsed time `deltaSeconds`, using
        // m_smoothing as the fraction closed per 1/60s reference tick:
        //   remainingFraction(dt) = (1 - m_smoothing) ^ (dt * kSmoothingReferenceFps)
        //   t(dt)                 = 1 - remainingFraction(dt)
        // This makes two consecutive Update(dt1) + Update(dt2) calls converge to
        // the same position as one Update(dt1 + dt2) call, independent of frame
        // rate (unlike a naive `t = m_smoothing * deltaSeconds`).
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
            t = 1.0f - std::pow(1.0f - m_smoothing, deltaSeconds * kSmoothingReferenceFps);
        }

        m_position = Vector3::Lerp(m_position, desired, t);
    }

    void FollowCamera::ApplyTo(Camera3D& camera, const Vector3& targetPosition) const
    {
        camera.SetPosition(m_position);
        camera.SetTarget(targetPosition);
    }
}
