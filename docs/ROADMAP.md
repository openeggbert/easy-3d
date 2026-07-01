# Easy3D — Roadmap

This roadmap is intentionally **conservative**. Easy3D grows only as far as
Galaxy Eggbert actually needs it. If a feature is not needed soon, it is not
built. Anything not listed here is out of scope until explicitly approved.

## Phase 0 — Scaffold *(done)*

* Build system (CMake, C++23, `easy3d` static library target).
* Documentation (`README.md`, `docs/ARCHITECTURE.md`, this file,
  `docs/QUESTIONS.md`, `CLAUDE.md`).
* Empty / minimal helper classes (compilable stubs).
* A minimal, self-contained test.

## Phase 1 — Camera helpers *(done)*

* `Easy3D::Camera3D` — position/target/up + FOV/aspect/near/far, producing CNA
  view and projection `Matrix` values.
* `Easy3D::OrbitCamera` — target + yaw/pitch/distance orbit camera.
* `Easy3D::FollowCamera` — smoothed follow camera (target + offset + smoothing).

## Phase 2 — Data-side batching and atlas helpers *(done)*

Non-rendering, CPU-side data storage — no GPU work, no draw calls.

* `Easy3D::TextureAtlas` — named sub-rectangles + UV lookup, plus
  `AddGrid` (spritesheet grid insertion) and `GetUvOrDefault` (non-throwing
  lookup).
* `Easy3D::BillboardBatch` — queues `BillboardItem` (position, size, UV,
  origin, rotation) for camera-facing quads.
* `Easy3D::CubeBatch` — queues `CubeItem` (center, size, UV) for cubes / tiles.
* `Easy3D::DebugDraw` — queues `DebugLine` / `DebugBox` for development
  overlays.

## Phase 3 — CPU-side vertex builders

* Turn queued `BillboardItem`/`CubeItem`/`DebugLine`/`DebugBox` data into
  vertex/index buffers (CPU-side geometry only) — still no GPU calls, no
  `GraphicsDevice`, no shaders.

## Phase 4 — CNA renderer adapters

* Consume the Phase 3 vertex/index data and actually issue CNA draw calls
  (`GraphicsDevice`, `BasicEffect`/`SpriteBatch`, vertex/index buffers). This
  is the first phase that requires a concrete CNA draw-path decision.

## Phase 5 — Galaxy Eggbert support

* Render Blupi as a **billboard** using existing Mobile Eggbert animation
  frames.
* Render simple **cube / tile terrain**.
* Provide **debug drawing** (lines, boxes) for development.
* **Keep all Eggbert-specific logic outside Easy3D** — Easy3D supplies generic
  billboard/cube/atlas/debug helpers only; the meaning of tiles, animation
  tables, and gameplay live in Galaxy Eggbert.

## Phase 6 — Optional future (no implementation yet)

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
