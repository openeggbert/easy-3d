// SPDX-License-Identifier: MIT
//
// Self-contained tests for Easy3D::TextureAtlas. Does NOT require linking CNA
// (AtlasRect/UvRect are plain Easy3D PODs, not CNA types). Uses a tiny CHECK
// macro instead of assert so the checks run even in NDEBUG builds.

#include "Easy3D/TextureAtlas.hpp"

#include <cmath>
#include <cstdio>
#include <stdexcept>

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
    // --- Defaults --------------------------------------------------------
    Easy3D::TextureAtlas atlas;
    CHECK(atlas.Count() == 0);
    CHECK(atlas.Empty());
    CHECK(!atlas.Contains("nope"));

    // --- Add + strict lookup ---------------------------------------------
    atlas.SetAtlasSize(100, 200);
    atlas.Add("a", Easy3D::AtlasRect{10, 20, 30, 40});
    CHECK(atlas.Count() == 1);
    CHECK(!atlas.Empty());
    CHECK(atlas.Contains("a"));

    const Easy3D::AtlasRect& r = atlas.GetRect("a");
    CHECK(r.X == 10 && r.Y == 20 && r.Width == 30 && r.Height == 40);

    const Easy3D::UvRect uv = atlas.GetUv("a");
    CHECK(approx(uv.U0, 0.10f)); // 10 / 100
    CHECK(approx(uv.V0, 0.10f)); // 20 / 200
    CHECK(approx(uv.U1, 0.40f)); // (10 + 30) / 100
    CHECK(approx(uv.V1, 0.30f)); // (20 + 40) / 200

    // --- GetUv with unset atlas size returns a zero UvRect ----------------
    // Locks in the documented behavior: GetUv returns {0,0,0,0} when the
    // atlas size is <= 0, even for a region with a non-zero origin.
    Easy3D::TextureAtlas unsized;
    unsized.Add("b", Easy3D::AtlasRect{10, 20, 30, 40});
    const Easy3D::UvRect zeroUv = unsized.GetUv("b");
    CHECK(approx(zeroUv.U0, 0.0f));
    CHECK(approx(zeroUv.V0, 0.0f));
    CHECK(approx(zeroUv.U1, 0.0f));
    CHECK(approx(zeroUv.V1, 0.0f));

    // --- Unknown region throws (strict lookup) ----------------------------
    bool threw = false;
    try {
        (void)atlas.GetRect("missing");
    } catch (const std::out_of_range&) {
        threw = true;
    }
    CHECK(threw);

    // --- GetUvOrDefault: existing key returns the real UV -----------------
    const Easy3D::UvRect uvOrDefaultExisting = atlas.GetUvOrDefault("a");
    CHECK(approx(uvOrDefaultExisting.U0, uv.U0));
    CHECK(approx(uvOrDefaultExisting.V0, uv.V0));
    CHECK(approx(uvOrDefaultExisting.U1, uv.U1));
    CHECK(approx(uvOrDefaultExisting.V1, uv.V1));

    // --- GetUvOrDefault: missing key returns the fallback, never throws ---
    const Easy3D::UvRect fallback{0.25f, 0.25f, 0.75f, 0.75f};
    const Easy3D::UvRect uvOrDefaultMissing = atlas.GetUvOrDefault("missing", fallback);
    CHECK(approx(uvOrDefaultMissing.U0, 0.25f));
    CHECK(approx(uvOrDefaultMissing.V0, 0.25f));
    CHECK(approx(uvOrDefaultMissing.U1, 0.75f));
    CHECK(approx(uvOrDefaultMissing.V1, 0.75f));

    // Default fallback (no argument) is a zero UvRect.
    const Easy3D::UvRect uvOrDefaultZero = atlas.GetUvOrDefault("still missing");
    CHECK(approx(uvOrDefaultZero.U0, 0.0f) && approx(uvOrDefaultZero.V0, 0.0f));
    CHECK(approx(uvOrDefaultZero.U1, 0.0f) && approx(uvOrDefaultZero.V1, 0.0f));

    // --- AddGrid: frame_0..frame_5 in row-major order ---------------------
    Easy3D::TextureAtlas sheet(48, 48); // 3 columns * 16px = 48, 2 rows * 24px = 48
    sheet.AddGrid("frame", 16, 24, 3, 2);
    CHECK(sheet.Count() == 6);
    CHECK(sheet.Contains("frame_0"));
    CHECK(sheet.Contains("frame_1"));
    CHECK(sheet.Contains("frame_2"));
    CHECK(sheet.Contains("frame_3"));
    CHECK(sheet.Contains("frame_4"));
    CHECK(sheet.Contains("frame_5"));
    CHECK(!sheet.Contains("frame_6"));

    // Row 0: frame_0, frame_1, frame_2 left to right at y=0.
    {
        const Easy3D::AtlasRect& f0 = sheet.GetRect("frame_0");
        CHECK(f0.X == 0 && f0.Y == 0 && f0.Width == 16 && f0.Height == 24);
        const Easy3D::AtlasRect& f1 = sheet.GetRect("frame_1");
        CHECK(f1.X == 16 && f1.Y == 0 && f1.Width == 16 && f1.Height == 24);
        const Easy3D::AtlasRect& f2 = sheet.GetRect("frame_2");
        CHECK(f2.X == 32 && f2.Y == 0 && f2.Width == 16 && f2.Height == 24);
    }
    // Row 1: frame_3, frame_4, frame_5 left to right at y=24.
    {
        const Easy3D::AtlasRect& f3 = sheet.GetRect("frame_3");
        CHECK(f3.X == 0 && f3.Y == 24 && f3.Width == 16 && f3.Height == 24);
        const Easy3D::AtlasRect& f4 = sheet.GetRect("frame_4");
        CHECK(f4.X == 16 && f4.Y == 24 && f4.Width == 16 && f4.Height == 24);
        const Easy3D::AtlasRect& f5 = sheet.GetRect("frame_5");
        CHECK(f5.X == 32 && f5.Y == 24 && f5.Width == 16 && f5.Height == 24);
    }

    // UVs follow the same pixel-to-normalized conversion as a manual Add().
    const Easy3D::UvRect f0Uv = sheet.GetUv("frame_0");
    CHECK(approx(f0Uv.U0, 0.0f) && approx(f0Uv.V0, 0.0f));
    CHECK(approx(f0Uv.U1, 16.0f / 48.0f) && approx(f0Uv.V1, 24.0f / 48.0f));

    // AddGrid with startX/startY/spacingX/spacingY offsets the whole grid and
    // inserts gaps between frames.
    Easy3D::TextureAtlas spaced(200, 200);
    spaced.AddGrid("s", 10, 10, 2, 1, /*startX=*/5, /*startY=*/7, /*spacingX=*/2, /*spacingY=*/0);
    const Easy3D::AtlasRect& s0 = spaced.GetRect("s_0");
    CHECK(s0.X == 5 && s0.Y == 7);
    const Easy3D::AtlasRect& s1 = spaced.GetRect("s_1");
    CHECK(s1.X == 5 + 10 + 2 && s1.Y == 7); // startX + (frameWidth + spacingX)

    // --- AddGrid: invalid parameters throw std::invalid_argument ---------
    Easy3D::TextureAtlas bad;
    auto expectThrows = [&bad](auto&& fn) {
        bool threwHere = false;
        try {
            fn();
        } catch (const std::invalid_argument&) {
            threwHere = true;
        }
        CHECK(threwHere);
    };
    expectThrows([&] { bad.AddGrid("", 16, 16, 1, 1); });          // empty prefix
    expectThrows([&] { bad.AddGrid("x", 0, 16, 1, 1); });          // frameWidth <= 0
    expectThrows([&] { bad.AddGrid("x", 16, 0, 1, 1); });          // frameHeight <= 0
    expectThrows([&] { bad.AddGrid("x", 16, 16, 0, 1); });         // columns <= 0
    expectThrows([&] { bad.AddGrid("x", 16, 16, 1, 0); });         // rows <= 0
    expectThrows([&] { bad.AddGrid("x", 16, 16, 1, 1, -1); });     // startX < 0
    expectThrows([&] { bad.AddGrid("x", 16, 16, 1, 1, 0, -1); });  // startY < 0
    expectThrows([&] { bad.AddGrid("x", 16, 16, 1, 1, 0, 0, -1); }); // spacingX < 0
    expectThrows([&] { bad.AddGrid("x", 16, 16, 1, 1, 0, 0, 0, -1); }); // spacingY < 0

    if (g_failures == 0) {
        std::printf("easy3d texture_atlas test: OK\n");
    }
    return g_failures == 0 ? 0 : 1;
}
