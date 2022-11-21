# ===============================================================================
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
# ===============================================================================

cmake_minimum_required(VERSION 3.24)

project(outcome
    LANGUAGES CXX
    VERSION 0.1.0)

set(OUTCOME_TEST
    OFF
    CACHE BOOL "Builds the unit tests in ./test. Fetches catch2 through CPM.")

set(OUTCOME_USE_ERRORREPORTS
    OFF
    CACHE BOOL "Compiles outcome with ErrorReports.")

set(OUTCOME_USE_MACROS
    OFF
    CACHE BOOL "Compiles outcome with Macros. Some Macros need ErrorReports.")

if(${OUTCOME_USE_ERRORREPORTS})
    add_compile_definitions(OUTCOME_USE_ERRORREPORTS)
endif()

if(${OUTCOME_USE_MACROS})
    add_compile_definitions(OUTCOME_USE_MACROS)
endif()

# ==========================
# outcome library & binaries
# ==========================

set(OUTCOME_SOURCE_DIR "${outcome_SOURCE_DIR}")
set(OUTCOME_BINARY_DIR "${outcome_BINARY_DIR}")
set(OUTCOME_INCLUDE_DIR "${outcome_SOURCE_DIR}")

include(GNUInstallDirs)

set(OUTCOME_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(OUTCOME_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
set(OUTCOME_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_DATADIR}/cmake/outcome")
set(OUTCOME_INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_DATADIR}/pkgconfig")

add_library(outcome INTERFACE)

target_include_directories(
    outcome INTERFACE
        $<BUILD_INTERFACE:${OUTCOME_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${OUTCOME_INSTALL_INCLUDE_DIR}>)

if(${OUTCOME_TEST})
    add_subdirectory(test)
endif()

# ============
# installation
# ============

install(
    DIRECTORY "${OUTCOME_INCLUDE_DIR}/outcome"
    DESTINATION "${OUTCOME_INSTALL_INCLUDE_DIR}")
install(
    TARGETS outcome
    EXPORT outcomeTargets)

export(
    TARGETS outcome
    NAMESPACE outcome::
    FILE "${OUTCOME_BINARY_DIR}/outcomeTargets.cmake")
export(PACKAGE outcome)
install(
    EXPORT outcomeTargets
    NAMESPACE outcome::
    DESTINATION "${OUTCOME_INSTALL_CMAKE_DIR}")

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
