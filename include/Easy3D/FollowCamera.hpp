// SPDX-License-Identifier: MIT
#pragma once

/// @file FollowCamera.hpp
/// @brief A camera position that smoothly follows a moving target.

#include <Microsoft/Xna/Framework/Vector3.hpp>

namespace Easy3D
{
    class Camera3D;

    /// @brief Smoothly chases `targetPosition + offset`.
    ///
    /// Holds its own current position and lerps toward the desired position each
    /// Update(). @p smoothing in (0, 1] controls responsiveness (1 = snap).
    class FollowCamera
    {
    public:
        using Vector3 = Microsoft::Xna::Framework::Vector3;

        FollowCamera();

        [[nodiscard]] const Vector3& GetOffset() const noexcept { return m_offset; }
        void SetOffset(const Vector3& value) noexcept { m_offset = value; }

        /// @brief Smoothing factor in (0, 1]; higher follows more tightly.
        ///
        /// Defined as the fraction of the remaining gap to the desired position
        /// that is closed in one reference tick of 1/60 s (see
        /// `kSmoothingReferenceFps` in FollowCamera.cpp), independent of the
        /// actual `deltaSeconds` passed to Update(). E.g. `0.15f` behaves the
        /// same (closes 15% of the gap per 1/60 s of elapsed time) whether the
        /// game runs at 30, 60, or 144 fps; only the *reference* rate used to
        /// interpret the value is fixed at 60.
        [[nodiscard]] float GetSmoothing() const noexcept { return m_smoothing; }
        void SetSmoothing(float value) noexcept { m_smoothing = value; }

        [[nodiscard]] const Vector3& GetPosition() const noexcept { return m_position; }
        void SetPosition(const Vector3& value) noexcept { m_position = value; }

        /// @brief Advance the follow position toward @p targetPosition + offset.
        /// @param targetPosition the point being followed (e.g. the player).
        /// @param deltaSeconds   frame time, used to scale the smoothing.
        /// @note Requires linking CNA (uses CNA Vector3 math).
        void Update(const Vector3& targetPosition, float deltaSeconds);

        /// @brief Push the current position into a Camera3D (and aim it at target).
        /// @note Requires linking CNA.
        void ApplyTo(Camera3D& camera, const Vector3& targetPosition) const;

    private:
        Vector3 m_position;
        Vector3 m_offset;
        float m_smoothing;
    };
}
