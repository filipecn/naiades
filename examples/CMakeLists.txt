function(build_example EXAMPLE_NAME)

  add_executable(${EXAMPLE_NAME} ${EXAMPLE_NAME}.cpp)

  target_include_directories(${EXAMPLE_NAME} PUBLIC
  ${CATCH2_INCLUDES}
  ${NAIADES_SOURCE_DIR}
  ${DEPS_INCLUDE_DIRS}
  )

  setup_target(${EXAMPLE_NAME})

  if(NAIADES_INCLUDE_DEBUG_TRAITS)
    target_compile_definitions(${EXAMPLE_NAME} PUBLIC NAIADES_INCLUDE_DEBUG_TRAITS)
    target_compile_definitions(${EXAMPLE_NAME} PUBLIC HERMES_INCLUDE_DEBUG_TRAITS)
  endif(NAIADES_INCLUDE_DEBUG_TRAITS)

  target_link_libraries(${EXAMPLE_NAME} PRIVATE
  DEPS
  naiades
  )

endfunction(build_example)

build_example(grid_fd_lid_driven_cavity)
build_example(grid_fd_poisson)
build_example(grid_rbf_poisson)
build_example(mesh_rbf_poisson)
build_example(morton_tree)
