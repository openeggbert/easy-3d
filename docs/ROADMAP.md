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

## Phase 3 — CPU-side vertex builders *(in progress)*

* Turn queued `BillboardItem`/`CubeItem`/`DebugLine`/`DebugBox` data into
  vertex/index buffers (CPU-side geometry only) — still no GPU calls, no
  `GraphicsDevice`, no shaders.
* `Easy3D::CubeMesh` (`AppendCubeMesh`/`BuildCubeMesh`) — **done.** Turns
  `CubeBatch` items into 24 vertices + 36 indices per cube (one UV per face,
  using the item's single `Uv` region on every face). Requested by Galaxy
  Eggbert's `E3D-MIG-051`.
* Billboard quad vertex builder — not started.
* Debug line/box vertex builder — not started.

## Phase 4 — CNA renderer adapters *(in progress)*

* Consume the Phase 3 vertex/index data and actually issue CNA draw calls
  (`GraphicsDevice`, `BasicEffect`/`SpriteBatch`, vertex/index buffers). This
  is the first phase that requires a concrete CNA draw-path decision.
* **Draw-path decision (made, 2026-07-02):** `VertexBuffer` + `IndexBuffer` +
  `BasicEffect` + `GraphicsDevice::DrawIndexedPrimitives` — a real, working,
  tested path already proven by CNA's own `examples/house3d_demo.cpp` (build
  GPU buffers once, `SetVertexBuffer`/`Indices`/`DrawIndexedPrimitives` per
  frame inside a `pass.Apply()` loop). `SpriteBatch` is 2D-only, not usable
  for cube/billboard geometry.
* `Easy3D::CubeMeshRenderer` — **done.** Uploads a `CubeMesh`'s vertex/index
  arrays to GPU buffers once (`CubeVertex{Position,Uv}` → CNA's
  `VertexPositionTexture{Position,TextureCoordinate}`, an exact 1:1 field
  match; 32-bit `IndexBuffer` to match `CubeMesh`'s `uint32_t` indices);
  `Draw(GraphicsDevice&, BasicEffect&)` issues the indexed draw call. The
  caller owns and configures the `BasicEffect` (World/View/Projection/
  Texture) — the adapter knows nothing about tiles, gameplay, or cameras.
  Requested by Galaxy Eggbert's `E3D-MIG-052`. Verified end-to-end from the
  real `GalaxyEggbertCNA` binary: a temporary hardcoded debug cube rendered
  and a center-screen pixel readback confirmed real, non-background pixels.
* Billboard renderer adapter — not started (needed for Blupi, Phase 5/6).
* Debug line/box renderer adapter — not started.

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
