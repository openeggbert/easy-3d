# SPDX-License-Identifier: MIT

# Self-contained tests for the CNA-free parts of Easy3D. These always build and
# run without linking CNA (the linker only pulls the CNA-free object files).
add_executable(easy3d_test_basics test_basics.cpp)
target_link_libraries(easy3d_test_basics PRIVATE easy3d)
add_test(NAME basics COMMAND easy3d_test_basics)

# Camera tests exercise CNA math at runtime, so *running* them requires linking
# CNA. Compiling them does not: CNA's math types are declared in headers, only
# defined in CNA's .cpp files, so translating test_camera.cpp to an object file
# only needs the headers to be visible. Build an OBJECT library (never linked
# into an executable, so no undefined-symbol errors) as a cheap compile-only
# check that catches CNA header/API drift even in the default, no-CNA-link
# build.
if(EASY3D_CNA_LINKED)
    add_executable(easy3d_test_camera test_camera.cpp)
    target_link_libraries(easy3d_test_camera PRIVATE easy3d)  # easy3d carries CNA transitively
    add_test(NAME camera COMMAND easy3d_test_camera)
else()
    add_library(easy3d_test_camera_compilecheck OBJECT test_camera.cpp)
    target_link_libraries(easy3d_test_camera_compilecheck PRIVATE easy3d)
    message(STATUS
        "Easy3D: camera test compile-checked only, not run "
        "(needs CNA linked to run: -DEASY3D_LINK_CNA=ON, or a parent-provided CNA target).")
endif()
