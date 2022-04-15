# Copyright (c) Darrell Wright
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
# Official repository: https://github.com/beached/parallel
#

# This prevents tests running on dependencies
option(DAW_ENABLE_TESTING "Build unit tests" OFF)
if (DAW_ENABLE_TESTING)
    set(DAW_ENABLE_TESTING OFF)
endif ()

include(FetchContent)
if (NOT DAW_TARGET_HEADER_LIBRARIES)
    FetchContent_Declare(
            daw-header-libraries
            GIT_REPOSITORY https://github.com/beached/header_libraries.git
    )
    FetchContent_MakeAvailable(daw-header-libraries)
endif ()

if (NOT DAW_TARGET_SYNCHRO)
    FetchContent_Declare(
            daw-synchro
            GIT_REPOSITORY https://github.com/beached/atomic_wait.git
    )
    FetchContent_MakeAvailable(daw-synchro)
endif ()

