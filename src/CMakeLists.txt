cmake_minimum_required(VERSION 3.12)
project(configure-defines.h)

set(NGT_SHARED_MEMORY_ALLOCATOR ON)
set(NGT_AVX_DISABLED TRUE) # TODO: check if AVX is available

file(STRINGS "NGT/VERSION" ngt_VERSION)
message(STATUS "NGT_VERSION: ${ngt_VERSION}")
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/NGT/lib/NGT/version_defs.h" "#define NGT_VERSION \"${ngt_VERSION}\" \n")

if(${UNIX})
  if(${NGT_SHARED_MEMORY_ALLOCATOR})
    set(NGT_QBG_DISABLED TRUE) # if QBG is enabled, requires BLAS and LAPACK
  endif(${NGT_SHARED_MEMORY_ALLOCATOR})

  if(${NGT_AVX_DISABLED})
    message(STATUS "AVX will not be used to compute distances.")
  endif()

  if(${NGT_QBG_DISABLED})
    message(STATUS "QBG is disabled.")
  endif()

  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/NGT/lib/NGT/defines.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/NGT/lib/NGT/defines.h")
endif()
