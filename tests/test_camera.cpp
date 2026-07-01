// SPDX-License-Identifier: MIT
//
// Basic construction / defaults for the camera helpers.
// Requires linking CNA (-DEASY3D_LINK_CNA=ON) because it exercises CNA math.

#include "Easy3D/Camera3D.hpp"
#include "Easy3D/OrbitCamera.hpp"
#include "Easy3D/FollowCamera.hpp"
#include "Easy3D/BillboardBatch.hpp"
#include "Easy3D/CubeBatch.hpp"
#include "Easy3D/DebugDraw.hpp"

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

    // --- FollowCamera: two 1/60s steps converge monotonically -----------
    const Vector3 target(10.0f, 0.0f, 0.0f);
    Easy3D::FollowCamera stepwise;
    stepwise.SetOffset(Vector3::Zero);
    stepwise.SetPosition(Vector3::Zero);
    stepwise.SetSmoothing(0.15f);
    stepwise.Update(target, 1.0f / 60.0f);
    const float afterStep1 = stepwise.GetPosition().X;
    CHECK(afterStep1 > 0.0f && afterStep1 < target.X);
    stepwise.Update(target, 1.0f / 60.0f);
    const float afterStep2 = stepwise.GetPosition().X;
    CHECK(afterStep2 > afterStep1 && afterStep2 < target.X);

    // --- FollowCamera: frame-rate independence ---------------------------
    // Two 1/60s Update() calls must land at the same place as one 2/60s call
    // (this is what the exponential-decay formula in FollowCamera.cpp buys
    // over a naive `t = smoothing * deltaSeconds` lerp).
    Easy3D::FollowCamera bigStep;
    bigStep.SetOffset(Vector3::Zero);
    bigStep.SetPosition(Vector3::Zero);
    bigStep.SetSmoothing(0.15f);
    bigStep.Update(target, 2.0f / 60.0f);
    CHECK(approx(bigStep.GetPosition().X, afterStep2));

    // --- BillboardBatch: queues items, exposes them, Begin() clears --------
    Easy3D::BillboardBatch batch;
    CHECK(batch.Count() == 0);

    const Easy3D::UvRect uvA{0.0f, 0.0f, 0.5f, 1.0f};
    const Easy3D::UvRect uvB{0.5f, 0.0f, 1.0f, 1.0f};
    batch.Add(Vector3(1.0f, 2.0f, 3.0f), Easy3D::BillboardBatch::Vector2(4.0f, 5.0f), uvA);
    batch.Add(Vector3(6.0f, 7.0f, 8.0f), Easy3D::BillboardBatch::Vector2(9.0f, 10.0f), uvB);
    CHECK(batch.Count() == 2);

    const auto& items = batch.Items();
    CHECK(items.size() == 2);
    CHECK(approx(items[0].Position.X, 1.0f) && approx(items[0].Position.Y, 2.0f) && approx(items[0].Position.Z, 3.0f));
    CHECK(approx(items[0].Size.X, 4.0f) && approx(items[0].Size.Y, 5.0f));
    CHECK(approx(items[0].Uv.U0, 0.0f) && approx(items[0].Uv.U1, 0.5f));
    CHECK(approx(items[1].Position.X, 6.0f));
    CHECK(approx(items[1].Uv.U0, 0.5f) && approx(items[1].Uv.U1, 1.0f));

    batch.Begin();
    CHECK(batch.Count() == 0);
    CHECK(batch.Items().empty());

    // --- CubeBatch: queues items, exposes them, Begin() clears -------------
    Easy3D::CubeBatch cubes;
    CHECK(cubes.Count() == 0);

    cubes.Add(Vector3(1.0f, 2.0f, 3.0f), Vector3(1.0f, 1.0f, 1.0f));
    cubes.Add(Vector3(4.0f, 5.0f, 6.0f), Vector3(2.0f, 2.0f, 2.0f));
    CHECK(cubes.Count() == 2);

    const auto& cubeItems = cubes.Items();
    CHECK(cubeItems.size() == 2);
    CHECK(approx(cubeItems[0].Center.X, 1.0f) && approx(cubeItems[0].Center.Y, 2.0f) && approx(cubeItems[0].Center.Z, 3.0f));
    CHECK(approx(cubeItems[0].Size.X, 1.0f));
    CHECK(approx(cubeItems[1].Center.X, 4.0f));
    CHECK(approx(cubeItems[1].Size.X, 2.0f));

    cubes.Begin();
    CHECK(cubes.Count() == 0);
    CHECK(cubes.Items().empty());

    // --- DebugDraw: queues lines/boxes, exposes them, Clear() clears -------
    Easy3D::DebugDraw debug;
    CHECK(debug.PrimitiveCount() == 0);

    debug.Line(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f));
    debug.Box(Vector3(2.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
    CHECK(debug.PrimitiveCount() == 2);

    const auto& lines = debug.Lines();
    const auto& boxes = debug.Boxes();
    CHECK(lines.size() == 1 && boxes.size() == 1);
    CHECK(approx(lines[0].From.X, 0.0f) && approx(lines[0].To.X, 1.0f));
    CHECK(approx(boxes[0].Center.X, 2.0f) && approx(boxes[0].Size.X, 1.0f));

    debug.Clear();
    CHECK(debug.PrimitiveCount() == 0);
    CHECK(debug.Lines().empty() && debug.Boxes().empty());

    if (g_failures == 0) {
        std::printf("easy3d camera test: OK\n");
    }
    return g_failures == 0 ? 0 : 1;
}
