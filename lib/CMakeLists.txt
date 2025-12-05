# ##############################################################################
# naiades lib                                                                ##
# ##############################################################################
set(NAIADES_HEADERS
  ${NAIADES_SOURCE_DIR}/lib/naiades/base/debug.h
  ${NAIADES_SOURCE_DIR}/lib/naiades/base/result.h

  ${NAIADES_SOURCE_DIR}/lib/naiades/core/field.h

  ${NAIADES_SOURCE_DIR}/lib/naiades/geo/grid.h

  ${NAIADES_SOURCE_DIR}/lib/naiades/sampling/stencil.h
  
  ${NAIADES_SOURCE_DIR}/lib/naiades/solvers/smoke_solver.h

  ${NAIADES_SOURCE_DIR}/lib/naiades/utils/fields.h
)

set(NAIADES_SOURCES
  ${NAIADES_SOURCE_DIR}/lib/naiades/core/field.cpp
  
  ${NAIADES_SOURCE_DIR}/lib/naiades/geo/grid.cpp

  ${NAIADES_SOURCE_DIR}/lib/naiades/sampling/stencil.cpp

  ${NAIADES_SOURCE_DIR}/lib/naiades/solvers/smoke_solver.cpp
  
  ${NAIADES_SOURCE_DIR}/lib/naiades/utils/fields.cpp
  
)

add_library(naiades STATIC ${NAIADES_SOURCES} ${NAIADES_HEADERS})
add_dependencies(naiades hermes)
target_include_directories(
  naiades PRIVATE  
  ${DEPS_INCLUDE_DIRS}
  ${NAIADES_SOURCE_DIR}/lib
)

if(NAIADES_INCLUDE_TO_STRING_METHODS)
  target_compile_definitions(naiades PUBLIC NAIADES_INCLUDE_TO_STRING)
  target_compile_definitions(naiades PUBLIC HERMES_INCLUDE_TO_STRING)
endif(NAIADES_INCLUDE_TO_STRING_METHODS)

  set_target_properties(
    naiades
    PROPERTIES CXX_STANDART 23
    CXX_STANDARD_REQUIRED YES
    LINKER_LANGUAGE CXX
  )

# ##############################################################################
# python                                                                      ##
# ##############################################################################
pybind11_add_module(naiades_py naiades_py.cpp)

add_dependencies(naiades_py hermes)
target_link_libraries(naiades_py PUBLIC 
  DEPS
  naiades
)

target_include_directories(naiades_py PUBLIC 
  ${DEPS_INCLUDE_DIRS}
  ${NAIADES_SOURCE_DIR}/lib)

if(NAIADES_INCLUDE_TO_STRING_METHODS)
  target_compile_definitions(naiades_py PUBLIC NAIADES_INCLUDE_TO_STRING)
  target_compile_definitions(naiades_py PUBLIC HERMES_INCLUDE_TO_STRING)
endif(NAIADES_INCLUDE_TO_STRING_METHODS)
