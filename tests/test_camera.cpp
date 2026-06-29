// SPDX-License-Identifier: MIT
//
// Basic construction / defaults for the camera helpers.
// Requires linking CNA (-DEASY3D_LINK_CNA=ON) because it exercises CNA math.

#include "Easy3D/Camera3D.hpp"
#include "Easy3D/OrbitCamera.hpp"
#include "Easy3D/FollowCamera.hpp"

#include <cmath>
#include <cstdio>

static int g_failures = 0;

#define CHECK(cond)                                                       \
    do {                                                                  \
        if (!(cond)) {                                                    \
            std::printf("FAIL: %s (line %d)\n", #cond, __LINE__);         \
            ++g_failures;                                                 \
        }                                                                 \
    } while (0)

static bool approx(float a, float b) { return std::fabs(a - b) < 1e-4f; }

int main()
{
    using Vector3 = Easy3D::Camera3D::Vector3;

    // --- Camera3D defaults ---------------------------------------------
    Easy3D::Camera3D cam;
    CHECK(approx(cam.GetNearPlane(), 0.1f));
    CHECK(approx(cam.GetFarPlane(), 1000.0f));
    CHECK(cam.GetFieldOfView() > 0.0f);
    CHECK(cam.GetAspectRatio() > 0.0f);

    // --- Camera3D setters ----------------------------------------------
    cam.SetPosition(Vector3(1.0f, 2.0f, 3.0f));
    CHECK(approx(cam.GetPosition().X, 1.0f));
    CHECK(approx(cam.GetPosition().Y, 2.0f));
    CHECK(approx(cam.GetPosition().Z, 3.0f));

    // Matrices should at least be computable.
    const auto view = cam.GetViewMatrix();
    const auto projection = cam.GetProjectionMatrix();
    (void)view;
    (void)projection;

    // --- OrbitCamera: yaw=pitch=0 => target + (0,0,distance) -----------
    Easy3D::OrbitCamera orbit;
    orbit.SetTarget(Vector3::Zero);
    orbit.SetDistance(5.0f);
    orbit.SetYaw(0.0f);
    orbit.SetPitch(0.0f);
    const Vector3 p = orbit.ComputePosition();
    CHECK(approx(p.X, 0.0f));
    CHECK(approx(p.Y, 0.0f));
    CHECK(approx(p.Z, 5.0f));

    orbit.ApplyTo(cam);
    CHECK(approx(cam.GetPosition().Z, 5.0f));

    // --- FollowCamera: smoothing=1 snaps to target+offset --------------
    Easy3D::FollowCamera follow;
    follow.SetPosition(Vector3::Zero);
    follow.SetSmoothing(1.0f);
    follow.Update(Vector3(10.0f, 0.0f, 0.0f), 1.0f / 60.0f);
    CHECK(approx(follow.GetPosition().X, 10.0f + follow.GetOffset().X));

    if (g_failures == 0) {
        std::printf("easy3d camera test: OK\n");
    }
    return g_failures == 0 ? 0 : 1;
}
