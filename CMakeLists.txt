# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)

project(easy3d
    VERSION 0.1.0
    DESCRIPTION "Small helper library for CNA"
    LANGUAGES CXX)

# Easy3D follows CNA and uses C++23. Keep this in sync with CNA.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(EASY3D_BUILD_EXAMPLES "Build Easy3D examples" ON)
option(EASY3D_BUILD_TESTS "Build Easy3D tests" ON)

# CNA's math types (Vector3, Matrix, ...) are declared in headers but DEFINED in
# CNA's compiled .cpp files. The easy3d library compiles against the headers
# alone, but anything that *runs* that math (e.g. Camera3D::GetViewMatrix) must
# LINK the CNA library. Linking CNA pulls in heavy deps (SHARP_RUNTIME, SDL3,
# ffmpeg, a graphics backend), so building CNA ourselves is opt-in and OFF by
# default. (A parent project that already defines the `CNA` target is detected
# and linked automatically regardless of this option — see below.)
option(EASY3D_LINK_CNA "When building standalone, build CNA from EASY3D_CNA_DIR and link it (enables the camera example + test)" OFF)

# CNA requires exactly one graphics backend to be selected. This picks which one
# to enable when easy3d builds CNA itself (EASY3D_LINK_CNA=ON). On Linux EASY_GL
# is the lightest, most-tested choice. Ignored when the parent project provides
# the CNA target (then the parent already chose a backend).
set(EASY3D_CNA_BACKEND "EASY_GL"
    CACHE STRING "CNA backend to enable when easy3d builds CNA itself: SDL_RENDERER | EASY_GL | BGFX | VULKAN")
set_property(CACHE EASY3D_CNA_BACKEND PROPERTY STRINGS SDL_RENDERER EASY_GL BGFX VULKAN)

# ---------------------------------------------------------------------------
# Locate CNA (companion runtime, expected at ../cna)
# ---------------------------------------------------------------------------
set(EASY3D_CNA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../cna"
    CACHE PATH "Path to the CNA repository (provides headers under <dir>/include).")

if(EXISTS "${EASY3D_CNA_DIR}/include")
    set(EASY3D_CNA_INCLUDE_DIR "${EASY3D_CNA_DIR}/include")
    message(STATUS "Easy3D: using CNA headers from ${EASY3D_CNA_INCLUDE_DIR}")
else()
    message(WARNING
        "Easy3D: CNA headers not found at '${EASY3D_CNA_DIR}/include'. "
        "Set -DEASY3D_CNA_DIR=/path/to/cna. The easy3d library will not compile "
        "until CNA's <Microsoft/Xna/Framework/*.hpp> headers are visible.")
endif()

# Some deeper CNA headers (Graphics/GraphicsDevice.hpp, Graphics/BasicEffect.hpp,
# via Color.hpp) include <SharpRuntime/SharpRuntimeHelper.hpp> directly, even
# just to declare types — unlike the shallow Vector2/Vector3/Matrix headers
# used by Camera3D/CubeBatch/BillboardBatch. When CNA is actually built
# (EASY3D_LINK_CNA=ON or a parent-provided CNA target), CNA's own CMakeLists
# already exposes SHARP_RUNTIME's include dir transitively. In the
# headers-only default build, nothing builds sharp-runtime, so its include
# dir needs to be added explicitly, the same way EASY3D_CNA_INCLUDE_DIR is.
set(EASY3D_SHARP_RUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../sharp-runtime"
    CACHE PATH "Path to the sharp-runtime repository (provides headers under <dir>/include).")

if(EXISTS "${EASY3D_SHARP_RUNTIME_DIR}/include")
    set(EASY3D_SHARP_RUNTIME_INCLUDE_DIR "${EASY3D_SHARP_RUNTIME_DIR}/include")
    message(STATUS "Easy3D: using sharp-runtime headers from ${EASY3D_SHARP_RUNTIME_INCLUDE_DIR}")
else()
    message(WARNING
        "Easy3D: sharp-runtime headers not found at '${EASY3D_SHARP_RUNTIME_DIR}/include'. "
        "Set -DEASY3D_SHARP_RUNTIME_DIR=/path/to/sharp-runtime. Headers-only builds of "
        "CubeMeshRenderer.hpp (and anything else touching GraphicsDevice/BasicEffect) will "
        "not compile until <SharpRuntime/*.hpp> headers are visible.")
endif()

# ---------------------------------------------------------------------------
# easy3d library
# ---------------------------------------------------------------------------
add_library(easy3d STATIC
    src/Version.cpp
    src/Camera3D.cpp
    src/OrbitCamera.cpp
    src/FollowCamera.cpp
    src/BillboardBatch.cpp
    src/BillboardMesh.cpp
    src/BillboardMeshRenderer.cpp
    src/CubeBatch.cpp
    src/CubeMesh.cpp
    src/CubeMeshRenderer.cpp
    src/TextureAtlas.cpp
    src/DebugDraw.cpp
)

target_include_directories(easy3d
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# ---------------------------------------------------------------------------
# Resolve CNA linkage
# ---------------------------------------------------------------------------
# easy3d ends up linked against CNA in one of three ways:
#   (1) A parent project (e.g. a game using both CNA and Easy3D) already defined
#       the `CNA` target before add_subdirectory(easy-3d). We detect and link it.
#   (2) Standalone build with -DEASY3D_LINK_CNA=ON: we add_subdirectory(../cna)
#       ourselves (selecting a backend, with CNA's own demos/tests disabled).
#   (3) Otherwise: headers only. easy3d still compiles; the consuming application
#       links CNA itself. This keeps the default build light and self-contained.
set(_easy3d_have_cna_target OFF)

if(TARGET CNA)
    # (1) Provided by the parent project — the integration path for games.
    set(_easy3d_have_cna_target ON)
    message(STATUS "Easy3D: linking the 'CNA' target provided by the parent project.")
elseif(EASY3D_LINK_CNA)
    # (2) Build CNA ourselves from EASY3D_CNA_DIR.
    if(NOT EXISTS "${EASY3D_CNA_DIR}/CMakeLists.txt")
        message(FATAL_ERROR
            "Easy3D: EASY3D_LINK_CNA=ON but no CNA CMake project at '${EASY3D_CNA_DIR}'. "
            "Set -DEASY3D_CNA_DIR=/path/to/cna.")
    endif()
    # Keep CNA's own demos/tests out of our build.
    set(CNA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(CNA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    # Select exactly one CNA graphics backend (CNA fails to configure otherwise).
    if(EASY3D_CNA_BACKEND STREQUAL "SDL_RENDERER")
        set(CNA_BACKEND_SDL_RENDERER ON CACHE BOOL "" FORCE)
    elseif(EASY3D_CNA_BACKEND STREQUAL "EASY_GL")
        set(CNA_BACKEND_EASY_GL ON CACHE BOOL "" FORCE)
    elseif(EASY3D_CNA_BACKEND STREQUAL "BGFX")
        set(CNA_BACKEND_BGFX ON CACHE BOOL "" FORCE)
    elseif(EASY3D_CNA_BACKEND STREQUAL "VULKAN")
        set(CNA_BACKEND_VULKAN ON CACHE BOOL "" FORCE)
    else()
        message(FATAL_ERROR
            "Easy3D: unknown EASY3D_CNA_BACKEND '${EASY3D_CNA_BACKEND}' "
            "(use SDL_RENDERER, EASY_GL, BGFX or VULKAN).")
    endif()
    message(STATUS "Easy3D: building CNA from '${EASY3D_CNA_DIR}' (backend: ${EASY3D_CNA_BACKEND}).")
    add_subdirectory("${EASY3D_CNA_DIR}" "${CMAKE_BINARY_DIR}/_cna")
    set(_easy3d_have_cna_target ON)
endif()

if(_easy3d_have_cna_target)
    # CNA propagates its PUBLIC include dirs (the Microsoft/Xna headers).
    target_link_libraries(easy3d PUBLIC CNA)
    target_compile_definitions(easy3d PUBLIC EASY3D_HAS_CNA_LINK)
elseif(EASY3D_CNA_INCLUDE_DIR)
    # (3) Headers-only: compile against CNA headers; consumer links CNA later.
    target_include_directories(easy3d PUBLIC "${EASY3D_CNA_INCLUDE_DIR}")
    if(EASY3D_SHARP_RUNTIME_INCLUDE_DIR)
        target_include_directories(easy3d PUBLIC "${EASY3D_SHARP_RUNTIME_INCLUDE_DIR}")
    endif()
endif()

# Whether camera-running targets (example/tests that execute CNA math) can link.
# Visible to the examples/ and tests/ subdirectories added below.
set(EASY3D_CNA_LINKED ${_easy3d_have_cna_target})

add_library(easy3d::easy3d ALIAS easy3d)

# ---------------------------------------------------------------------------
# Examples & tests
# ---------------------------------------------------------------------------
if(EASY3D_BUILD_EXAMPLES)
    add_subdirectory(examples/minimal)
endif()

if(EASY3D_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
