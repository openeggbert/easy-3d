# CLAUDE.md — rules for automated contributions to Easy3D

This file is for future Claude Code (or any automated) sessions working in the
**easy-3d** repository. Read it before making changes.

## Read first

1. [`README.md`](README.md) — what Easy3D is and is not.
2. [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) — how Easy3D relates to CNA.
3. [`docs/ROADMAP.md`](docs/ROADMAP.md) — what is in scope, and in what order.
4. [`docs/QUESTIONS.md`](docs/QUESTIONS.md) — open questions still awaiting the
   user's decision. Do not silently decide these; ask.

## Working rules

* **Make one small change at a time.** Easy3D should stay small, boring, and
  testable. Prefer minimal helper classes over frameworks.
* **Keep CNA types visible.** Easy3D APIs may accept and return CNA / XNA-style
  types (`Vector3`, `Matrix`, …). Do **not** wrap or hide them.
* **Do not turn Easy3D into Simple3D.** No entity/component framework, no engine
  object hierarchy, no generic "Game" god-object.
* **Do not refactor CNA.** CNA is the runtime and lives in its own repository.

## Do NOT modify sibling repositories

Do **not** edit, build into, or "fix" these unless the user explicitly asks:

* `../cna`
* `../sharp-runtime`
* `../mobile-eggbert`
* `../galaxy-eggbert`

You may *read* `../cna` to understand include paths, namespaces, CMake target
names, and existing CNA types. Do **not** inspect or copy from `../simple-3d`
(or nova-3d / mesh-craft / mesh-world) unless the user explicitly approves it.
Nez / libGDX / simple-3d are conceptual inspiration only — never copy their code.

## Features that require explicit approval

Do not add any of the following without the user explicitly approving it first:

* **Lua** integration (see roadmap — currently out of scope).
* Physics, navigation, networking.
* A scene editor, asset database, resource cache, or plugin system.
* A model importer / MeshCraft import.
* A large ECS / entity-component framework.
* A PBR renderer or any "full engine" subsystem.

## Where things belong

* **Eggbert-specific logic does not go in Easy3D.** If a helper only makes sense
  for Galaxy Eggbert or Mobile Eggbert, put it in that project later.
* **Mobile Eggbert must not be changed** just because Galaxy Eggbert (or Easy3D)
  would find it convenient. Any future Mobile Eggbert improvement is a separate,
  carefully-scoped decision (e.g. enum classes instead of magic constants).
* **If a feature is not needed by Galaxy Eggbert soon, do not build it now.**

## After meaningful changes

* Update the docs (`README.md`, `docs/ARCHITECTURE.md`, `docs/ROADMAP.md`) when
  the architecture or scope changes.
* Add or update a small test when you add behavior that can be tested without
  linking all of CNA.
* Keep the default build green: `cmake -S . -B build && cmake --build build`.
