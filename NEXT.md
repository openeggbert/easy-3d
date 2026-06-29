# NEXT.md — Easy3D handoff

Concise handoff for resuming work on **easy-3d**, for Claude Code or a human.
Reflects the repository state as of the last commit on branch `develop`
(`a364ad7`, "Wire up CNA build/link integration"). Working tree is clean.

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
* **Current phase:** **Phase 0 (scaffold) complete + start of Phase 1 (camera
  helpers).** Camera helpers are implemented and tested; rendering helpers are
  intentional stubs. See `docs/ROADMAP.md`.
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
  * Default (`cmake -S . -B build`): builds `libeasy3d.a` + the CNA-free
    `basics` test. No CNA build required.
  * CNA-linked (`-DEASY3D_LINK_CNA=ON`, backend `EASY_GL`): verified — builds
    SHARP_RUNTIME + easygl backend + CNA + easy3d + camera example + camera test.
* **Test status:** ✅
  * Default build: `ctest` → 1/1 pass (`basics`).
  * CNA-linked build: `ctest` → 2/2 pass (`basics`, `camera`).
* **Available now:**
  * Library `easy3d` (alias `easy3d::easy3d`).
  * **Implemented:** `Easy3D::Camera3D`, `Easy3D::OrbitCamera`,
    `Easy3D::FollowCamera` (real CNA view/projection math), `Easy3D::TextureAtlas`
    (CNA-free, named pixel rects → normalized UVs), `Easy3D::Version*`.
  * **Stubs only (no rendering):** `Easy3D::BillboardBatch`, `Easy3D::CubeBatch`,
    `Easy3D::DebugDraw` — they only count queued items via `Add`/`Line`/`Box`.
  * Example: `examples/minimal/main.cpp` (builds/runs only when CNA is linked).
    Observed output: `Easy3D 0.1.0` / `camera eye: (6.47308, 3.54624, 9.46168)` /
    `atlas regions: 1, blupi_idle_0 UV0: (0, 0)`.
* **What does NOT work yet:**
  * No actual GPU rendering anywhere — batches and DebugDraw do not draw.
  * `TextureAtlas::GetUv` returns `(0,0,...)` if atlas size is unset/0 (by design),
    which is why the example prints UV0 `(0, 0)` — see Known bugs/limitations.
  * No `find_package(CNA)` / installed-package path; no prebuilt-lib import path.
  * No HUD/2D helpers, no model loading, no Lua (intentionally out of scope).

## 3. Recent changes

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

## 4. Current blocker / main problem

**There is no failing build or test — nothing is currently blocked.**

The honest "main problem" is a **scope/decision gate, not a bug**: the rendering
helpers (`BillboardBatch`, `CubeBatch`, `DebugDraw`) are stubs, and turning them
into real rendering requires a CNA draw-path decision that has **not** been made
yet. The relevant open questions are in `docs/QUESTIONS.md` (esp. Q6/Q7: models
vs billboards/cubes; how Blupi is first rendered) and Q3b (packaging of CNA).

If something must be called the next step rather than a blocker: pick the first
task in §8.

* Exact symptom: n/a (no failure).
* Failing command: n/a.
* Failing test: n/a.
* Affected files for the next step: `src/BillboardBatch.cpp`, `src/CubeBatch.cpp`,
  `src/DebugDraw.cpp`, and their headers.
* Suspected cause: features simply not implemented yet (by design — Phase 2/3).
* Already tried: full default + CNA-linked builds and both test suites — all green.

## 5. Known bugs and limitations

* **incomplete:** `BillboardBatch`, `CubeBatch`, `DebugDraw` do no rendering;
  `Add`/`Line`/`Box` only increment a counter.
* **needs verification:** `TextureAtlas::GetUv` returns a zero `UvRect{}` when
  `atlasWidth/Height <= 0`. The minimal example constructs `TextureAtlas(256,256)`
  yet prints `UV0 (0, 0)` — that is correct here only because the region origin is
  `(0,0)`; confirm UVs are right for a non-zero-origin region (the `basics` test
  already checks `{10,20,30,40}` → `(0.10,0.10,0.40,0.30)` and passes).
* **limitation:** `FollowCamera::Update` smoothing is frame-rate-corrected against
  a hard-coded 60 fps assumption (`deltaSeconds * 60`). Documented, but a magic
  constant.
* **limitation/unknown:** only the `EASY_GL` CNA backend has been built/verified
  from easy-3d; `SDL_RENDERER`, `BGFX`, `VULKAN` are wired in CMake but untested
  here (BGFX uses `FetchContent` → needs network).
* **limitation:** no `find_package(CNA)` / installed-package or prebuilt-lib
  consumption path (Q3b open). Only `add_subdirectory` and parent-target paths
  exist.
* **risky assumption:** `Camera3D::Vector3`/`Matrix` are exposed as `using`
  aliases to the CNA types. This is deliberate (does not hide CNA) but means the
  public API is tied to CNA's exact type names.

## 6. Architecture notes

* **Modules / classes:**
  * Cameras: `Camera3D` (position/target/up + fov/aspect/near/far →
    `Matrix::CreateLookAt` / `CreatePerspectiveFieldOfView`); `OrbitCamera`
    (target + yaw/pitch/distance → `ComputePosition`/`ApplyTo`); `FollowCamera`
    (offset + smoothing → `Update`/`ApplyTo`).
  * Rendering stubs: `BillboardBatch`, `CubeBatch`, `DebugDraw`.
  * Data: `TextureAtlas` + PODs `AtlasRect` (pixels) and `UvRect` (normalized);
    CNA-free.
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
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j

# Test (default build → 1 test: basics)
ctest --test-dir build --output-on-failure

# Configure + build WITH CNA linked (builds CNA via add_subdirectory, EASY_GL)
cmake -S . -B build-cna -DCMAKE_BUILD_TYPE=Debug -DEASY3D_LINK_CNA=ON -DEASY3D_CNA_BACKEND=EASY_GL
cmake --build build-cna -j        # heavy: builds SHARP_RUNTIME + CNA + easy-gl

# Test (CNA-linked build → 2 tests: basics + camera)
ctest --test-dir build-cna --output-on-failure

# Run the most important demo (only exists in the CNA-linked build)
./build-cna/examples/minimal/easy3d_minimal

# Compile-check camera test/example against CNA headers WITHOUT linking CNA
c++ -std=c++23 -Iinclude -I../cna/include -c tests/test_camera.cpp -o /tmp/tc.o
c++ -std=c++23 -Iinclude -I../cna/include -c examples/minimal/main.cpp -o /tmp/mn.o
```

* No linter/formatter is configured in this repo.
* "Reproduce the current bug": n/a — no current failure (see §4).

## 8. Next smallest tasks (ordered)

1. **Add a TextureAtlas edge-case test for unset atlas size.**
   * Goal: lock in the documented behavior that `GetUv` returns `{0,0,0,0}` when
     atlas size ≤ 0, and that a non-zero-origin region gives correct UVs.
   * Files: `tests/test_basics.cpp` (only).
   * Verify: `cmake --build build -j && ctest --test-dir build --output-on-failure`.
2. **Decide & document FollowCamera smoothing semantics (remove the 60-fps magic
   constant or justify it in code).**
   * Goal: make smoothing framerate-independent without an unexplained `*60`.
   * Files: `src/FollowCamera.cpp`, `include/Easy3D/FollowCamera.hpp`,
     `tests/test_camera.cpp` (add a 2-step convergence assertion).
   * Verify: `ctest --test-dir build-cna --output-on-failure` (needs CNA link).
3. **CI-friendly compile-check target for camera code without linking CNA.**
   * Goal: catch CNA-API drift cheaply; add an `OBJECT`-library or compile-only
     custom target so `test_camera.cpp`/`main.cpp` are compiled even in the
     default (no-CNA-link) build.
   * Files: `CMakeLists.txt`, `tests/CMakeLists.txt`, `examples/minimal/CMakeLists.txt`.
   * Verify: `cmake -S . -B build && cmake --build build -j` (no CNA link).
4. **Write `docs/QUESTIONS.md` answers as a short decision once the user replies
   to Q6/Q7**, then start Phase 2 `BillboardBatch` design (interface only, still
   no GPU).
   * Files: `docs/QUESTIONS.md`, `include/Easy3D/BillboardBatch.hpp`.
   * Verify: builds unchanged; no behavior change yet.

## 9. Do not do yet

* **No rendering implementation** in `BillboardBatch`/`CubeBatch`/`DebugDraw`
  until the CNA draw-path / Q6–Q7 decisions are made.
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
