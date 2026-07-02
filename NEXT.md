# NEXT.md — Easy3D handoff

Concise handoff for resuming work on **easy-3d**, for Claude Code or a human.
Reflects the repository state after a small cleanup pass (2026-07-01) on top
of the data-side API expansion committed as `6390684` on branch `develop`.
Working tree has uncommitted changes from this cleanup — see §3.

---

## 1. Project summary

* **What it is:** `easy-3d` (library name `easy3d`, namespace `Easy3D`) — a small
  C++23 *helper* library that lives **beside** [CNA](../cna), a XNA-4.0-style
  runtime. It is **not** an engine, **not** a replacement for CNA, and must not
  hide CNA. Its APIs use CNA / XNA types directly
  (`Microsoft::Xna::Framework::Vector3`, `Matrix`).
* **Main goal:** provide boring, testable utilities (cameras, billboard/cube
  batching, texture atlas, debug draw) so small 3D projects can be built against
  CNA without re-inventing glue. First concrete consumer is **Galaxy Eggbert**
  (a 3D remake of Mobile Eggbert / Speedy Blupi).
* **Current phase:** **Phase 0–2 complete**, **Phase 3/4 in progress** (scaffold,
  camera helpers, data-side batching/atlas helpers all done; CPU-side vertex
  builders and CNA renderer adapters started, for cubes specifically).
  `TextureAtlas`, `BillboardBatch`, `CubeBatch`, and `DebugDraw` all have real
  (still non-rendering) item-storage/lookup interfaces. `Easy3D::CubeMesh`
  (`AppendCubeMesh`/`BuildCubeMesh`, added 2026-07-02 for Galaxy Eggbert's
  `E3D-MIG-051`) turns `CubeBatch` items into CPU-side vertex/index arrays.
  `Easy3D::CubeMeshRenderer` (added 2026-07-02 for `E3D-MIG-052`) actually
  **does** issue CNA draw calls now — `VertexBuffer`/`IndexBuffer`/
  `BasicEffect`/`GraphicsDevice::DrawIndexedPrimitives`, the same pattern
  proven by CNA's own `examples/house3d_demo.cpp`. Billboard/debug-line
  vertex builders and renderer adapters remain unbuilt. See `docs/ROADMAP.md`.
* **Key architectural decisions:**
  * Easy3D depends on CNA; nothing in CNA depends on Easy3D.
  * CNA math types (`Vector3`, `Matrix`) are declared in headers but **defined in
    CNA `.cpp`** → the library compiles header-only, but running camera math
    requires **linking `CNA`**.
  * CNA linkage is resolved 3 ways (parent target / `EASY3D_LINK_CNA` / headers
    only). When linked, `EASY3D_HAS_CNA_LINK` is defined.
  * MIT licensed (CNA itself is Ms-PL; Easy3D only uses CNA headers).

## 2. Current status

* **Build status:** ✅ Working.
  * Default (`cmake -S . -B build -DEASY3D_CNA_DIR=../cna`): builds
    `libeasy3d.a` + the CNA-free `basics`/`texture_atlas` tests, plus
    compile-only checks of `test_camera.cpp`/`test_batches.cpp`/`main.cpp`
    (`easy3d_test_camera_compilecheck`, `easy3d_test_batches_compilecheck`,
    `easy3d_minimal_compilecheck` — never linked, so they don't need CNA; they
    exist to catch CNA header/API drift even in this build). Does not require
    building or linking CNA itself, only its headers.
  * CNA-linked (`-DEASY3D_LINK_CNA=ON -DEASY3D_CNA_BACKEND=EASY_GL
    -DEASY3D_CNA_DIR=../cna`): verified — builds SHARP_RUNTIME + easygl backend
    + CNA + easy3d + camera example + all 4 test executables.
* **Test status:** ✅
  * Default build: `ctest` → 2/2 pass (`basics`, `texture_atlas`).
  * CNA-linked build: `ctest` → 4/4 pass (`basics`, `texture_atlas`, `camera`,
    `batches`).
* **Available now:**
  * Library `easy3d` (alias `easy3d::easy3d`).
  * **Implemented:** `Easy3D::Camera3D`, `Easy3D::OrbitCamera`,
    `Easy3D::FollowCamera` (real CNA view/projection math), `Easy3D::Version*`.
  * **`Easy3D::TextureAtlas`** (CNA-free): named pixel rects → normalized UVs
    (`Add`/`GetRect`/`GetUv`, strict, throws `std::out_of_range` on unknown
    names); `AddGrid(prefix, frameWidth, frameHeight, columns, rows, startX=0,
    startY=0, spacingX=0, spacingY=0)` registers a row-major spritesheet grid
    as `prefix_0`, `prefix_1`, ... (throws `std::invalid_argument` on bad
    parameters; shares `Add()`'s overwrite-on-duplicate-name behavior);
    `GetUvOrDefault(name, fallback={})` — treats an unknown name as normal
    (returns `fallback` instead of throwing `std::out_of_range`); `Count`/
    `Empty` are `noexcept`, `Contains`/`GetUvOrDefault` are not (both build a
    temporary `std::string` key, which can in principle throw
    `std::bad_alloc` — see §3 for the noexcept-removal fix in this session).
  * **Non-rendering item storage:** `Easy3D::BillboardBatch` — `Add(position,
    size)` / `Add(position, size, uv)` / `Add(BillboardItem)` store a
    `BillboardItem{Position, Size, Uv={0,0,1,1}, Origin={0.5,0.5},
    RotationRadians=0}` per call; `Items()` exposes the queue. `Easy3D::CubeBatch`
    — `Add(center, size)` / `Add(center, size, uv)` / `Add(CubeItem)` store a
    `CubeItem{Center, Size, Uv={0,0,1,1}}`, same `Items()` pattern.
    `Easy3D::DebugDraw` — `Line(from, to)`/`Box(center, size)` store
    `DebugLine`/`DebugBox` in separate vectors, exposed via `Lines()`/`Boxes()`;
    `LineCount()`/`BoxCount()`/`PrimitiveCount()` (= line+box count). All three
    also have `Begin()`/`Clear()`/`Empty()`/`Count()`/`End()` (no-op). None do
    GPU work.
  * Example: `examples/minimal/main.cpp` (builds/runs only when CNA is linked).
    Observed output: `Easy3D 0.1.0` / `camera eye: (6.47308, 3.54624, 9.46168)` /
    `atlas regions: 1, blupi_idle_0 UV0: (0, 0)`.
* **What does NOT work yet:**
  * `BillboardBatch`/`DebugDraw` still only store queued items — no vertex
    builder or renderer adapter exists for either yet. `CubeBatch` is the
    exception: `CubeMesh` (vertex builder) + `CubeMeshRenderer` (renderer
    adapter) together can now genuinely draw cubes end-to-end — see §3.
  * `TextureAtlas::GetUv`/`GetUvOrDefault` return `(0,0,...)` if atlas size is
    unset/0 (by design), which is why the example prints UV0 `(0, 0)` — see
    Known bugs/limitations.
  * No `find_package(CNA)` / installed-package path; no prebuilt-lib import path
    (decided not needed for now — see `docs/QUESTIONS.md` Q3b).
  * No HUD/2D helpers, no model loading, no Lua (intentionally out of scope —
    see `docs/QUESTIONS.md` Q5/Q6, both decided 2026-07-01).

## 3. Recent changes

* **`CubeMeshRenderer` CNA renderer adapter (2026-07-02, uncommitted):**
  Roadmap Phase 4, requested by galaxy-eggbert (`E3D-MIG-052`). New
  `include/Easy3D/CubeMeshRenderer.hpp` + `src/CubeMeshRenderer.cpp`:
  constructor uploads a `CubeMesh`'s `CubeVertex`/index arrays to CNA GPU
  buffers once (`CubeVertex{Position,Uv}` → CNA `VertexPositionTexture`, an
  exact field match; 32-bit `IndexBuffer` matching `CubeMesh`'s `uint32_t`
  indices); `Draw(GraphicsDevice&, BasicEffect&)` issues
  `DrawIndexedPrimitives` inside the effect's current-technique pass loop —
  the same pattern CNA's own `examples/house3d_demo.cpp` already proves
  works. The caller owns/configures the `BasicEffect`
  (World/View/Projection/Texture); the adapter has zero Eggbert-specific
  knowledge. Draw-path decision (raw `VertexBuffer`+`IndexBuffer`+
  `BasicEffect`, not `SpriteBatch` which is 2D-only) made after surveying
  CNA's real, tested 3D API surface — recorded in `docs/ROADMAP.md` Phase 4.
  **Bug found and fixed along the way:** the headers-only default build
  (`EASY3D_LINK_CNA=OFF`, no parent CNA target) was missing
  `../sharp-runtime/include` on its path — `GraphicsDevice.hpp`/
  `BasicEffect.hpp` pull in `Color.hpp` → `SharpRuntime/SharpRuntimeHelper.hpp`
  even just to declare types, unlike the shallow `Vector2`/`Vector3`/`Matrix`
  headers `Camera3D`/`CubeBatch`/`CubeMesh` use. Added a new
  `EASY3D_SHARP_RUNTIME_DIR` cache variable (mirrors the existing
  `EASY3D_CNA_DIR` pattern) and wired it into the headers-only
  `target_include_directories` branch. New `tests/test_cube_mesh_renderer.cpp`
  — **compile-check only, unconditionally** (not gated by `EASY3D_CNA_LINKED`
  like the other tests), since the class needs a live `GraphicsDevice&` that
  only exists once a real CNA `Game` has opened a window — not something a
  plain `main()` can produce.
  * Verified: default (no-CNA-link) build — `easy3d` +
    `easy3d_test_cube_mesh_renderer_compilecheck` + `ctest` → 2/2 pass (no
    regression). CNA-linked rebuild — `ctest` → 5/5 pass (`basics`,
    `texture_atlas`, `camera`, `batches`, `cube_mesh`; the renderer test
    isn't linked/run here either way, by design).
  * **Real runtime GPU verification came from galaxy-eggbert, not from a test
    in this repo:** galaxy-eggbert wired one temporary hardcoded debug cube
    into its `GalaxyEggbertCnaGame`, ran the real windowed `GalaxyEggbertCNA`
    binary, and read back the center-screen pixel via
    `GraphicsDevice::GetBackBufferData` (mirroring CNA's own
    `easygl_vertex_formats_test.cpp` readback pattern) — printed `RGBA (255,
    255, 255, 255)`, clearly distinct from the sky-blue clear color,
    confirming a real `DrawIndexedPrimitives` call actually rasterized pixels.
* **`CubeMesh` CPU-side vertex builder (2026-07-02, uncommitted):** implements
  the cube portion of the recommended §8 item 8 task, requested by
  galaxy-eggbert (`E3D-MIG-051`, following its own `E3D-MIG-050` decision that
  this work belongs here rather than as a Galaxy-Eggbert-local adapter). See
  §8 item 8 for full detail. Summary: new `Easy3D::CubeVertex`,
  `AppendCubeMesh`, `BuildCubeMesh` (`include/Easy3D/CubeMesh.hpp` +
  `src/CubeMesh.cpp`); `CMakeLists.txt` and `Easy3D.hpp` updated; new
  CNA-link-gated `tests/test_cube_mesh.cpp`. Verified: default build 2/2,
  CNA-linked build 5/5, plus a clean rebuild of galaxy-eggbert's
  `GalaxyEggbertCNA` consumer and its unrelated 54-test `GalaxyEggbertWorldsTests`
  suite (no regressions). **Not committed yet.**
* **Initial commit (`b2f004a`):** `README.md`, `.gitignore`, `LICENSE` (MIT).
  Present on both `master` and `develop`.
* **Scaffold (`6f9b261`):** added `CMakeLists.txt`; headers `include/Easy3D/*.hpp`
  (umbrella `Easy3D.hpp`, `Version`, `Camera3D`, `OrbitCamera`, `FollowCamera`,
  `BillboardBatch`, `CubeBatch`, `TextureAtlas`, `DebugDraw`); sources `src/*.cpp`;
  `examples/minimal/{CMakeLists.txt,main.cpp}`; `tests/{CMakeLists.txt,
  test_basics.cpp,test_camera.cpp}`; docs (`ARCHITECTURE`, `ROADMAP`, `QUESTIONS`)
  and `CLAUDE.md`.
* **CNA integration (`a364ad7`):** `CMakeLists.txt` now (a) auto-detects a
  parent-provided `CNA` target, (b) builds CNA via `add_subdirectory` on
  `-DEASY3D_LINK_CNA=ON` with backend selected by `EASY3D_CNA_BACKEND` (default
  `EASY_GL`) and CNA demos/tests disabled, (c) else headers-only. Example/test
  gating moved from `EASY3D_LINK_CNA` to internal `EASY3D_CNA_LINKED` so the
  parent-provided path also enables them. Updated `README`, `ARCHITECTURE`,
  `QUESTIONS`.
* **Tests added:** `test_basics` (Version + TextureAtlas, CNA-free) and
  `test_camera` (Camera3D/OrbitCamera/FollowCamera defaults + math, needs CNA).
* **TextureAtlas edge case, FollowCamera constant, compile-check targets, Q&A,
  BillboardBatch/CubeBatch/DebugDraw item storage (2026-07-01):** see §8 items
  1–7 for full detail. Summary: added an unset-atlas-size `GetUv` test; named
  and documented the `FollowCamera` smoothing reference-fps constant + added
  convergence/frame-rate-independence tests; added `OBJECT`-library
  compile-only checks for `test_camera.cpp`/`main.cpp` in the default
  (no-CNA-link) build; recorded decisions for all 7 `docs/QUESTIONS.md` items;
  turned `BillboardBatch`/`CubeBatch`/`DebugDraw` into real (still
  non-rendering) `Add(...)`/`Line(...)`/`Box(...)` → `Items()`/`Lines()`/
  `Boxes()` interfaces. Committed as `0ac99ca` and `1981616`.
* **TextureAtlas grid/fallback API, BillboardItem/CubeItem extensions, test
  reorganization, Phase renumbering (2026-07-01):** a scripted task requested
  a broader data-side API pass across all four helper classes plus test/doc
  updates. Summary:
  * `TextureAtlas`: added `AddGrid(prefix, frameWidth, frameHeight, columns,
    rows, startX=0, startY=0, spacingX=0, spacingY=0)` (row-major spritesheet
    insertion, throws `std::invalid_argument` on bad params, shares `Add()`'s
    overwrite behavior — documented, not changed); added
    `GetUvOrDefault(name, fallback={})` (non-throwing-on-unknown-name lookup);
    added `Empty()`. (This pass also marked `Contains()`/`GetUvOrDefault()`
    `noexcept` — **reverted** in the next entry below; see there for why.)
  * `BillboardBatch`: `BillboardItem` gained `Origin` (CNA `Vector2`, default
    `{0.5,0.5}` = center) and `RotationRadians` (default `0`); `Uv` got a
    default member initializer (full texture `{0,0,1,1}`) so it's optional now.
    Added `Add(position, size)` (2-arg, all-defaults overload) and
    `Add(const BillboardItem&)` alongside the existing 3-arg `Add(position,
    size, uv)`. Added `Clear()` (alias for `Begin()`'s clearing behavior) and
    `Empty()`.
  * `CubeBatch`: `CubeItem` gained a `Uv` member (default `{0,0,1,1}`). Added
    `Add(center, size, uv)` and `Add(const CubeItem&)` alongside the existing
    `Add(center, size)`. Added `Clear()` and `Empty()`.
  * `DebugDraw`: renamed `LineItem`→`DebugLine`, `BoxItem`→`DebugBox` (no
    external consumers existed yet, so this is a clean rename, not a breaking
    change to any caller). Added `LineCount()`/`BoxCount()` alongside the
    existing `PrimitiveCount()`.
  * Tests reorganized: `tests/test_basics.cpp` now covers only `Version` (was
    Version+TextureAtlas). New `tests/test_texture_atlas.cpp` (CNA-free) covers
    `TextureAtlas` including `AddGrid`/`GetUvOrDefault`/invalid-parameter
    cases. New `tests/test_batches.cpp` (needs CNA link, same reasoning as
    `test_camera.cpp`) covers `BillboardBatch`/`CubeBatch`/`DebugDraw`,
    including default-UV/origin/rotation, `Add(Item)` overloads, insertion
    order, `Begin()`/`Clear()`/`End()`. `tests/test_camera.cpp` trimmed back to
    just `Camera3D`/`OrbitCamera`/`FollowCamera`. `tests/CMakeLists.txt`
    updated: new `easy3d_test_texture_atlas` executable (always built/run);
    new `easy3d_test_batches`/`easy3d_test_batches_compilecheck` mirroring the
    existing `easy3d_test_camera`/`_compilecheck` CNA-linked/no-link split.
  * Docs: `README.md` got a "Current status" section. `docs/ROADMAP.md`
    renumbered — old Phase 2 ("basic rendering helpers") is now explicitly
    "data-side batching and atlas helpers" and marked done; inserted new
    Phase 3 ("CPU-side vertex builders") and Phase 4 ("CNA renderer adapters")
    between it and what was Phase 3 ("Galaxy Eggbert support", now Phase 5) and
    Phase 4 ("Optional future", now Phase 6).
  * Committed as `6390684`.
* **Cleanup pass: noexcept fix + docs/QUESTIONS.md wording (2026-07-01,
  uncommitted):** a follow-up scripted task caught two issues from the
  previous entry:
  * **`noexcept` fix (correctness):** `TextureAtlas::Contains()` and
    `TextureAtlas::GetUvOrDefault()` were marked `noexcept` in the previous
    pass, but both call `m_regions.find(std::string(name))` — constructing
    that temporary `std::string` can in principle throw `std::bad_alloc`, so
    `noexcept` was a lie (would `std::terminate()` instead of propagating on
    OOM). Removed `noexcept` from both the declarations
    (`include/Easy3D/TextureAtlas.hpp`) and definitions
    (`src/TextureAtlas.cpp`); added a one-line `@note` on each explaining why.
    `Count()`/`Empty()` stay `noexcept` (no string construction). Behavior
    unchanged; no test changes needed (no test asserted `noexcept`-ness).
    True allocation-free transparent lookup (heterogeneous hashing) was
    explicitly out of scope for this fix.
  * **`docs/QUESTIONS.md` wording:** replaced all 8 `— DECIDED (2026-07-01[,
    ...])` headings with `— Current project decision` (or `— Current project
    decision (discussion only)` for Q5) per the user's request for more
    neutral wording. Also fixed a stale cross-reference: Q5's body said "see
    `docs/ROADMAP.md` Phase 4" for the Lua discussion, but Phase 4 is now "CNA
    renderer adapters" after the Phase renumbering in the previous entry — the
    Lua discussion actually lives in Phase 6 ("Optional future"). Fixed to
    Phase 6. No decisions themselves were changed, only headings/wording and
    that one stale reference.
  * Verified: `cmake -S . -B build -DEASY3D_CNA_DIR=../cna && cmake --build
    build -j && ctest --test-dir build --output-on-failure` → 2/2 pass
    (`basics`, `texture_atlas`). Also re-verified the existing CNA-linked
    build (`build-cna`, already configured with `-DEASY3D_LINK_CNA=ON
    -DEASY3D_CNA_BACKEND=EASY_GL -DEASY3D_CNA_DIR=../cna`): rebuild + `ctest`
    → 4/4 pass (`basics`, `texture_atlas`, `camera`, `batches`) — no
    regression from the `noexcept` removal.
  * **Not committed yet.**

## 4. Current blocker / main problem

**There is no failing build or test — nothing is currently blocked.**

**Update (2026-07-02):** the cube portion of §8 item 8 (CPU-side vertex
builder) is now done — see §3. The remaining pieces (billboard/debug-line
vertex builders, and the still-undecided Phase 4 CNA draw-path) are unchanged
from below.

All `docs/QUESTIONS.md` items are settled as **current project decisions**
(2026-07-01, see §8 item 4; headings reworded from "DECIDED" to "Current
project decision" in the cleanup pass in §3, decisions themselves unchanged):
Blupi will render as a 2D billboard from Mobile Eggbert sprites (Q7), and
Easy3D stays billboard/cube/tile-only, no 3D models (Q6). The scope/decision
gate that used to block rendering work is gone; the rendering helpers
(`BillboardBatch`, `CubeBatch`, `DebugDraw`) are still stubs, but the next step
(§8 item 5) is now a design task, not a "waiting on the user" gate. A CNA
draw-path decision (how to actually issue GPU draw calls) is still needed
before real rendering, but that's implementation detail for later, not an open
question blocking the next small step.

Pick the first unstruck task in §8. §8 item 8 (CPU-side vertex builders,
Roadmap Phase 3) is the explicitly recommended next task from this session.

* Exact symptom: n/a (no failure).
* Failing command: n/a.
* Failing test: n/a.
* Affected files for the next step: none yet — item 8 (vertex builders) would
  likely add new files, e.g. `include/Easy3D/BillboardMesh.hpp` or similar;
  not started.
* Suspected cause: features simply not implemented yet (by design — Phase 3/4).
* Already tried: full default + CNA-linked builds and all 4 tests — all green
  (this session verified with `-DEASY3D_CNA_DIR=../cna` since `../cna` exists
  as a sibling repo in this environment; if it's absent elsewhere, the default
  build still configures headers-only and only the CNA-free tests
  (`basics`, `texture_atlas`) run).

## 5. Known bugs and limitations

* **incomplete (by design, Phase 2 is done, Phase 3/4 aren't started):**
  `BillboardBatch`, `CubeBatch`, `DebugDraw` do no rendering and build no
  vertex/index buffers; `Add`/`Line`/`Box` only append to a `std::vector`.
* **verified (was "needs verification"):** `TextureAtlas::GetUv`/`GetUvOrDefault`
  return a zero `UvRect{}` when `atlasWidth/Height <= 0`. The minimal example
  constructs `TextureAtlas(256,256)` yet prints `UV0 (0, 0)` — that is correct
  here only because the region origin is `(0,0)`; non-zero-origin regions are
  now covered by `tests/test_texture_atlas.cpp` (`{10,20,30,40}` →
  `(0.10,0.10,0.40,0.30)`, plus `AddGrid` frame rects/UVs) and pass.
* ~~**limitation:** `FollowCamera::Update` smoothing magic constant.~~ **Resolved**
  (see §8 item 2): the `60.0f` is now the named, documented constant
  `kSmoothingReferenceFps` in `src/FollowCamera.cpp`; it is intentionally fixed
  (not a tunable) since it defines what a given `SetSmoothing()` value means.
* **limitation/unknown:** only the `EASY_GL` CNA backend has been built/verified
  from easy-3d; `SDL_RENDERER`, `BGFX`, `VULKAN` are wired in CMake but untested
  here (BGFX uses `FetchContent` → needs network).
* **limitation (accepted, not a gap):** no `find_package(CNA)` / installed-package
  or prebuilt-lib consumption path. Only `add_subdirectory` and parent-target
  paths exist — per Q3b (decided 2026-07-01), this is sufficient for now.
* **risky assumption:** `Camera3D::Vector3`/`Matrix` are exposed as `using`
  aliases to the CNA types. This is deliberate (does not hide CNA) but means the
  public API is tied to CNA's exact type names.

## 6. Architecture notes

* **Modules / classes:**
  * Cameras: `Camera3D` (position/target/up + fov/aspect/near/far →
    `Matrix::CreateLookAt` / `CreatePerspectiveFieldOfView`); `OrbitCamera`
    (target + yaw/pitch/distance → `ComputePosition`/`ApplyTo`); `FollowCamera`
    (offset + smoothing → `Update`/`ApplyTo`).
  * Non-rendering item storage: `BillboardBatch` (`BillboardItem`: position,
    size, UV, origin, rotation), `CubeBatch` (`CubeItem`: center, size, UV),
    `DebugDraw` (`DebugLine`, `DebugBox`).
  * Data: `TextureAtlas` (+ `AddGrid`/`GetUvOrDefault`) + PODs `AtlasRect`
    (pixels) and `UvRect` (normalized); CNA-free.
  * `Version` (constexpr + `VersionString()`), umbrella `Easy3D.hpp`.
* **Data flow:** a game owns the CNA `Game`/devices; Easy3D helpers compute
  matrices/positions and (eventually) feed a CNA graphics device. Easy3D never
  owns the game loop.
* **Invariants / boundaries (do not break):**
  * Easy3D must **not** hide or wrap-away CNA; CNA types stay visible in APIs.
  * No engine/ECS/"Game god-object". No Lua, physics, navigation, networking,
    editor, asset DB, resource cache, model importer, plugin system, PBR.
  * Eggbert-specific logic stays out of Easy3D. Do **not** modify `../cna`,
    `../sharp-runtime`, `../mobile-eggbert`, `../galaxy-eggbert`. Do **not** read
    or copy from `../simple-3d`.
* **Compatibility / stability:** namespace `Easy3D`; library target `easy3d` (+
  alias `easy3d::easy3d`); `EASY3D_HAS_CNA_LINK` defined when CNA is linked;
  CMake options `EASY3D_BUILD_EXAMPLES`, `EASY3D_BUILD_TESTS`, `EASY3D_LINK_CNA`,
  `EASY3D_CNA_DIR`, `EASY3D_CNA_BACKEND`. CNA target name is `CNA`, headers under
  `../cna/include`, namespace `Microsoft::Xna::Framework`. C++23 (match CNA).

## 7. Useful commands

Run from the repository root.

```sh
# Configure + build (default: light, headers-only, no CNA link)
# -DEASY3D_CNA_DIR=../cna is only needed if CNA isn't at the default relative
# path; omit it if ../cna already sits next to this repo.
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DEASY3D_CNA_DIR=../cna
cmake --build build -j

# Test (default build → 2 tests: basics, texture_atlas)
ctest --test-dir build --output-on-failure

# Configure + build WITH CNA linked (builds CNA via add_subdirectory, EASY_GL)
cmake -S . -B build-cna -DCMAKE_BUILD_TYPE=Debug -DEASY3D_LINK_CNA=ON -DEASY3D_CNA_BACKEND=EASY_GL -DEASY3D_CNA_DIR=../cna
cmake --build build-cna -j        # heavy: builds SHARP_RUNTIME + CNA + easy-gl

# Test (CNA-linked build → 4 tests: basics, texture_atlas, camera, batches)
ctest --test-dir build-cna --output-on-failure

# Run the most important demo (only exists in the CNA-linked build)
./build-cna/examples/minimal/easy3d_minimal

# Compile-check camera/batch tests + example against CNA headers WITHOUT linking CNA
c++ -std=c++23 -Iinclude -I../cna/include -c tests/test_camera.cpp -o /tmp/tc.o
c++ -std=c++23 -Iinclude -I../cna/include -c tests/test_batches.cpp -o /tmp/tb.o
c++ -std=c++23 -Iinclude -I../cna/include -c examples/minimal/main.cpp -o /tmp/mn.o
```

* No linter/formatter is configured in this repo.
* "Reproduce the current bug": n/a — no current failure (see §4).

## 8. Next smallest tasks (ordered)

1. ~~**Add a TextureAtlas edge-case test for unset atlas size.**~~ **Done.**
   Added a case in `tests/test_basics.cpp`: a fresh `TextureAtlas` with no
   `SetAtlasSize` call, given a non-zero-origin region (`{10,20,30,40}`), now
   asserts `GetUv` returns `{0,0,0,0}`. The existing non-zero-origin-with-set-size
   case (`{10,20,30,40}` → `(0.10,0.10,0.40,0.30)`) already covered the "correct
   UVs" half. Verified: default build + `ctest` → 1/1 (`basics`) pass.
2. ~~**Decide & document FollowCamera smoothing semantics.**~~ **Done.** Kept the
   `*60` (justify-in-code branch, not remove — the math was already
   frame-rate-independent; only the constant was unexplained). Changes:
   * `src/FollowCamera.cpp`: named the constant `kSmoothingReferenceFps` in an
     anonymous namespace, with a comment explaining it is an arbitrary but
     fixed reference (60 fps, kept for continuity with existing `SetSmoothing()`
     call sites) and the exponential-decay derivation.
   * `include/Easy3D/FollowCamera.hpp`: expanded the `GetSmoothing`/`SetSmoothing`
     doc comment to state the "fraction of gap closed per 1/60s" semantics
     explicitly.
   * `tests/test_camera.cpp`: added a 2-step monotonic convergence check, plus a
     frame-rate-independence check (`Update(dt=2/60)` once == `Update(dt=1/60)`
     twice), which is the property the reference-fps design actually buys.
   * Verified: `ctest --test-dir build-cna --output-on-failure` → 2/2 pass;
     default (no-CNA) build also rebuilt clean → 1/1 pass (no regression).
3. ~~**CI-friendly compile-check target for camera code without linking CNA.**~~
   **Done.** `test_camera.cpp`/`main.cpp` only need CNA's *headers* to compile
   (the math types are declared in headers, defined in CNA's `.cpp`s); only
   *running* them needs CNA linked. Added `OBJECT` library targets
   (`easy3d_test_camera_compilecheck`, `easy3d_minimal_compilecheck`) in the
   `else()` branches of `tests/CMakeLists.txt` /
   `examples/minimal/CMakeLists.txt` — built by default (part of `all`, not
   `EXCLUDE_FROM_ALL`), never linked into an executable, so no undefined-symbol
   errors. `CMakeLists.txt` itself needed no changes (`EASY3D_CNA_LINKED` /
   `EASY3D_CNA_INCLUDE_DIR` already existed and are sufficient).
   * Verified: fresh `cmake -S . -B build && cmake --build build -j` (no CNA
     link) now compiles `easy3d_test_camera_compilecheck` and
     `easy3d_minimal_compilecheck` alongside `easy3d_test_basics`; `ctest` → 1/1
     pass. Fresh CNA-linked reconfigure+rebuild (`-DEASY3D_LINK_CNA=ON`) still
     takes the executable/test path unchanged; `ctest` → 2/2 pass (no
     regression).
4. ~~**Write `docs/QUESTIONS.md` answers**~~ **Done (2026-07-01).** All 7
   open questions answered by the user and recorded as short "DECIDED" notes in
   `docs/QUESTIONS.md`:
   * Q1: keep using CNA types directly (no Easy3D aliases).
   * Q2: `CNA` confirmed as the stable CMake target name.
   * Q3 / Q3b: keep opt-in CNA linkage (`EASY3D_LINK_CNA` default OFF); current
     `add_subdirectory` + parent-target-detection consumption is sufficient,
     no `find_package(CNA)`/prebuilt-import path needed now.
   * Q4: keep `easy3d` a compiled `STATIC` library (not header-only).
   * Q5 (discussion only): if Lua is ever approved, it'd be a separate
     `easy3d-lua` module in this repo — still unimplemented/out of scope.
   * Q6: billboard/cube/tile only for first Galaxy Eggbert versions, no 3D
     model support.
   * Q7: Blupi renders initially as **(b) a 2D billboard** from existing Mobile
     Eggbert sprite animations.
   * None of these required code changes — all confirm current scaffold
     choices, except Q6/Q7 which now unblock Phase 2 `BillboardBatch` design.
   * Verify: n/a (docs-only change).

5. ~~**Start Phase 2 `BillboardBatch` design (interface only, still no GPU).**~~
   **Done (2026-07-01).** `BillboardBatch` is now a real (still non-rendering)
   item-storage interface:
   * Added `Easy3D::BillboardItem` (POD: CNA `Vector3 Position`, CNA `Vector2
     Size`, Easy3D `UvRect Uv`) in `include/Easy3D/BillboardBatch.hpp`
     (now includes `TextureAtlas.hpp` for `UvRect`).
   * `BillboardBatch::Add(worldPosition, size, uv)` — signature grew a
     required `const UvRect&` third parameter (was position+size only); stores
     one `BillboardItem` per call in a `std::vector`.
   * Added `BillboardBatch::Items()` (`const std::vector<BillboardItem>&`)
     exposing the queue in `Add()` order — the intended consumption point for
     a future CNA draw path.
   * `Begin()` now clears the vector (was resetting an int counter); `Count()`
     is now `m_items.size()` (no separate counter member).
   * `src/BillboardBatch.cpp` shrank to one line: `m_items.push_back(...)` (the
     `TODO` comment is gone — the thing it deferred is done).
   * Tests: added a `BillboardBatch` section to `tests/test_camera.cpp` (needs
     CNA link — constructing `Vector3(x,y,z)`/`Vector2(x,y)` values calls CNA's
     compiled constructors) covering `Add`×2 → `Count()`/`Items()` contents →
     `Begin()` clears both.
   * `examples/minimal/main.cpp` intentionally untouched (doesn't call
     `BillboardBatch`; no update needed).
   * Verified: default (no-CNA-link) build — `easy3d` + `easy3d_test_camera_compilecheck`
     (now also compile-checking the `BillboardBatch` calls) + `ctest` → 1/1
     pass. CNA-linked rebuild — `ctest` → 2/2 pass (no regression).

6. ~~**Apply the same item-storage treatment to `CubeBatch`.**~~ **Done
   (2026-07-01).** Mirrors item 5 exactly:
   * `include/Easy3D/CubeBatch.hpp`: added `CubeItem{Center, Size}` (both CNA
     `Vector3`); `Add(center, size)` now stores one `CubeItem` per call in a
     `std::vector`; added `Items()`; `Begin()` clears the vector, `Count()`
     reads its size (same shape as `BillboardBatch`, no `UvRect` needed here).
   * `src/CubeBatch.cpp`: shrank to one line, `m_items.push_back(...)`.
   * Tests: added a `CubeBatch` section to `tests/test_camera.cpp` (same
     CNA-link reasoning as item 5) — `Add`×2 → `Count()`/`Items()` contents →
     `Begin()` clears both.
   * `DebugDraw` intentionally left as a pure counter (no immediate Galaxy
     Eggbert consumer; lower priority — see the next task).
   * Verified: default (no-CNA-link) build — `easy3d` +
     `easy3d_test_camera_compilecheck` (now also compile-checking the
     `CubeBatch` calls) + `ctest` → 1/1 pass. CNA-linked rebuild — `ctest` →
     2/2 pass (no regression).

7. ~~**Apply the same item-storage treatment to `DebugDraw`.**~~ **Done
   (2026-07-01).** User explicitly chose "implement now" (asked first, since
   unlike items 5/6 there was no roadmap/QUESTIONS.md answer already pointing
   at "yes, do this now" — `DebugDraw` has no concrete consumer yet). Mirrors
   items 5/6, but with *two* item vectors instead of one (lines and boxes are
   different shapes):
   * `include/Easy3D/DebugDraw.hpp`: added `LineItem{From, To}` and
     `BoxItem{Center, Size}` (both all-CNA-`Vector3` PODs); `Line(from, to)`/
     `Box(center, size)` now store into separate `std::vector<LineItem>` /
     `std::vector<BoxItem>` members; added `Lines()`/`Boxes()` accessors;
     `Clear()` clears both vectors; `PrimitiveCount()` is now
     `m_lines.size() + m_boxes.size()` (no separate counter member).
   * `src/DebugDraw.cpp`: `Line`/`Box` shrank to one `push_back` line each.
   * Tests: added a `DebugDraw` section to `tests/test_camera.cpp` (same
     CNA-link reasoning as items 5–6) — one `Line` + one `Box` →
     `PrimitiveCount()`/`Lines()`/`Boxes()` contents → `Clear()` empties both.
   * Verified: default (no-CNA-link) build — `easy3d` +
     `easy3d_test_camera_compilecheck` (now also compile-checking the
     `DebugDraw` calls) + `ctest` → 1/1 pass. CNA-linked rebuild — `ctest` →
     2/2 pass (no regression).

All three rendering-helper stubs (`BillboardBatch`, `CubeBatch`, `DebugDraw`)
now have real (still non-rendering) item storage. The next real step for any
of them is a CNA draw-path decision (how to actually issue GPU draw calls from
`Items()`/`Lines()`/`Boxes()`) — not yet asked, not yet decided; raise it with
the user before starting GPU work.

8. **Add vertex-builder helpers for billboard quads, cube meshes, and debug
   line geometry, still without GPU rendering (Roadmap Phase 3).**
   * This is the explicitly recommended next task (per this session's scripted
     task brief), following up on item 7's note above. It sits *between* the
     current item storage (`Items()`/`Lines()`/`Boxes()`, done — Phase 2) and
     an eventual CNA renderer adapter (not started — Phase 4, needs a CNA
     draw-path decision first).
   * Goal: convert queued `BillboardItem`/`CubeItem`/`DebugLine`/`DebugBox`
     data into plain CPU-side vertex/index arrays (e.g. positions + UVs for a
     billboard quad, 8 corners + 12/36 indices for a cube, 2 points per debug
     line) — no `GraphicsDevice`, no vertex/index *buffers* (GPU resources), no
     shaders, no `BasicEffect`/`SpriteBatch`. Plain `std::vector<...>` output
     the caller could later upload to CNA.
   * ~~Cube part~~ **Done (2026-07-02).** `include/Easy3D/CubeMesh.hpp` +
     `src/CubeMesh.cpp`: `Easy3D::CubeVertex{Position, Uv}`,
     `AppendCubeMesh(const CubeItem&, vertices, indices)` (24 vertices, 36
     indices — 4 vertices per face so each face carries its own UV corners,
     rather than 8 shared corners, since a shared-corner vertex can't hold 3
     different per-face UVs) and `BuildCubeMesh(const CubeBatch&, vertices,
     indices)` (concatenates every item, offsetting indices correctly). Faces
     wound CCW as seen from outside (outward normal = `(v1-v0) x (v2-v0)`),
     matching CNA/XNA's right-handed convention. Requested by Galaxy Eggbert's
     `E3D-MIG-051` — that repo's own `E3D-MIG-050` decided this vertex-builder
     work belongs here, in Easy3D, not as a Galaxy-Eggbert-local adapter.
     `Easy3D.hpp` umbrella header updated; `CMakeLists.txt`
     (`src/CubeMesh.cpp` added to the library); new
     `tests/test_cube_mesh.cpp` (CNA-link-gated, same pattern as
     `test_batches.cpp`) covering single-cube geometry/UV correctness,
     custom-UV/off-center cubes, multi-item `BuildCubeMesh` index offsetting,
     and the empty-batch case.
     * Verified: default (no-CNA-link) build — `easy3d` +
       `easy3d_test_cube_mesh_compilecheck` + `ctest` → 2/2 pass (no
       regression). CNA-linked rebuild (`-DEASY3D_LINK_CNA=ON
       -DEASY3D_CNA_BACKEND=EASY_GL`) — `ctest` → 5/5 pass (`basics`,
       `texture_atlas`, `camera`, `batches`, `cube_mesh`). Also re-verified
       galaxy-eggbert's `GalaxyEggbertCNA` target still builds clean against
       this updated `easy3d`, and its unrelated `GalaxyEggbertWorldsTests`
       (54 tests) still pass — confirming this change didn't regress the
       consumer.
   * **Remaining:** billboard quad vertex builder and debug line/box vertex
     builder are not started — `CubeMesh` was prioritized because it's what
     Galaxy Eggbert's terrain phase (Phase 5) actually needs first; billboard
     geometry is needed later for Blupi (Phase 6).
   * ~~**Do not continue past this into Phase 4 without checking in first.**~~
     Checked in — see item 9 below.

9. ~~**Phase 4: CNA renderer adapter for `CubeBatch`.**~~ **Done (2026-07-02),
   user approved.** See §3 for full detail. Summary: `Easy3D::CubeMeshRenderer`
   (`include/Easy3D/CubeMeshRenderer.hpp` + `src/CubeMeshRenderer.cpp`)
   uploads `CubeMesh` vertex/index data to CNA `VertexBuffer`/`IndexBuffer`
   once, then `Draw(GraphicsDevice&, BasicEffect&)` issues
   `DrawIndexedPrimitives` — the exact pattern CNA's own
   `examples/house3d_demo.cpp` proves works. Draw-path decision: raw
   `VertexBuffer`+`IndexBuffer`+`BasicEffect`, not `SpriteBatch` (2D-only).
   Fixed a real headers-only-build bug along the way (missing
   `../sharp-runtime/include`, new `EASY3D_SHARP_RUNTIME_DIR` cache var).
   Verified end-to-end via galaxy-eggbert's real windowed `GalaxyEggbertCNA`
   binary (pixel readback confirmed real draw output), since
   `CubeMeshRenderer` needs a live `GraphicsDevice` a plain test `main()`
   can't produce — see `tests/test_cube_mesh_renderer.cpp` (compile-check
   only, by design).
   * **Remaining for full Phase 4:** billboard renderer adapter (needed once
     the billboard vertex builder from item 8 exists) and debug line/box
     renderer adapter. Neither is started.

## 9. Do not do yet

* **No GPU rendering implementation** in `BillboardBatch`/`DebugDraw` until
  their own CNA draw-path check-in happens (mirroring `CubeBatch`/`CubeMesh`/
  `CubeMeshRenderer`, which now has one — see §8 items 8/9). Don't assume the
  cube draw-path decision automatically extends to billboards/debug lines
  without asking first.
* **No refactor of CNA** and **no edits** to `../cna`, `../sharp-runtime`,
  `../mobile-eggbert`, `../galaxy-eggbert`. **No reading/copying** `../simple-3d`.
* **No new subsystems**: no ECS, physics, navigation, networking, editor, asset
  DB, resource cache, model importer, plugin system, PBR. **No Lua.**
* **No API/type renames** (`Easy3D::Camera3D::Vector3` alias, target name `easy3d`,
  `EASY3D_HAS_CNA_LINK`, CMake option names) without checking consumers.
* **No mass cleanup / speculative abstraction.** Keep changes one-small-step.
* **Do not turn on / chase the untested CNA backends** (BGFX needs network) as
  part of unrelated work.

## 10. Resume prompt (copy/paste for a future Claude Code session)

```text
Read NEXT.md in the repo root first. Work on easy-3d only; do not modify ../cna,
../sharp-runtime, ../mobile-eggbert, ../galaxy-eggbert, or read ../simple-3d.
Pick the first task in NEXT.md §8. Inspect only the files that task lists — do not
refactor unrelated code and do not add new subsystems or Lua. Make one small,
verified improvement, then run the task's verification command:
  cmake -S . -B build && cmake --build build -j && ctest --test-dir build --output-on-failure
(or the build-cna / -DEASY3D_LINK_CNA=ON variant if the task needs CNA linked).
Keep CNA types visible; do not hide CNA. When done, update NEXT.md (status, recent
changes, and tick off / re-order §8) and stop.
```
