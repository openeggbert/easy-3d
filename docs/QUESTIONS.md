# Easy3D — Open Questions

Questions for the user. Future automated sessions must **not** silently decide
these — ask first. Each item notes what was already learned by inspecting
`../cna`.

## 1. CNA math types directly, or tiny Easy3D aliases? — Current project decision

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
> **Decision:** keep using CNA types directly everywhere (no new aliases). No
> code change required — matches the current camera helpers.

## 2. Exact CNA CMake target name? — Current project decision

> **Finding (resolved):** CNA's library target is **`CNA`** — a `STATIC` library
> built with C++23, whose `PUBLIC` include directory is `../cna/include`. It
> links `SHARP_RUNTIME`, a graphics backend target, and (privately) SDL3 /
> ffmpeg.
>
> **Decision:** confirmed — `CNA` is the stable, public target name Easy3D can
> rely on.

## 3. Should examples link CNA now, or stay documentation-only? — Current project decision

> **Finding / current choice:** CNA math is implemented out-of-line, and linking
> CNA pulls in heavy transitive dependencies (SHARP_RUNTIME, SDL3, ffmpeg, a
> graphics backend). To keep the default build light and self-contained, the
> camera **example** and camera **test** are gated behind the CMake option
> `EASY3D_LINK_CNA` (default **OFF**). The default build produces the `easy3d`
> library plus a CNA-free test.
>
> **Decision:** keep opt-in CNA linkage (`EASY3D_LINK_CNA` default `OFF`). No
> code change required.

### 3b. How should Easy3D consume CNA from CMake? — Current project decision

**Current implementation (works today):**

* If a parent project already defines the `CNA` target (a game that did
  `add_subdirectory(../cna)`), easy3d **detects and links it automatically**.
  This is the integration path for games using both CNA and Easy3D.
* Standalone with `-DEASY3D_LINK_CNA=ON`, easy3d does
  `add_subdirectory(${EASY3D_CNA_DIR})` itself — selecting a backend via
  `EASY3D_CNA_BACKEND` (default `EASY_GL`) and turning off CNA's own
  demos/tests. Verified to build and link the camera example + test.
* Otherwise it stays headers-only.

**Decision:** the current `add_subdirectory` + parent-target-detection approach
is sufficient; no `find_package(CNA)` config or `IMPORTED`/prebuilt-`libCNA.a`
path is needed for now. Revisit only if a concrete consumer needs it.

## 4. Header-only, or compiled library? — Current project decision

Should the simple helpers be header-only, or a normal compiled library?

> **Current choice:** a normal compiled `STATIC` library (`easy3d`).
>
> **Decision:** keep it a compiled `STATIC` library. No code change required.

## 5. Lua as a separate `easy3d-lua` module, or completely outside this repo? — Current project decision (discussion only)

Lua is **not** part of this version. If it ever happens, should it be an
optional separate module within this repo (e.g. `easy3d-lua`), or live entirely
outside Easy3D? No implementation until explicitly approved.

> **Decision (for if/when Lua is ever approved):** it would be an optional,
> separate module within this repo, working name `easy3d-lua`. This is a
> discussion-only answer — Lua itself remains out of scope and unimplemented
> until separately, explicitly approved (see `docs/ROADMAP.md` Phase 6).

## 6. 3D models eventually, or billboard/cube/tile only? — Current project decision

For the first Galaxy Eggbert versions, should Easy3D support 3D models at all,
or stay strictly billboard / cube / tile helpers?

> **Decision:** billboard / cube / tile only for the first Galaxy Eggbert
> versions. No 3D model loading/import — that stays a hard limit
> (`docs/ROADMAP.md`) unless separately, explicitly approved later.

## 7. How should Galaxy Eggbert render Blupi initially? — Current project decision

* (a) invisible debug capsule,
* (b) 2D billboard built from existing Mobile Eggbert sprites, or
* (c) a later, optional 3D model?

This affects whether `BillboardBatch` + `TextureAtlas` are enough for the first
playable version (likely option **b**), or whether more is needed sooner.

> **Decision:** option **(b)** — a 2D billboard built from existing Mobile
> Eggbert sprite animations. `BillboardBatch` + `TextureAtlas` are the two
> Easy3D pieces this needs; per Q6, no 3D model path is needed for this.
