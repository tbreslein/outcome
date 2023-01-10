# ===============================================================================
#
# CMake file configuring the building process for Outcome
#
# Copyright (c) 2022-2023 Tommy Breslein <https://github.com/tbreslein>
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

# ==========================
# Setup CPM and fetch Catch2
# ==========================

set(CPM_DOWNLOAD_VERSION 0.34.0)

if(CPM_SOURCE_CACHE)
    set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else()
    set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif()

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD
        https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
        ${CPM_DOWNLOAD_LOCATION}
    )
endif()

include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage(
    NAME catch2
    GITHUB_REPOSITORY catchorg/Catch2
    VERSION 3.2.0
)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

# ===============
# Setup the tests
# ===============

# create compile_commands.json in the build folder, which is needed for LSPs
# like ccls and clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE INTERNAL "Write compile_commands.json; needed for LSPs")
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
        ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_executable(test tests.cpp)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    target_compile_options(test PRIVATE
        ${DEFAULT_COMPILE_OPTIONS}
        $<$<CONFIG:Release>:
        /Gw # whole program global optimization
        /GS- # buffer security check: no
        /GL # whole program optimization: enable link-time code generation
        # (disables Zi)
        /GF # enable string pooling
        >
        )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(test PRIVATE
        ${DEFAULT_COMPILE_OPTIONS}
        -fdiagnostics-color=always
        -Wall
        -Wextra
        -Wextra-semi
        -Wcast-align
        -Wduplicated-branches
        -Wdeprecated-copy
        -Wdeprecated-declarations
        -Wduplicated-cond
        -Wformat=2
        -Wformat-truncation
        -Wignored-qualifiers
        -Wlogical-op
        -Wmissing-braces
        -Wmisleading-indentation
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wpedantic
        -Wreorder
        -Wreturn-type
        -Wsign-compare
        -Wsign-conversion
        -Wshadow
        -Wsuggest-final-methods
        -Wsuggest-final-types
        -Wsuggest-override
        -Wswitch-enum
        -Wuninitialized
        -Wunused
        -Wvolatile
        )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(test PRIVATE
        ${DEFAULT_COMPILE_OPTIONS}
        -Wall
        -Wextra
        -Wformat=2
        -Wcast-align
        -Wcomma
        -Wdeprecated-declarations
        -Wignored-qualifiers
        -Wmissing-braces
        -Woverloaded-virtual
        -Wpedantic
        -Wreorder
        -Wreturn-type
        -Wsign-conversion
        -Wsuggest-override
        -Wswitch-enum
        -Wuninitialized
        -Wnon-virtual-dtor
        -fcolor-diagnostics
        )
endif()

set(CTEST_OUTPUT_ON_FAILURE ON)
add_compile_definitions(OUTCOME_USE_ERRORREPORT OUTCOME_USE_MACROS)
target_compile_features(outcome INTERFACE cxx_std_17)

target_link_libraries(test PRIVATE outcome Catch2::Catch2WithMain)
include(CTest)
enable_testing()
include(Catch)
catch_discover_tests(test)
