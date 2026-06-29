# SPDX-License-Identifier: MIT

# Self-contained tests for the CNA-free parts of Easy3D. These always build and
# run without linking CNA (the linker only pulls the CNA-free object files).
add_executable(easy3d_test_basics test_basics.cpp)
target_link_libraries(easy3d_test_basics PRIVATE easy3d)
add_test(NAME basics COMMAND easy3d_test_basics)

# Camera tests exercise CNA math at runtime, so they require linking CNA.
if(EASY3D_LINK_CNA)
    add_executable(easy3d_test_camera test_camera.cpp)
    target_link_libraries(easy3d_test_camera PRIVATE easy3d)  # easy3d carries CNA transitively
    add_test(NAME camera COMMAND easy3d_test_camera)
else()
    message(STATUS
        "Easy3D: skipping camera test "
        "(needs -DEASY3D_LINK_CNA=ON). test_camera.cpp stays in the tree.")
endif()
