// SPDX-License-Identifier: MIT
//
// Minimal Easy3D example.
//
// The point of this example is the *shape* of the API: a real game still drives
// CNA directly (Game loop, GraphicsDevice, input, ...). Easy3D only helps with
// the camera, batching, atlases, and debug drawing.
//
// NOTE: This target only builds when CNA is linked (-DEASY3D_LINK_CNA=ON),
// because constructing a Camera3D / using CNA math requires CNA's compiled
// code. See README.md and docs/QUESTIONS.md.

#include <Easy3D/Easy3D.hpp>

#include <iostream>

int main()
{
    std::cout << "Easy3D " << Easy3D::VersionString() << '\n';

    using Vector3 = Easy3D::Camera3D::Vector3;

    // Easy3D helps configure a camera; CNA Matrix values come straight back out.
    Easy3D::Camera3D camera;
    camera.SetPosition(Vector3(0.0f, 3.0f, 8.0f));
    camera.SetTarget(Vector3::Zero);

    const auto view = camera.GetViewMatrix();        // Microsoft::Xna::Framework::Matrix
    const auto projection = camera.GetProjectionMatrix();
    (void)view;
    (void)projection;

    // An orbit camera is just a helper that positions the same Camera3D.
    Easy3D::OrbitCamera orbit;
    orbit.SetDistance(12.0f);
    orbit.SetYaw(0.6f);
    orbit.ApplyTo(camera);

    const Vector3& eye = camera.GetPosition();
    std::cout << "camera eye: (" << eye.X << ", " << eye.Y << ", " << eye.Z << ")\n";

    // A small, CNA-free texture atlas (e.g. Blupi sprite frames).
    Easy3D::TextureAtlas atlas(256, 256);
    atlas.Add("blupi_idle_0", Easy3D::AtlasRect{0, 0, 32, 48});
    const Easy3D::UvRect uv = atlas.GetUv("blupi_idle_0");
    std::cout << "atlas regions: " << atlas.Count()
              << ", blupi_idle_0 UV0: (" << uv.U0 << ", " << uv.V0 << ")\n";

    return 0;
}
