// SPDX-License-Identifier: MIT
//
// Self-contained tests for Easy3D::Version. Does NOT require linking CNA.
// TextureAtlas tests live in test_texture_atlas.cpp. Uses a tiny CHECK macro
// instead of assert so the checks run even in NDEBUG builds.

#include "Easy3D/Version.hpp"

#include <cstdio>
#include <string>

static int g_failures = 0;

#define CHECK(cond)                                                       \
    do {                                                                  \
        if (!(cond)) {                                                    \
            std::printf("FAIL: %s (line %d)\n", #cond, __LINE__);         \
            ++g_failures;                                                 \
        }                                                                 \
    } while (0)

int main()
{
    // --- Version --------------------------------------------------------
    CHECK(Easy3D::VersionMajor == 0);
    CHECK(Easy3D::VersionMinor == 1);
    CHECK(Easy3D::VersionNumber() == 100); // 0.1.0
    CHECK(std::string(Easy3D::VersionString()) == "0.1.0");

    if (g_failures == 0) {
        std::printf("easy3d basics test: OK (version %s)\n", Easy3D::VersionString());
    }
    return g_failures == 0 ? 0 : 1;
}
