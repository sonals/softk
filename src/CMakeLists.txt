# SPDX-License-Identifier: Apache-2.0
# Copyright (C) 2021 Xilinx, Inc. All rights reserved.

PROJECT(softkernel)
CMAKE_MINIMUM_REQUIRED(VERSION 3.5.0)

SET(PROJECT_DESCRIPTION "https://github.com/sonals/softkernel")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FindPkgConfig)

pkg_check_modules(LIBDW REQUIRED libdw)

IF(LIBDW_FOUND)
   MESSAGE(STATUS "Found libdw at ${LIBDW_PREFIX} ${LIBDW_VERSION}")
   INCLUDE_DIRECTORIES(${LIBDW_INCLUDEDIR})
ELSE(LIBDW_FOUND)
   MESSAGE(FATAL_ERROR "libdw not found")
ENDIF(LIBDW_FOUND)

pkg_check_modules(LIBELF REQUIRED libelf)

IF(LIBELF_FOUND)
   MESSAGE(STATUS "Found libelf at ${LIBELF_PREFIX} ${LIBELF_VERSION}")
   INCLUDE_DIRECTORIES(${LIBELF_INCLUDEDIR})
ELSE(LIBELF_FOUND)
   MESSAGE(FATAL_ERROR "libelf not found")
ENDIF(LIBELF_FOUND)

pkg_check_modules(LIBFFI REQUIRED libffi)

IF(LIBFFI_FOUND)
   MESSAGE(STATUS "Found libffi at ${LIBFFI_PREFIX} ${LIBFFI_VERSION}")
   INCLUDE_DIRECTORIES(${LIBFFI_INCLUDEDIR})
ELSE(LIBFFI_FOUND)
   MESSAGE(FATAL_ERROR "libffi not found")
ENDIF(LIBFFI_FOUND)

include (CTest)

set (SRC
  "main.cpp"
  "dwarfwalk.cpp"
  "skutils.cpp"
  )

add_executable(sk ${SRC})

target_compile_options(sk PRIVATE -Werror -Wall)

set_target_properties(sk PROPERTIES
  CXX_VISIBILITY_PRESET "hidden"
  )

target_link_libraries(sk
  pthread
  ${LIBDW_LDFLAGS}
  ${LIBELF_LDFLAGS}
  ${LIBFFI_LDFLAGS}
  )

enable_testing()
  add_test(NAME smoke
  COMMAND ${CMAKE_BINARY_DIR}/sk ${CMAKE_BINARY_DIR}/sk
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
