# ===================================================================================================
#
# CMake file configuring the building process for Outcome
#
# Copyright (c) 2022 Tommy Breslein <https://github.com/tbreslein>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# ===================================================================================================

cmake_minimum_required(VERSION 3.24)

project(outcome
    LANGUAGES CXX
    VERSION 0.1.0)

add_library(outcome STATIC)
set_target_properties(
    outcome
    PROPERTIES
        LINKER_LANGUAGE CXX)

target_compile_features(outcome PUBLIC cxx_std_20)

# ##############################################################################
# CMake config options
# ##############################################################################

# config time options
set(OUTCOME_TEST
    OFF
    CACHE BOOL "Builds the unit tests in ./test. Requires doctest.")

set(OUTCOME_EXAMPLES
    OFF
    CACHE BOOL "Builds the examples in ./examples.")

set(OUTCOME_DOCS
    OFF
    CACHE BOOL "Builds the documentation. Requires doxygen.")

# ##############################################################################
# outcome library & binaries
# ##############################################################################

include(GNUInstallDirs)

set(OUTCOME_SOURCE_DIR "${outcome_SOURCE_DIR}")
set(OUTCOME_BINARY_DIR "${outcome_BINARY_DIR}")
set(OUTCOME_INCLUDE_DIR "${outcome_SOURCE_DIR}")

set(OUTCOME_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(OUTCOME_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
set(OUTCOME_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_DATADIR}/cmake/outcome")
set(OUTCOME_INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_DATADIR}/pkgconfig")

target_sources(${PROJECT_NAME} PRIVATE "src/outcome.hpp")

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC # NEEDS to be public, so the includes can travel into the executable
         ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

target_include_directories(outcome
    PUBLIC # NEEDS to be public, so the includes can travel into the executable
        $<BUILD_INTERFACE:${OUTCOME_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${OUTCOME_INSTALL_INCLUDE_DIR}>)

export(
    TARGETS outcome
    NAMESPACE outcome::
    FILE "${OUTCOME_BINARY_DIR}/outcomeTargets.cmake")
export(PACKAGE outcome)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${OUTCOME_SOURCE_DIR}/data/outcomeConfig.cmake.in"
    "${OUTCOME_BINARY_DIR}/outcomeConfig.cmake"
    INSTALL_DESTINATION "${OUTCOME_INSTALL_CMAKE_DIR}"
    PATH_VARS OUTCOME_INSTALL_INCLUDE_DIR)
write_basic_package_version_file(
    "${OUTCOME_BINARY_DIR}/outcomeConfigVersion.cmake"
    VERSION ${outcome_VERSION}
    COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)
install(FILES "${OUTCOME_BINARY_DIR}/outcomeConfigVersion.cmake"
              "${OUTCOME_BINARY_DIR}/outcomeConfig.cmake"
        DESTINATION "${OUTCOME_INSTALL_CMAKE_DIR}")

configure_file("${OUTCOME_SOURCE_DIR}/data/outcome.pc.in"
               "${OUTCOME_BINARY_DIR}/outcome.pc" @ONLY)
install(FILES "${OUTCOME_BINARY_DIR}/outcome.pc"
        DESTINATION "${OUTCOME_INSTALL_PKGCONFIG_DIR}")

if(${OUTCOME_TEST})
    add_subdirectory(test)
endif()

if(${OUTCOME_EXAMPLES})
    add_subdirectory(examples)
endif()

if(${OUTCOME_DOCS})
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
    add_subdirectory(doxygen)
    file(CREATE_LINK ${CMAKE_BINARY_DIR}/doxygen/sphinx
        ${CMAKE_CURRENT_LIST_DIR}/docs SYMBOLIC)
endif()
