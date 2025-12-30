add_executable(smoke_solver2
  smoke_solver2.cpp
)

target_link_options(smoke_solver2 INTERFACE --coverage)

target_include_directories(smoke_solver2 PUBLIC 
  ${CATCH2_INCLUDES} 
  ${NAIADES_SOURCE_DIR}
  ${DEPS_INCLUDE_DIRS}
)

setup_target(smoke_solver2)

if(NAIADES_INCLUDE_TO_STRING_METHODS)
  target_compile_definitions(smoke_solver2 PUBLIC NAIADES_INCLUDE_TO_STRING)
  target_compile_definitions(smoke_solver2 PUBLIC HERMES_INCLUDE_TO_STRING)
endif(NAIADES_INCLUDE_TO_STRING_METHODS)

target_link_libraries(smoke_solver2 PRIVATE 
  naiades
  hermes
)

