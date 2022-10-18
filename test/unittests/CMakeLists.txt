# ===================================================================================================
#
# CMake file for building outcome's unit tests
#
# Copyright (c) 2022 Tommy Breslein - All Rights Reserved
#
# ===================================================================================================

set(unittest_list map)

find_program(found_valgrind NAMES valgrind)

foreach(test_name ${unittest_list})
  set(test_exe "test_${test_name}")
  add_executable(${test_exe} "${test_name}.cpp")

  target_compile_options(${test_exe} ${DEFAULT_COMPILE_OPTIONS})

  target_compile_definitions(${test_exe} PRIVATE ${DEFAULT_COMPILE_DEFINITIONS})

  target_link_libraries(${test_exe} PRIVATE outcome ${required_deps}
                                            ${test_lib})

  # provided by doctest cmake module; shorthand for add_test with a some default
  # configuration
  doctest_discover_tests(${test_exe})
endforeach()
