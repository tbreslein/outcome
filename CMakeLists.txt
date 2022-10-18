# ===================================================================================================
#
# CMake file configuring the building process for outcome
#
# Copyright (c) 2022 Tommy Breslein - All Rights Reserved
#
# ===================================================================================================

cmake_minimum_required(VERSION 3.20)

project(
  outcome
  LANGUAGES CXX
  VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# create compile_commands.json in the build folder, which is needed for LSPs
# like ccls and clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE INTERNAL "Write compile_commands.json; needed for LSPs")

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# config time options
set(force_colored_output
    ON
    CACHE BOOL "Forces coloured compiler output when when using Ninja.")

set(enable-unittests
    OFF
    CACHE BOOL "Builds the unit tests in ./test/. Requires doctest.")

set(enable-clang-format
    OFF
    CACHE
      BOOL
      "Enables a test running a clang-format check over src/* and example/*. Requires clang-format and python."
)

set(enable-cppcheck
    OFF
    CACHE BOOL "Enables cppcheck test target. Requires cppcheck.")

set(enable-clang-tidy
    OFF
    CACHE
      BOOL
      "Enables running clang-tidy during build. Requires clang-tidy. Slows down compilation a lot."
)

set(enable-all-tests
    OFF
    CACHE
      BOOL
      "Shorthand for setting enable-unittests=ON, enable-clang-format=ON, enable-cppcheck=ON, and
    enable-clang-tidy=ON. Requires doctest, clang-format, clang-tidy.")

set(enable-benchmark
    OFF
    CACHE BOOL "Builds the benchmakers ./bench. Requires doctest.")

set(enable-examples
    ON
    CACHE BOOL "Builds the examples in ./examples.")

# set(enable-docs OFF CACHE BOOL "Builds the documentation. Requires doxygen.")

set(use-conan
    ON
    CACHE BOOL "Use Conan to pull in missing dependencies.")

if(${enable-all-tests})
  set(enable-unittests ON)
  set(enable-clang-format ON)
  set(enable-cppcheck ON)
  set(enable-clang-tidy ON)
endif()

if(${enable-clang-format}
   OR ${enable-cppcheck}
   OR ${enable-clang-tidy})
  set(enable-statictests ON)
else()
  set(enable-statictests OFF)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX
      "${CMAKE_CURRENT_SOURCE_DIR}"
      CACHE
        PATH
        "Set install folder; defaults to bin subfolder in the project folder."
        FORCE)
endif()

if(${enable-clang-tidy})
  set(CMAKE_CXX_CLANG_TIDY
      clang-tidy;
      -checks=*,-fuchsia-*,-llvmlibc-*,-*magic-numbers,-readability-avoid-const-params-in-decls,-altera-unroll-loops,-altera-id-dependent-backward-branch,-bugprone-easily-swappable-parameters,-*-named-parameter,-*-else-after-return,-cppcoreguidelines-macro-usage;
      -warnings-as-errors=*;
      --use-color)
endif()

# ##############################################################################
# compile options & definitions
# ##############################################################################

# Compile definitions
set(DEFAULT_COMPILE_DEFINITIONS SYSTEM_${SYSTEM_NAME_UPPER} PRIVATE
                                USE_INTERNAL_SIMD)

# MSVC compiler options
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
  set(DEFAULT_COMPILE_OPTIONS
      ${DEFAULT_COMPILE_OPTIONS}
      PRIVATE
      $<$<CONFIG:Release>:
      /Gw # whole program global optimization
      /GS- # buffer security check: no
      /GL # whole program optimization: enable link-time code generation
          # (disables Zi)
      /GF # enable string pooling
      >)
endif()

# GCC and Clang compiler options
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}"
                                               MATCHES "Clang")
  set(DEFAULT_COMPILE_OPTIONS
      ${DEFAULT_COMPILE_OPTIONS}
      PRIVATE
      $<$<CXX_COMPILER_ID:GNU>:
      $<$<BOOL:force_colored_output>:
      -fdiagnostics-color=always
      >
      >
      $<$<CXX_COMPILER_ID:Clang>:
      $<$<BOOL:force_colored_output>:
      -fcolor-diagnostics
      >
      >)
endif()

# ##############################################################################
# Conan
# ##############################################################################

# outcome only has deps when testing
if(${enable-unittests})
  if(${use-conan})
    set(conan_deps)
    # check which packages are missing that can be pulled through conan

    find_package(doctest QUIET)
    if(NOT doctest_FOUND)
      message("-- Doctest not found, pulling it through conan!")
      list(APPEND conan_deps doctest/2.4.9)
    endif()

    if(conan_deps) # REMINDER: a non-empty list is truthy
      message("-- at least one dependency is missing, using conan...")
      # setup conan.cmake if it does not exist
      list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
      list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

      if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        message(
          STATUS
            "Downloading conan.cmake from https://github.com/conan-io/cmake-conan"
        )
        file(
          DOWNLOAD
          "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
          "${CMAKE_BINARY_DIR}/conan.cmake"
          EXPECTED_HASH
            SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
          TLS_VERIFY ON)
      endif()

      include(${CMAKE_BINARY_DIR}/conan.cmake)

      conan_cmake_configure(REQUIRES ${conan_deps} GENERATORS cmake_paths
                            cmake_find_package)

      conan_cmake_autodetect(settings)

      # download deps in conanfile.txt
      conan_cmake_install(
        PATH_OR_REFERENCE
        .
        BUILD
        missing
        REMOTE
        conancenter
        SETTINGS
        ${settings})

      # add paths of conan managed packages
      include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
    endif()
  endif()
endif()

# ##############################################################################
# libraries & binaries
# ##############################################################################

# outcome library
add_subdirectory(src)

# tests
if(${enable-unittests} OR ${enable-statictests})
  enable_testing()
  find_package(doctest REQUIRED)
  set(test_lib doctest::doctest)
  include(CTest)
  include(doctest)

  add_subdirectory(tests)
endif()

# benchmarking
if(${enable-benchmark})
  add_subdirectory(bench)
endif()

# examples
if(${enable-examples})
  add_subdirectory(examples)
endif()

# docs if(${enable-docs}) add_subdirectory(doxygen) endif()

# useful soft links compile_commands
file(CREATE_LINK ${CMAKE_BINARY_DIR}/compile_commands.json
     ${CMAKE_CURRENT_LIST_DIR}/compile_commands.json SYMBOLIC)
