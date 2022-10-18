# ===================================================================================================
#
# CMake file for building outcome's examples
#
# Copyright (c) 2022 Tommy Breslein - All Rights Reserved
#
# ===================================================================================================

set(example_list map)

foreach(example_name ${example_list})
  add_executable(${example_name} "${example_name}.cpp")

  target_compile_options(${example_name} ${DEFAULT_COMPILE_OPTIONS})

  target_compile_definitions(${example_name}
                             PRIVATE ${DEFAULT_COMPILE_DEFINITIONS})

  target_link_libraries(${example_name} PRIVATE outcome ${required_deps})

  install(TARGETS ${example_name} DESTINATION bin)
endforeach()
