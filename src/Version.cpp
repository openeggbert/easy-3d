// SPDX-License-Identifier: MIT
#include "Easy3D/Version.hpp"

#define EASY3D_STR2(x) #x
#define EASY3D_STR(x) EASY3D_STR2(x)

namespace Easy3D
{
    const char* VersionString() noexcept
    {
        return EASY3D_STR(EASY3D_VERSION_MAJOR) "."
               EASY3D_STR(EASY3D_VERSION_MINOR) "."
               EASY3D_STR(EASY3D_VERSION_PATCH);
    }
}

#undef EASY3D_STR
#undef EASY3D_STR2
