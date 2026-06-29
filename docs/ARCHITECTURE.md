# Easy3D — Architecture

## The big picture

```text
Galaxy Eggbert
  -> uses CNA directly
  -> uses Easy3D helpers
  -> may use Mobile Eggbert as existing reference/library/data source

Easy3D
  -> depends on CNA
  -> does not hide CNA
  -> contains helper classes only

CNA
  -> XNA 4.0-style runtime and future CNA/NOXNA extensions
```

## Principles

* **Easy3D is a companion library, not an engine.**
  It sits *beside* CNA. A game still creates and drives its CNA `Game`, devices,
  graphics device, and input directly. Easy3D only removes repetitive glue:
  cameras, billboard/cube batching, a texture atlas, debug drawing.

* **Easy3D APIs may expose CNA types.**
  It is normal and encouraged for Easy3D functions to take or return
  `Microsoft::Xna::Framework::Vector3`, `Matrix`, and friends. Easy3D must not
  invent parallel math types that hide CNA, and must not wrap CNA objects just
  to rename them.

* **Eggbert-specific rules stay out of Easy3D.**
  Anything that only makes sense for Galaxy Eggbert / Mobile Eggbert (tile
  meanings, Blupi animation tables, level formats, gameplay rules) belongs in
  those projects — never in Easy3D.

* **Mobile Eggbert must not be changed just because Galaxy Eggbert needs
  something.** Mobile Eggbert is an existing, working reference/library. Galaxy
  Eggbert may *reuse* its data/assets/animations/sounds, but does not force
  refactors back into it.

## Dependency direction

```text
galaxy-eggbert ──depends on──> easy-3d ──depends on──> cna
        │                                               ▲
        └───────────────── also depends on ─────────────┘
                       (uses CNA directly too)
```

Easy3D depends on CNA. Nothing in CNA depends on Easy3D. Galaxy Eggbert depends
on both, and is free to call CNA directly whenever Easy3D does not help.

## How Easy3D consumes CNA (current state)

* CNA's public headers are under `../cna/include`
  (namespace `Microsoft::Xna::Framework`).
* CNA's CMake library target is named **`CNA`** (a `STATIC` library, C++23).
* CNA math types such as `Vector3` and `Matrix` are *declared* in headers but
  *defined* in CNA's compiled `.cpp` files. Therefore:
  * The `easy3d` static library **compiles** against CNA headers alone.
  * Code that actually executes CNA math (e.g. `Camera3D::GetViewMatrix()`)
    must **link** the `CNA` library at final link time. The consuming
    application (e.g. Galaxy Eggbert) links both `easy3d` and `CNA`.
* Easy3D does **not** depend on `sharp-runtime` directly, even though CNA does.

See [`QUESTIONS.md`](QUESTIONS.md) for the open CMake-integration decisions.

## What Easy3D is deliberately NOT

No ECS, no generic engine object hierarchy, no Lua, no physics, no navigation,
no networking, no editor, no asset database, no resource cache, no model
importer, no plugin system, no PBR renderer. If one of these starts to appear,
it is a sign Easy3D is drifting back toward Simple3D — stop and reconsider.
