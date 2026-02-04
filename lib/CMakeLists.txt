# ##############################################################################
# naiades lib                                                                 ##
# ##############################################################################
set(NAIADES_HEADERS
  ${NAIADES_SOURCE_DIR}/naiades/base/debug.h
  ${NAIADES_SOURCE_DIR}/naiades/base/result.h

  ${NAIADES_SOURCE_DIR}/naiades/core/element.h
  ${NAIADES_SOURCE_DIR}/naiades/core/field.h
  ${NAIADES_SOURCE_DIR}/naiades/core/geometry.h
  ${NAIADES_SOURCE_DIR}/naiades/core/neighbourhood.h
  ${NAIADES_SOURCE_DIR}/naiades/core/topology.h

  ${NAIADES_SOURCE_DIR}/naiades/geo/grid.h

  ${NAIADES_SOURCE_DIR}/naiades/numeric/blas.h
  ${NAIADES_SOURCE_DIR}/naiades/numeric/boundary.h
  ${NAIADES_SOURCE_DIR}/naiades/numeric/boundary_conditions.h
  ${NAIADES_SOURCE_DIR}/naiades/numeric/discrete_operator.h
  ${NAIADES_SOURCE_DIR}/naiades/numeric/spatial_discretization.h

  ${NAIADES_SOURCE_DIR}/naiades/sampling/sampler.h
  ${NAIADES_SOURCE_DIR}/naiades/sampling/stencil.h
  
  ${NAIADES_SOURCE_DIR}/naiades/solvers/convection.h
  ${NAIADES_SOURCE_DIR}/naiades/solvers/sim_control.h
  # ${NAIADES_SOURCE_DIR}/naiades/solvers/smoke_solver.h

  ${NAIADES_SOURCE_DIR}/naiades/utils/fields.h
  ${NAIADES_SOURCE_DIR}/naiades/utils/math.h
  ${NAIADES_SOURCE_DIR}/naiades/utils/utils.h
)

set(NAIADES_SOURCES
  ${NAIADES_SOURCE_DIR}/naiades/core/element.cpp
  ${NAIADES_SOURCE_DIR}/naiades/core/field.cpp
  ${NAIADES_SOURCE_DIR}/naiades/core/neighbourhood.cpp
  
  ${NAIADES_SOURCE_DIR}/naiades/geo/grid.cpp

  ${NAIADES_SOURCE_DIR}/naiades/numeric/boundary.cpp
  ${NAIADES_SOURCE_DIR}/naiades/numeric/discrete_operator.cpp
  ${NAIADES_SOURCE_DIR}/naiades/numeric/spatial_discretization.cpp

  ${NAIADES_SOURCE_DIR}/naiades/sampling/stencil.cpp

  ${NAIADES_SOURCE_DIR}/naiades/solvers/sim_control.cpp
  # ${NAIADES_SOURCE_DIR}/naiades/solvers/smoke_solver.cpp
  
  ${NAIADES_SOURCE_DIR}/naiades/utils/fields.cpp
  ${NAIADES_SOURCE_DIR}/naiades/utils/math.cpp
  ${NAIADES_SOURCE_DIR}/naiades/utils/utils.cpp
)

add_library(naiades STATIC ${NAIADES_SOURCES} ${NAIADES_HEADERS})
add_dependencies(naiades hermes)
target_include_directories(
  naiades PRIVATE  
  ${DEPS_INCLUDE_DIRS}
  ${NAIADES_SOURCE_DIR}
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

 if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(
      naiades
      PUBLIC
      # cuda
      "$<IF:$<AND:$<CONFIG:DEBUG>,$<COMPILE_LANGUAGE:CUDA>>,-Xcompiler=-Wextra,>"
      "$<IF:$<AND:$<CONFIG:DEBUG>,$<COMPILE_LANGUAGE:CUDA>>,-Xcompiler=-Wall,>"
      "$<IF:$<AND:$<CONFIG:DEBUG>,$<COMPILE_LANGUAGE:CUDA>>,-Xcompiler=-Werror,>"
      "$<IF:$<AND:$<CONFIG:DEBUG>,$<COMPILE_LANGUAGE:CUDA>>,-Xcompiler=-O0,>"
      # CXX
      "$<IF:$<AND:$<CONFIG:DEBUG>,$<COMPILE_LANGUAGE:CXX>>,-Wextra,>"
      "$<IF:$<AND:$<CONFIG:DEBUG>,$<COMPILE_LANGUAGE:CXX>>,-Wall,>"
      "$<IF:$<AND:$<CONFIG:DEBUG>,$<COMPILE_LANGUAGE:CXX>>,-Werror,>"
      "$<IF:$<AND:$<CONFIG:DEBUG>,$<COMPILE_LANGUAGE:CXX>>,-O0,>"
      # common
      "$<$<CONFIG:Debug>:-g;-pg;-fprofile-arcs;-ftest-coverage;--coverage;-fPIC>"
      "$<$<CONFIG:Release>:-O3;-fPIC;>")
  endif(CMAKE_COMPILER_IS_GNUCXX)

# ##############################################################################
# python                                                                      ##
# ##############################################################################
#pybind11_add_module(naiades_py naiades_py.cpp)
#
#add_dependencies(naiades_py hermes)
#target_link_libraries(naiades_py PUBLIC 
#  DEPS
#  naiades
#)
#
#target_include_directories(naiades_py PUBLIC 
#  ${DEPS_INCLUDE_DIRS}
#  ${NAIADES_SOURCE_DIR})
#
#if(NAIADES_INCLUDE_TO_STRING_METHODS)
#  target_compile_definitions(naiades_py PUBLIC NAIADES_INCLUDE_TO_STRING)
#  target_compile_definitions(naiades_py PUBLIC HERMES_INCLUDE_TO_STRING)
#endif(NAIADES_INCLUDE_TO_STRING_METHODS)
