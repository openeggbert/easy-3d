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
# ffmpeg, a graphics backend), so it is opt-in and OFF by default.
option(EASY3D_LINK_CNA "Link the compiled CNA library (needed to run camera math / build camera example + test)" OFF)

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

# ---------------------------------------------------------------------------
# easy3d library
# ---------------------------------------------------------------------------
add_library(easy3d STATIC
    src/Version.cpp
    src/Camera3D.cpp
    src/OrbitCamera.cpp
    src/FollowCamera.cpp
    src/BillboardBatch.cpp
    src/CubeBatch.cpp
    src/TextureAtlas.cpp
    src/DebugDraw.cpp
)

target_include_directories(easy3d
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(EASY3D_CNA_INCLUDE_DIR)
    # PUBLIC: Easy3D's public headers include CNA's math headers.
    target_include_directories(easy3d PUBLIC "${EASY3D_CNA_INCLUDE_DIR}")
endif()

# Linking CNA (optional; see EASY3D_LINK_CNA above).
#
# TODO(QUESTIONS.md Q3b): decide the canonical way to consume CNA from CMake.
# CNA's library target is named `CNA` (a STATIC library, C++23). The three
# candidate approaches are:
#   * add_subdirectory("${EASY3D_CNA_DIR}" ...)  -> builds all of CNA + deps
#   * find_package(CNA ...)                      -> needs CNA to export a config
#   * an IMPORTED target pointing at a prebuilt libCNA.a
# For now we use add_subdirectory when EASY3D_LINK_CNA is ON, and otherwise
# leave linkage to the consuming application (e.g. galaxy-eggbert).
if(EASY3D_LINK_CNA)
    if(NOT TARGET CNA)
        add_subdirectory("${EASY3D_CNA_DIR}" "${CMAKE_BINARY_DIR}/_cna")
    endif()
    target_link_libraries(easy3d PUBLIC CNA)
endif()

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
