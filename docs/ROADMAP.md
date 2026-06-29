# Easy3D — Roadmap

This roadmap is intentionally **conservative**. Easy3D grows only as far as
Galaxy Eggbert actually needs it. If a feature is not needed soon, it is not
built. Anything not listed here is out of scope until explicitly approved.

## Phase 0 — Scaffold *(current)*

* Build system (CMake, C++23, `easy3d` static library target).
* Documentation (`README.md`, `docs/ARCHITECTURE.md`, this file,
  `docs/QUESTIONS.md`, `CLAUDE.md`).
* Empty / minimal helper classes (compilable stubs).
* A minimal, self-contained test.

## Phase 1 — Camera helpers

* `Easy3D::Camera3D` — position/target/up + FOV/aspect/near/far, producing CNA
  view and projection `Matrix` values.
* `Easy3D::OrbitCamera` — target + yaw/pitch/distance orbit camera.
* `Easy3D::FollowCamera` — smoothed follow camera (target + offset + smoothing).

## Phase 2 — Basic rendering helpers

* `Easy3D::BillboardBatch` — batch of camera-facing quads.
* `Easy3D::CubeBatch` — batch of cubes / tiles.
* `Easy3D::TextureAtlas` — named sub-rectangles + UV lookup.

## Phase 3 — Galaxy Eggbert support

* Render Blupi as a **billboard** using existing Mobile Eggbert animation
  frames.
* Render simple **cube / tile terrain**.
* Provide **debug drawing** (lines, boxes) for development.
* **Keep all Eggbert-specific logic outside Easy3D** — Easy3D supplies generic
  billboard/cube/atlas/debug helpers only; the meaning of tiles, animation
  tables, and gameplay live in Galaxy Eggbert.

## Phase 4 — Optional future (no implementation yet)

* **Lua: discussion only.** Lua is currently *undecided* and intentionally out
  of scope. If it ever happens it would be an optional, separate module
  (working name `easy3d-lua`), and only after the user explicitly approves it.
  No Lua code is to be added before then.
* Possible later: simple HUD/2D helpers (`Easy3D::Hud2D`), and a discussion
  about whether Easy3D should ever load 3D models (vs. staying billboard / cube
  / tile only). See [`QUESTIONS.md`](QUESTIONS.md).

## Hard limits (do not cross without explicit approval)

No ECS, physics, navigation, networking, editor, asset database, resource
cache, model importer, MeshCraft import, plugin system, or PBR renderer.
