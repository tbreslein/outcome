# ===================================================================================================
#
# CMake file for building outcome's static tests
#
# Copyright (c) 2022 Tommy Breslein - All Rights Reserved
#
# ===================================================================================================

if(${enable-clang-format})
  find_program(found_clang_format NAMES clang-format)

  if(NOT found_clang_format)
    message(
      WARNING
        "could not find clang-format in your path; skipping clang-format tests."
    )
  else()
    add_test(NAME "clang_format_test"
             COMMAND ${PROJECT_SOURCE_DIR}/clang_format_test
                     ${PROJECT_SOURCE_DIR})
  endif()
endif()

if(${enable-cppcheck})
  find_program(found_cppcheck NAMES cppcheck)
  if(NOT found_cppcheck)
    message(
      WARNING "could not find cppcheck in your path; skipping cppcheck tests.")
  else()
    add_test(NAME "cppcheck_test"
             COMMAND cppcheck --quiet --error-exitcode=1 --enable=all
                     --check-config ${PROJECT_SOURCE_DIR}/src)
  endif()
endif()
