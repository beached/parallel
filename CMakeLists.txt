# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/parallel
#

cmake_minimum_required(VERSION 3.13)

project("parallel"
        VERSION "1.0.0"
        DESCRIPTION "Various parallel headers"
        HOMEPAGE_URL "https://github.com/beached/parallel"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested.")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

option(DAW_ENABLE_TESTING "Build unit tests" OFF)

include(GNUInstallDirs)
set(parallel_INSTALL_CMAKEDIR
        "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake"
        CACHE STRING
        "The install directory to use for CMake package config files")

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_subdirectory( extern )

add_library(${PROJECT_NAME} INTERFACE)
add_library(daw::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PUBLIC daw::header_libraries atomic_wait ${CMAKE_THREAD_LIBS_INIT})

# Protect dependents from this project's warnings if the guard isn't disabled
set(DAW_Parallel_warning_guard SYSTEM)
if (DAW_Parallel_INCLUDE_WITHOUT_SYSTEM)
    set(DAW_Parallel_warning_guard "")
endif ()
if (DAW_ENABLE_TESTING)
    set(DAW_Parallel_warning_guard "")
endif ()

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
target_include_directories(${PROJECT_NAME} ${DAW_Parallel_warning_guard}
        INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        )

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


include(CMakePackageConfigHelpers)

configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/daw_${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/daw_${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION
        ${parallel_INSTALL_CMAKEDIR})

write_basic_package_version_file("daw_${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE daw::
        DESTINATION ${parallel_INSTALL_CMAKEDIR})

install(FILES "${PROJECT_BINARY_DIR}/daw_${PROJECT_NAME}Config.cmake"
        "${PROJECT_BINARY_DIR}/daw_${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${parallel_INSTALL_CMAKEDIR}$)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/daw DESTINATION include)

if (DAW_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()

