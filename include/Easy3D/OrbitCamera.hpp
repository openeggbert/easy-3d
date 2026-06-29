// SPDX-License-Identifier: MIT
#pragma once

/// @file OrbitCamera.hpp
/// @brief Orbit (turntable) camera: yaw/pitch/distance around a target point.

#include <Microsoft/Xna/Framework/Vector3.hpp>

namespace Easy3D
{
    class Camera3D;

    /// @brief Computes an eye position orbiting a target.
    ///
    /// Stores a @p target and spherical orbit parameters (@p yaw, @p pitch,
    /// @p distance). It does not own a Camera3D; call ApplyTo() to push the
    /// computed position/target into one.
    class OrbitCamera
    {
    public:
        using Vector3 = Microsoft::Xna::Framework::Vector3;

        OrbitCamera();

        [[nodiscard]] const Vector3& GetTarget() const noexcept { return m_target; }
        void SetTarget(const Vector3& value) noexcept { m_target = value; }

        /// @brief Horizontal angle around the target, in radians.
        [[nodiscard]] float GetYaw() const noexcept { return m_yaw; }
        void SetYaw(float radians) noexcept { m_yaw = radians; }

        /// @brief Vertical angle above the target's horizontal plane, in radians.
        [[nodiscard]] float GetPitch() const noexcept { return m_pitch; }
        void SetPitch(float radians) noexcept { m_pitch = radians; }

        [[nodiscard]] float GetDistance() const noexcept { return m_distance; }
        void SetDistance(float value) noexcept { m_distance = value; }

        /// @brief The eye position implied by target + yaw/pitch/distance.
        /// @note Requires linking CNA (constructs a CNA Vector3).
        [[nodiscard]] Vector3 ComputePosition() const;

        /// @brief Push ComputePosition()/target into a Camera3D.
        /// @note Requires linking CNA.
        void ApplyTo(Camera3D& camera) const;

    private:
        Vector3 m_target;
        float m_yaw;       ///< radians
        float m_pitch;     ///< radians
        float m_distance;
    };
}
