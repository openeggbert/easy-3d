// SPDX-License-Identifier: MIT
//
// Compile-check ONLY for Easy3D::BillboardMeshRenderer — always an OBJECT
// library, never linked/run here, regardless of EASY3D_CNA_LINKED. Mirrors
// test_cube_mesh_renderer.cpp exactly: BillboardMeshRenderer's constructor
// needs a live Microsoft::Xna::Framework::Graphics::GraphicsDevice&, which
// only exists once a real CNA Game has created a window/graphics context —
// not something a plain main() can produce. This file only proves the API
// compiles against real CNA headers (catches header/API drift); the real
// runtime verification lives in galaxy-eggbert's GalaxyEggbertCNA target,
// which already has a live window/GraphicsDevice.

#include "Easy3D/BillboardMeshRenderer.hpp"

namespace
{
    // Never called from main() below — exists purely so the compiler
    // type-checks every BillboardMeshRenderer call against the real CNA headers.
    void CompileCheckOnly(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                          const std::vector<Easy3D::BillboardVertex>& vertices,
                          const std::vector<std::uint32_t>& indices)
    {
        Easy3D::BillboardMeshRenderer renderer(device, vertices, indices);
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
