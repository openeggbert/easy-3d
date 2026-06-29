# Easy3D

**Easy3D** is a small C++ helper library for [CNA](../cna).
It is a *companion* library that lives **next to** CNA — not on top of it, and
not in front of it.

Easy3D provides a handful of boring, practical helpers (cameras, billboard/cube
batching, a texture atlas, debug drawing) so that small 3D projects can be
written against CNA without re-inventing the same glue code every time.

Its first concrete purpose is to support **Galaxy Eggbert**, a planned 3D remake
of *Mobile Eggbert* / *Speedy Blupi*, without dragging in Urho3D, Nova-3D,
Simple3D, MeshCraft, or Mesh World.

---

## What Easy3D is

* A **small helper library** for CNA.
* A collection of **minimal, testable utility classes**.
* Used **beside** CNA: a game uses CNA directly *and* uses Easy3D where it helps.
* Free to **accept and return CNA / XNA-style types** (`Vector3`, `Matrix`, …)
  wherever that is convenient.

## What Easy3D is **not**

* It is **not** a replacement for CNA. CNA remains the runtime.
* It does **not** hide CNA. CNA types stay fully visible in Easy3D APIs.
* It is **not** Simple3D, and must not grow back into Simple3D.
* It is **not** Nova-3D or a Urho3D-style abstraction.
* It is **not** a general game engine.
* It has **no** entity/component framework, physics, navigation, networking,
  editor, asset database, resource cache, model importer, or plugin system.

If you find yourself building any of the above, it does **not** belong in Easy3D.

---

## Relationship to CNA

```text
Galaxy Eggbert
  -> uses CNA directly
  -> uses Easy3D helpers for convenience
  -> may reuse Mobile Eggbert as an existing reference / data source

Easy3D
  -> depends on CNA
  -> does NOT hide CNA
  -> contains helper classes only

CNA
  -> XNA 4.0-style runtime + optional CNA/NOXNA extensions
```

A bad design (Easy3D hiding everything):

```cpp
Easy3D::Game game;
game.RunEverything();   // NO — this is not what Easy3D is for
```

The intended design (CNA stays in charge, Easy3D just helps):

```cpp
// The game still uses CNA directly for the Game loop, devices, input, etc.
// Easy3D only helps with cameras, billboard/cube drawing, atlases, debug draw.
Easy3D::Camera3D    camera;
Easy3D::OrbitCamera orbit;
Easy3D::TextureAtlas atlas;
```

---

## Layout assumptions

Easy3D expects the sibling repositories to sit next to it:

```text
.../openeggbert/
  ├── cna/            <- required: the CNA runtime (headers under cna/include)
  ├── sharp-runtime/  <- optional: used by CNA; Easy3D does NOT depend on it (yet)
  └── easy-3d/        <- this repository
```

* **CNA is expected at `../cna`.** Its public headers live in `../cna/include`
  (namespace `Microsoft::Xna::Framework`), and its CMake target is named `CNA`.
* **sharp-runtime may exist at `../sharp-runtime`**, but Easy3D does **not**
  depend on it directly in this initial version.

---

## Building

Requirements: a C++23 compiler and CMake ≥ 3.20.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

By default this builds:

* the `easy3d` static library (compiled against CNA's **headers**), and
* a small, self-contained test that exercises the CNA-free helpers.

### CNA linkage (cameras at runtime)

CNA's math types (`Vector3`, `Matrix`, …) are declared in headers but
**implemented in CNA's compiled `.cpp` files**. The `easy3d` library compiles
fine against the headers alone, but anything that actually *runs* camera math
(e.g. `Camera3D::GetViewMatrix()`) must **link** the `CNA` library.

Because building full CNA pulls in heavy dependencies (SHARP_RUNTIME, SDL3,
ffmpeg, a graphics backend), easy3d figures out CNA linkage in one of three
ways:

1. **A parent project already provides the `CNA` target** (a game that does
   `add_subdirectory(../cna)` before `add_subdirectory(../easy-3d)`). easy3d
   detects the existing `CNA` target and links it automatically — no option
   needed. *This is the path games use* (see below).
2. **Standalone, with `-DEASY3D_LINK_CNA=ON`.** easy3d builds CNA itself from
   `EASY3D_CNA_DIR` (default `../cna`), selecting a graphics backend via
   `EASY3D_CNA_BACKEND` (default `EASY_GL`) and disabling CNA's own demos/tests:

   ```sh
   cmake -S . -B build -DEASY3D_LINK_CNA=ON          # backend defaults to EASY_GL
   cmake --build build
   ctest --test-dir build                            # runs basics + camera tests
   ```
3. **Otherwise (default): headers only.** easy3d compiles against CNA's headers
   and leaves linking to the consuming application. The default build stays light
   and self-contained.

When CNA is linked (case 1 or 2), easy3d defines `EASY3D_HAS_CNA_LINK` and the
camera **example** and camera **test** are built.

### Using Easy3D + CNA together in a game

A game (e.g. Galaxy Eggbert) uses CNA directly *and* Easy3D for convenience.
Pull both in as subdirectories — add CNA first, then Easy3D auto-links it:

```cmake
add_subdirectory(../cna       cna)        # defines the CNA target (+ chosen backend)
add_subdirectory(../easy-3d   easy-3d)    # detects CNA, links it automatically

add_executable(galaxy_eggbert src/main.cpp ...)
target_link_libraries(galaxy_eggbert PRIVATE CNA easy3d)
#                                            ^^^         the game still uses CNA directly
#                                                ^^^^^^  and Easy3D helpers beside it
```

Because the game already selects CNA's backend and options, easy3d does not need
`EASY3D_LINK_CNA` here — it simply links the `CNA` target the game created.

### CMake options

| Option                  | Default   | Meaning                                                                       |
| ----------------------- | --------- | ----------------------------------------------------------------------------- |
| `EASY3D_BUILD_EXAMPLES` | `ON`      | Build the `examples/` targets (camera example needs CNA linked).              |
| `EASY3D_BUILD_TESTS`    | `ON`      | Build the `tests/` targets and register them with CTest.                      |
| `EASY3D_LINK_CNA`       | `OFF`     | Standalone only: build CNA from `EASY3D_CNA_DIR` and link it.                  |
| `EASY3D_CNA_DIR`        | `../cna`  | Path to the CNA repository.                                                    |
| `EASY3D_CNA_BACKEND`    | `EASY_GL` | CNA backend to enable when easy3d builds CNA itself (`SDL_RENDERER`/`EASY_GL`/`BGFX`/`VULKAN`). |

---

## Scope notes

* **Lua is intentionally not part of this version.** It may later be *discussed*
  as an optional, separate module (e.g. `easy3d-lua`), but no Lua code will be
  added without explicit approval. See [`docs/ROADMAP.md`](docs/ROADMAP.md).
* **Eggbert-specific logic does not live here.** Anything specific to Galaxy
  Eggbert / Mobile Eggbert belongs in those projects, not in Easy3D.
* **Mobile Eggbert is not refactored** because of Easy3D or Galaxy Eggbert.

See [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) and
[`docs/ROADMAP.md`](docs/ROADMAP.md) for details, and
[`CLAUDE.md`](CLAUDE.md) for rules that future automated contributions must
follow.

## License

Easy3D is licensed under the [MIT License](LICENSE).

> Note: CNA itself is licensed under the Microsoft Public License (Ms-PL).
> Easy3D only *uses* CNA's public headers; it does not copy CNA source.
