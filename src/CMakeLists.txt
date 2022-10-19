# ===================================================================================================
#
# CMake file for building outcome
#
# Copyright (c) 2022 Tommy Breslein - All Rights Reserved
#
# ===================================================================================================

# project( outcome LANGUAGES CXX VERSION 0.0.1)

# configuration
set(PROJECT_VERSION ${CMAKE_PROJECT_VERSION})
add_library(${PROJECT_NAME} STATIC)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_sources(${PROJECT_NAME} PRIVATE "outcome.hpp")

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC # NEEDS to be public, so the includes can travel into the executable
         ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

target_compile_options(${PROJECT_NAME} PRIVATE ${DEFAULT_COMPILE_OPTIONS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${required_deps})
