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

set (SRC
  "softkernel.cpp"
  "dwarfwalk.cpp")

add_executable(sk ${SRC})

target_link_libraries(sk
  pthread
  ${LIBDW_LDFLAGS}
  ${LIBELF_LDFLAGS}
  )
