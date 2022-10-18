# ===================================================================================================
#
# CMake file for building outcome's benchmarks
#
# Copyright (c) 2022 Tommy Breslein - All Rights Reserved
#
# ===================================================================================================

project(outcome_benchmarks LANGUAGES CXX)

set(bench_list bench)

foreach(bench_name ${bench_list})
  add_executable(${bench_name} "${bench_name}.cpp")
  target_compile_options(${bench_name} ${DEFAULT_COMPILE_OPTIONS})
  target_link_libraries(${bench_name} PRIVATE outcome ${required_deps})
endforeach()
