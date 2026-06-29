// SPDX-License-Identifier: MIT
#pragma once

/// @file Camera3D.hpp
/// @brief A minimal free-look 3D camera that produces CNA view/projection matrices.

#include <Microsoft/Xna/Framework/Vector3.hpp>
#include <Microsoft/Xna/Framework/Matrix.hpp>

namespace Easy3D
{
    /// @brief A small, explicit perspective camera.
    ///
    /// Camera3D stores a position/target/up and the usual perspective
    /// parameters, and produces CNA (`Microsoft::Xna::Framework::Matrix`) view
    /// and projection matrices. CNA types are used directly and on purpose:
    /// Easy3D does not hide CNA.
    ///
    /// @note GetViewMatrix()/GetProjectionMatrix() call into CNA's compiled math
    ///       (`Matrix::CreateLookAt`, `Matrix::CreatePerspectiveFieldOfView`),
    ///       so executables that call them must link the `CNA` library.
    class Camera3D
    {
    public:
        using Vector3 = Microsoft::Xna::Framework::Vector3;
        using Matrix = Microsoft::Xna::Framework::Matrix;

        Camera3D();

        // --- Eye/target/up -------------------------------------------------
        [[nodiscard]] const Vector3& GetPosition() const noexcept { return m_position; }
        void SetPosition(const Vector3& value) noexcept { m_position = value; }

        [[nodiscard]] const Vector3& GetTarget() const noexcept { return m_target; }
        void SetTarget(const Vector3& value) noexcept { m_target = value; }

        [[nodiscard]] const Vector3& GetUp() const noexcept { return m_up; }
        void SetUp(const Vector3& value) noexcept { m_up = value; }

        // --- Perspective parameters ---------------------------------------
        /// @brief Vertical field of view, in radians.
        [[nodiscard]] float GetFieldOfView() const noexcept { return m_fieldOfView; }
        void SetFieldOfView(float radians) noexcept { m_fieldOfView = radians; }

        [[nodiscard]] float GetAspectRatio() const noexcept { return m_aspectRatio; }
        void SetAspectRatio(float aspect) noexcept { m_aspectRatio = aspect; }

        [[nodiscard]] float GetNearPlane() const noexcept { return m_nearPlane; }
        void SetNearPlane(float value) noexcept { m_nearPlane = value; }

        [[nodiscard]] float GetFarPlane() const noexcept { return m_farPlane; }
        void SetFarPlane(float value) noexcept { m_farPlane = value; }

        // --- Derived matrices (require linking CNA) ------------------------
        [[nodiscard]] Matrix GetViewMatrix() const;
        [[nodiscard]] Matrix GetProjectionMatrix() const;

    private:
        Vector3 m_position;
        Vector3 m_target;
        Vector3 m_up;
        float m_fieldOfView;  ///< radians
        float m_aspectRatio;
        float m_nearPlane;
        float m_farPlane;
    };
}
