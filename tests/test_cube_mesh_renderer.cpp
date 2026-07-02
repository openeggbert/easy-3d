// SPDX-License-Identifier: MIT
//
// Compile-check ONLY for Easy3D::CubeMeshRenderer — always an OBJECT library,
// never linked/run here, regardless of EASY3D_CNA_LINKED. Unlike
// test_camera.cpp/test_batches.cpp/test_cube_mesh.cpp (which only need CNA's
// math types, constructible without a window), CubeMeshRenderer's
// constructor needs a live Microsoft::Xna::Framework::Graphics::GraphicsDevice&,
// which only exists once a real CNA Game has created a window/graphics
// context — not something a plain main() can produce. This file only proves
// the API compiles against real CNA headers (catches header/API drift); the
// real runtime verification for CubeMeshRenderer lives in galaxy-eggbert's
// GalaxyEggbertCNA target, which already has a live window/GraphicsDevice.

#include "Easy3D/CubeMeshRenderer.hpp"

namespace
{
    // Never called from main() below — exists purely so the compiler
    // type-checks every CubeMeshRenderer call against the real CNA headers.
    void CompileCheckOnly(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                          const std::vector<Easy3D::CubeVertex>& vertices,
                          const std::vector<std::uint32_t>& indices)
    {
        Easy3D::CubeMeshRenderer renderer(device, vertices, indices);
        Microsoft::Xna::Framework::Graphics::BasicEffect effect(device);
        effect.World = Microsoft::Xna::Framework::Matrix::getIdentityProperty();
        effect.setTextureEnabledProperty(false);
        renderer.Draw(device, effect);
        (void)renderer.VertexCount();
        (void)renderer.PrimitiveCount();
    }
}

int main()
{
    return 0;
}
