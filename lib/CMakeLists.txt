# ##############################################################################
# naiades lib                                                                ##
# ##############################################################################
set(NAIADES_HEADERS
  ${NAIADES_SOURCE_DIR}/lib/naiades/base/debug.h
  ${NAIADES_SOURCE_DIR}/lib/naiades/base/result.h

  ${NAIADES_SOURCE_DIR}/lib/naiades/core/field.h

  ${NAIADES_SOURCE_DIR}/lib/naiades/geo/grid.h

  ${NAIADES_SOURCE_DIR}/lib/naiades/sampling/sampler.h
  ${NAIADES_SOURCE_DIR}/lib/naiades/sampling/stencil.h
  
  ${NAIADES_SOURCE_DIR}/lib/naiades/solvers/smoke_solver.h

  ${NAIADES_SOURCE_DIR}/lib/naiades/utils/fields.h
  ${NAIADES_SOURCE_DIR}/lib/naiades/utils/math.h
)

set(NAIADES_SOURCES
  ${NAIADES_SOURCE_DIR}/lib/naiades/core/field.cpp
  
  ${NAIADES_SOURCE_DIR}/lib/naiades/geo/grid.cpp

  ${NAIADES_SOURCE_DIR}/lib/naiades/sampling/stencil.cpp

  ${NAIADES_SOURCE_DIR}/lib/naiades/solvers/smoke_solver.cpp
  
  ${NAIADES_SOURCE_DIR}/lib/naiades/utils/fields.cpp
  ${NAIADES_SOURCE_DIR}/lib/naiades/utils/math.cpp
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
