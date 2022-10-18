# ===================================================================================================
#
# CMake file for building outcome's tests
#
# Copyright (c) 2022 Tommy Breslein - All Rights Reserved
#
# ===================================================================================================

if(${enable-unittests})
  add_subdirectory(unittests)
endif()

if(${enable-statictests})
  add_subdirectory(static)
endif()
