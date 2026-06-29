# Easy3D — Open Questions

Questions for the user. Future automated sessions must **not** silently decide
these — ask first. Each item notes what was already learned by inspecting
`../cna`.

## 1. CNA math types directly, or tiny Easy3D aliases?

Should Easy3D use CNA math types directly in its API and storage, or define tiny
aliases / its own small POD types?

> **Findings.** CNA exposes `Microsoft::Xna::Framework::Vector3`, `Vector2`, and
> `Matrix` (with `Matrix::CreateLookAt` and
> `Matrix::CreatePerspectiveFieldOfView`). The current scaffold uses these CNA
> types **directly** in the camera helpers (no hidden wrappers). Note this has a
> linkage consequence — see Q2/Q3: those types are defined in CNA `.cpp` files,
> so using them at runtime requires linking `CNA`.
> `TextureAtlas` deliberately uses small Easy3D-local PODs (`AtlasRect`,
> `UvRect`) because pixel/UV rectangles are not really CNA concepts; this also
> keeps it CNA-free and unit-testable on its own.
>
> **Decision needed:** keep using CNA types directly everywhere (current
> assumption), or introduce `Easy3D::Vector3 = Microsoft::Xna::Framework::Vector3`
> style aliases for brevity?

## 2. Exact CNA CMake target name?

> **Finding (resolved):** CNA's library target is **`CNA`** — a `STATIC` library
> built with C++23, whose `PUBLIC` include directory is `../cna/include`. It
> links `SHARP_RUNTIME`, a graphics backend target, and (privately) SDL3 /
> ffmpeg. Please confirm `CNA` is the intended stable, public target name.

## 3. Should examples link CNA now, or stay documentation-only?

> **Finding / current choice:** CNA math is implemented out-of-line, and linking
> CNA pulls in heavy transitive dependencies (SHARP_RUNTIME, SDL3, ffmpeg, a
> graphics backend). To keep the default build light and self-contained, the
> camera **example** and camera **test** are gated behind the CMake option
> `EASY3D_LINK_CNA` (default **OFF**). The default build produces the `easy3d`
> library plus a CNA-free test.
>
> **Decision needed:** is "opt-in CNA linkage" acceptable, or should examples
> link CNA by default? And *how* should CNA be linked (see Q below)?

### 3b. How should Easy3D consume CNA from CMake?

Three plausible options, not yet chosen:

* `add_subdirectory(../cna ...)` — simplest, but builds all of CNA and its deps.
* An installed CNA + `find_package(CNA)` — cleanest, requires CNA to export a
  package config.
* Link a prebuilt `libCNA.a` via an `IMPORTED` target — lightest, but brittle
  (must match compiler/flags/ABI).

The scaffold currently leaves a clearly-marked TODO in `CMakeLists.txt` rather
than guessing.

## 4. Header-only, or compiled library?

Should the simple helpers be header-only, or a normal compiled library?

> **Current choice:** a normal compiled `STATIC` library (`easy3d`). Open to
> switching simple helpers to header-only if preferred.

## 5. Lua as a separate `easy3d-lua` module, or completely outside this repo?

Lua is **not** part of this version. If it ever happens, should it be an
optional separate module within this repo (e.g. `easy3d-lua`), or live entirely
outside Easy3D? No implementation until explicitly approved.

## 6. 3D models eventually, or billboard/cube/tile only?

For the first Galaxy Eggbert versions, should Easy3D support 3D models at all,
or stay strictly billboard / cube / tile helpers?

## 7. How should Galaxy Eggbert render Blupi initially?

* (a) invisible debug capsule,
* (b) 2D billboard built from existing Mobile Eggbert sprites, or
* (c) a later, optional 3D model?

This affects whether `BillboardBatch` + `TextureAtlas` are enough for the first
playable version (likely option **b**), or whether more is needed sooner.
