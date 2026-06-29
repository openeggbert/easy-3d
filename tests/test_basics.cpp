// SPDX-License-Identifier: MIT
//
// Self-contained tests for the CNA-free parts of Easy3D (Version, TextureAtlas).
// These do NOT require linking CNA. Uses a tiny CHECK macro instead of assert so
// the checks run even in NDEBUG builds.

#include "Easy3D/Version.hpp"
#include "Easy3D/TextureAtlas.hpp"

#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <string>

static int g_failures = 0;

#define CHECK(cond)                                                       \
    do {                                                                  \
        if (!(cond)) {                                                    \
            std::printf("FAIL: %s (line %d)\n", #cond, __LINE__);         \
            ++g_failures;                                                 \
        }                                                                 \
    } while (0)

static bool approx(float a, float b) { return std::fabs(a - b) < 1e-6f; }

int main()
{
    // --- Version --------------------------------------------------------
    CHECK(Easy3D::VersionMajor == 0);
    CHECK(Easy3D::VersionMinor == 1);
    CHECK(Easy3D::VersionNumber() == 100); // 0.1.0
    CHECK(std::string(Easy3D::VersionString()) == "0.1.0");

    // --- TextureAtlas defaults -----------------------------------------
    Easy3D::TextureAtlas atlas;
    CHECK(atlas.Count() == 0);
    CHECK(!atlas.Contains("nope"));

    // --- TextureAtlas add + lookup -------------------------------------
    atlas.SetAtlasSize(100, 200);
    atlas.Add("a", Easy3D::AtlasRect{10, 20, 30, 40});
    CHECK(atlas.Count() == 1);
    CHECK(atlas.Contains("a"));

    const Easy3D::AtlasRect& r = atlas.GetRect("a");
    CHECK(r.X == 10 && r.Y == 20 && r.Width == 30 && r.Height == 40);

    const Easy3D::UvRect uv = atlas.GetUv("a");
    CHECK(approx(uv.U0, 0.10f)); // 10 / 100
    CHECK(approx(uv.V0, 0.10f)); // 20 / 200
    CHECK(approx(uv.U1, 0.40f)); // (10 + 30) / 100
    CHECK(approx(uv.V1, 0.30f)); // (20 + 40) / 200

    // --- Unknown region throws -----------------------------------------
    bool threw = false;
    try {
        (void)atlas.GetRect("missing");
    } catch (const std::out_of_range&) {
        threw = true;
    }
    CHECK(threw);

    if (g_failures == 0) {
        std::printf("easy3d basics test: OK (version %s)\n", Easy3D::VersionString());
    }
    return g_failures == 0 ? 0 : 1;
}
