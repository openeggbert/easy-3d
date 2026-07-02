// SPDX-License-Identifier: MIT
#pragma once

/// @file Easy3D.hpp
/// @brief Umbrella header — includes all public Easy3D helpers.
///
/// Easy3D is a small helper library that lives *beside* CNA. It does not hide
/// CNA: its APIs use CNA / XNA-style types (Vector3, Matrix, ...) directly.
/// See README.md and docs/ARCHITECTURE.md.

#include "Easy3D/Version.hpp"
#include "Easy3D/Camera3D.hpp"
#include "Easy3D/OrbitCamera.hpp"
#include "Easy3D/FollowCamera.hpp"
#include "Easy3D/BillboardBatch.hpp"
#include "Easy3D/CubeBatch.hpp"
#include "Easy3D/CubeMesh.hpp"
#include "Easy3D/CubeMeshRenderer.hpp"
#include "Easy3D/TextureAtlas.hpp"
#include "Easy3D/DebugDraw.hpp"
