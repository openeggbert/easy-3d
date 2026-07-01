# SPDX-License-Identifier: MIT

# The minimal example constructs an Easy3D::Camera3D and reads CNA Matrix values,
# so *running* it must link CNA. Only build the executable when CNA is linkable
# (either the parent project provided the CNA target, or -DEASY3D_LINK_CNA=ON
# built it). Otherwise, compile-check main.cpp as an OBJECT library (CNA headers
# alone are enough to compile; only running it needs CNA's compiled symbols) so
# the default, no-CNA-link build still catches CNA header/API drift.
if(EASY3D_CNA_LINKED)
    add_executable(easy3d_minimal main.cpp)
    target_link_libraries(easy3d_minimal PRIVATE easy3d)  # easy3d carries CNA transitively
else()
    add_library(easy3d_minimal_compilecheck OBJECT main.cpp)
    target_link_libraries(easy3d_minimal_compilecheck PRIVATE easy3d)
    message(STATUS
        "Easy3D: 'minimal' example compile-checked only, not built as an executable "
        "(needs CNA linked to run: -DEASY3D_LINK_CNA=ON, or a parent project that defines the CNA target).")
endif()
