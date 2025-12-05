# ##############################################################################
# CMAKE                                                                        #
# ##############################################################################
cmake_minimum_required(VERSION 3.12..3.30 FATAL_ERROR)

project(
  NAIADES
  VERSION 0.01
  DESCRIPTION "Fluid simulation framework"
  LANGUAGES C CXX)

cmake_path(SET NAIADES_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
cmake_path(SET NAIADES_EXAMPLES_DIR ${CMAKE_CURRENT_LIST_DIR}/examples)

# cmake print functions
include(CMakePrintHelpers)

# ##############################################################################
# OPTIONS                                                                      #
# ##############################################################################
option(NAIADES_BUILD_ALL "build all targets" OFF)
option(NAIADES_BUILD_TESTS "build tests" OFF)
option(NAIADES_BUILD_EXAMPLES "build examples" OFF)
option(NAIADES_BUILD_DOCS "build library documentation" OFF)
option(NAIADES_INCLUDE_TO_STRING_METHODS "enable naiades::to_string methods" ON)

# ##############################################################################
# COMPILATION                                                                  #
# ##############################################################################
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(ConfigCompilerFlags)
include(ExternalPrograms)

# ##############################################################################
# DEPENDENCIES                                                                 #
# ##############################################################################
add_subdirectory(ext)

# ##############################################################################
# ENGINE                                                                       #
# ##############################################################################
add_subdirectory(lib)

# ##############################################################################
# TESTS                                                                        #
# ##############################################################################
#if (NAIADES_BUILD_TESTS)
  # add_subdirectory(tests)
  #endif (NAIADES_BUILD_TESTS)

# ##############################################################################
# COMPILATION/RUN SANITY TEST                                                  #
# ##############################################################################
# try_compile(COMPILATION_TEST_RESULT bindir SOURCES source.cpp)
# try_run(RUN_TEST_RESULT bindr SOURCES source.cpp)

if (NAIADES_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif (NAIADES_BUILD_EXAMPLES)

# ##############################################################################
#                                                                DOCUMENTATION #
# ##############################################################################
if(NAIADES_BUILD_DOCS)
  include(GenerateDocs)
endif(NAIADES_BUILD_DOCS)
