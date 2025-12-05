include(ExternalProject)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

set(DEPS_INCLUDE_DIRS "")

add_library(DEPS INTERFACE)

if(NAIADES_INCLUDE_VDB)
  include(openvdb)
  target_link_libraries(DEPS INTERFACE openvdb)
endif (NAIADES_INCLUDE_VDB)

include(hermes)
include(stb)
include(pybind11)
include(libigl)
include(eigen)

list(APPEND DEPS_INCLUDE_DIRS ";${HERMES_INCLUDE_DIR}")
list(APPEND DEPS_INCLUDE_DIRS ";${STB_INCLUDE_DIR}")
list(APPEND DEPS_INCLUDE_DIRS ";${LIBIGL_INCLUDE_DIR}")

target_link_libraries(DEPS INTERFACE 
  hermes
  Eigen3::Eigen
)

if (NAIADES_BUILD_TESTS)  
  include(catch2)
endif (NAIADES_BUILD_TESTS)

set(DEPS_INCLUDE_DIRS ${DEPS_INCLUDE_DIRS} CACHE STRING "" FORCE)
