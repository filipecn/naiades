add_executable(naiades_tests 
  geo_tests.cpp
  sampling_tests.cpp
)

target_link_options(naiades_tests INTERFACE --coverage)

add_dependencies(naiades_tests Catch2 naiades)

target_include_directories(naiades_tests PUBLIC 
  ${CATCH2_INCLUDES} 
  ${NAIADES_SOURCE_DIR}
  ${DEPS_INCLUDE_DIRS}
)

setup_target(naiades_tests)

if(NAIADES_INCLUDE_TO_STRING_METHODS)
  target_compile_definitions(naiades_tests PUBLIC NAIADES_INCLUDE_TO_STRING)
  target_compile_definitions(naiades_tests PUBLIC HERMES_INCLUDE_TO_STRING)
endif(NAIADES_INCLUDE_TO_STRING_METHODS)

target_link_libraries(naiades_tests PRIVATE 
  naiades
  hermes
  Catch2::Catch2WithMain
)

if (UNIX)

  target_link_libraries(naiades_tests 
    PRIVATE
    gcov
  )

  add_custom_target(test_naiades
    COMMAND lcov --directory . --zerocounters
    COMMAND naiades_tests DEPENDS naiades_tests
    COMMAND lcov --directory . --capture -output-file cov.info
    COMMAND lcov --remove cov.info '/usr/*' --output-file cov.info
    COMMAND lcov --remove cov.info '*/ext/*' --output-file cov.info
    COMMAND lcov --remove cov.info '*/tests/*' --output-file cov.info
    COMMAND genhtml cov.info --output-directory out
  )

endif (UNIX)


list(APPEND CMAKE_MODULE_PATH ${CATCH2_EXTRAS})
enable_testing()
include(CTest) 
include(Catch)
catch_discover_tests(naiades_tests)
