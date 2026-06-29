# SPDX-License-Identifier: MIT

# The minimal example constructs an Easy3D::Camera3D and reads CNA Matrix values,
# so it must link CNA. Only build it when CNA is linkable (either the parent
# project provided the CNA target, or -DEASY3D_LINK_CNA=ON built it). main.cpp
# stays in the tree either way as documentation of the intended API shape.
if(EASY3D_CNA_LINKED)
    add_executable(easy3d_minimal main.cpp)
    target_link_libraries(easy3d_minimal PRIVATE easy3d)  # easy3d carries CNA transitively
else()
    message(STATUS
        "Easy3D: skipping 'minimal' example "
        "(needs CNA linked: -DEASY3D_LINK_CNA=ON, or a parent project that defines the CNA target).")
endif()
