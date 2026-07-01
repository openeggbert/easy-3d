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
* **Current phase:** **Phase 0/1 complete, start of Phase 2 (basic rendering
  helpers).** Camera helpers are implemented and tested. `BillboardBatch` now
  has a real (still non-rendering) item-storage interface; `CubeBatch` and
  `DebugDraw` are still pure item-counting stubs. See `docs/ROADMAP.md`.
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
    `basics` test, plus compile-only checks of `test_camera.cpp`/`main.cpp`
    (`easy3d_test_camera_compilecheck`, `easy3d_minimal_compilecheck` — never
    linked, so they don't need CNA; they exist to catch CNA header/API drift
    even in this build). No CNA build required.
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
  * **Non-rendering item storage:** `Easy3D::BillboardBatch` — `Add(position,
    size, uv)` stores a `BillboardItem{Position, Size, Uv}` per call; `Items()`
    exposes the queued list (`std::vector<BillboardItem>`). `Easy3D::CubeBatch`
    — `Add(center, size)` stores a `CubeItem{Center, Size}` per call, same
    `Items()` pattern. Both still do no GPU work.
  * **Stub only (no item storage yet):** `Easy3D::DebugDraw` — only counts
    queued primitives via `Line`/`Box`.
  * Example: `examples/minimal/main.cpp` (builds/runs only when CNA is linked).
    Observed output: `Easy3D 0.1.0` / `camera eye: (6.47308, 3.54624, 9.46168)` /
    `atlas regions: 1, blupi_idle_0 UV0: (0, 0)`.
* **What does NOT work yet:**
  * No actual GPU rendering anywhere — `BillboardBatch`/`CubeBatch` only store
    items; `DebugDraw` only counts them. Nothing draws yet.
  * `TextureAtlas::GetUv` returns `(0,0,...)` if atlas size is unset/0 (by design),
    which is why the example prints UV0 `(0, 0)` — see Known bugs/limitations.
  * No `find_package(CNA)` / installed-package path; no prebuilt-lib import path
    (decided not needed for now — see `docs/QUESTIONS.md` Q3b).
  * No HUD/2D helpers, no model loading, no Lua (intentionally out of scope —
    see `docs/QUESTIONS.md` Q5/Q6, both decided 2026-07-01).

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
* **TextureAtlas edge case, FollowCamera constant, compile-check targets, Q&A,
  BillboardBatch/CubeBatch design (this session, 2026-07-01):** see §8 items
  1–6 for full detail. Summary: added an unset-atlas-size `GetUv` test; named
  and documented the `FollowCamera` smoothing reference-fps constant + added
  convergence/frame-rate-independence tests; added `OBJECT`-library
  compile-only checks for `test_camera.cpp`/`main.cpp` in the default
  (no-CNA-link) build; recorded decisions for all 7 `docs/QUESTIONS.md` items;
  turned `BillboardBatch` and `CubeBatch` into real (still non-rendering)
  `Add(...)` → `Items()` interfaces.

## 4. Current blocker / main problem

**There is no failing build or test — nothing is currently blocked.**

All `docs/QUESTIONS.md` items are now **DECIDED** (2026-07-01, see §8 item 4):
Blupi will render as a 2D billboard from Mobile Eggbert sprites (Q7), and
Easy3D stays billboard/cube/tile-only, no 3D models (Q6). The scope/decision
gate that used to block rendering work is gone; the rendering helpers
(`BillboardBatch`, `CubeBatch`, `DebugDraw`) are still stubs, but the next step
(§8 item 5) is now a design task, not a "waiting on the user" gate. A CNA
draw-path decision (how to actually issue GPU draw calls) is still needed
before real rendering, but that's implementation detail for later, not an open
question blocking the next small step.

Pick the first unstruck task in §8.

* Exact symptom: n/a (no failure).
* Failing command: n/a.
* Failing test: n/a.
* Affected files for the next step: `src/DebugDraw.cpp` and its header
  (`BillboardBatch`/`CubeBatch` already have real item-storage interfaces as of
  §8 items 5–6).
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

7. **Apply the same item-storage treatment to `DebugDraw` — or decide it's not
   worth it yet.**
   * Goal: either add `LineItem{From, To}` / `BoxItem{Center, Size}` storage
     (mirroring items 5–6), or explicitly decide `DebugDraw` stays a pure
     counter until a concrete debug-overlay consumer exists (it has none yet
     — no Galaxy Eggbert dependency drives this the way Q7's billboard answer
     drove `BillboardBatch`/`CubeBatch`). Worth asking the user which, since
     unlike items 5/6 there's no roadmap/QUESTIONS.md answer already pointing
     at "yes, do this now".
   * Files (if implemented): `include/Easy3D/DebugDraw.hpp`,
     `src/DebugDraw.cpp`, `tests/test_camera.cpp`.
   * Verify: default build + `ctest` (1/1) and CNA-linked build + `ctest` (2/2).

## 9. Do not do yet

* **No GPU rendering implementation** in `BillboardBatch`/`CubeBatch`/`DebugDraw`
  until a CNA draw-path decision is made (Q6/Q7 *scope* is decided — billboard
  from Mobile Eggbert sprites, no 3D models — but *how draw calls are issued*
  is a separate, still-open implementation decision).
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
