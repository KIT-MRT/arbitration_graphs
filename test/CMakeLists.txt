cmake_minimum_required(VERSION 3.22)


######################
## Project settings ##
######################

# We support building this as top-level project, e.g. in order to test the lib installation
project(arbitration_graphs_tests
  LANGUAGES CXX
)


###############
## C++ setup ##
###############

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  # Require C++17
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)

  # Let's ensure -std=c++xx instead of -std=g++xx
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Let's nicely support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Allow clangd and others to properl y understand this C++ project
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  # Testing only available if this is the main project
  # Note this needs to be done in the main CMakeLists
  # since it calls enable_testing, which must be in the
  # main CMakeLists.
  include(CTest)
endif()


###################
## Find packages ##
###################

find_package(GTest)
find_package(pybind11 CONFIG)

if(NOT GTEST_FOUND AND NOT pybind11_FOUND)
  message(WARNING "Neither GTest nor pybind11 found. Cannot compile tests!")
endif()

# Find installed lib and its dependencies, if this is build as top-level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  find_package(Glog REQUIRED)
  find_package(util_caching REQUIRED)
  find_package(Yaml-cpp REQUIRED)

  find_package(arbitration_graphs REQUIRED)
endif()


####################
## C++ Unit Tests ##
####################

if(GTEST_FOUND)
  file(GLOB_RECURSE _tests CONFIGURE_DEPENDS "*.cpp" "*.cc")
  list(FILTER _tests EXCLUDE REGEX "${CMAKE_CURRENT_BINARY_DIR}")
  list(REMOVE_ITEM _tests "${CMAKE_CURRENT_SOURCE_DIR}/python_bindings.cpp")

  foreach(_test ${_tests})
    get_filename_component(_test_name ${_test} NAME_WE)
    # make sure we add only one -test to the target
    string(REGEX REPLACE "-test" "" TEST_TARGET_NAME ${_test_name})
    set(TEST_TARGET_NAME arbitration_graphs-gtest-${TEST_TARGET_NAME})

    message(STATUS
      "Adding gtest unittest \"${TEST_TARGET_NAME}\" with working dir ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_FOLDER} \n _test: ${_test}"
    )

    add_executable(${TEST_TARGET_NAME} ${_test})

    target_link_libraries(${TEST_TARGET_NAME} PUBLIC
      ${GTEST_BOTH_LIBRARIES} pthread
      arbitration_graphs
    )

    add_test(NAME ${TEST_TARGET_NAME}
      COMMAND ${TEST_TARGET_NAME}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
  endforeach()
endif()

#######################
## Python Unit Tests ##
#######################
  
if(pybind11_FOUND)
  find_package(Python3 REQUIRED)

  # Build the individual pybind modules
  pybind11_add_module(arbitration_graphs_py
    python_bindings.cpp
  )
  target_link_libraries(arbitration_graphs_py PUBLIC
    arbitration_graphs
  )
  pybind11_add_module(arbitration_graphs_py_with_subcommand
    python_bindings.cpp
  )
  target_link_libraries(arbitration_graphs_py_with_subcommand PUBLIC
    arbitration_graphs
  )
  pybind11_add_module(arbitration_graphs_py_with_verifier
    python_bindings.cpp
  )
  target_link_libraries(arbitration_graphs_py_with_verifier PUBLIC
    arbitration_graphs
  )

  # Copy required modules to the build directory
  # These do not contain tests but are roughly equivalent to the C++ test-specific header files.
  configure_file(cost_estimator.py cost_estimator.py COPYONLY)
  configure_file(dummy_types.py dummy_types.py COPYONLY)

  # Copy Python test files to the build directory and add them as tests.
  # Of course, python tests do not need to be compiled.
  # However, we can easily import the compiled pybind11 module if they are in the same directory.
  # We also want to be able run the using ctest to keep things consistent with the C++ tests.
  file(GLOB_RECURSE _py_tests CONFIGURE_DEPENDS "*_test.py")
  foreach(_py_test ${_py_tests})
    get_filename_component(_py_test_name ${_py_test} NAME)
    string(REGEX REPLACE "_test" "" PY_TEST_NAME ${_py_test_name})
    set(PY_TEST_NAME arbitration_graphs-pytest-${PY_TEST_NAME})

    message(STATUS
      "Adding python unittest \"${PY_TEST_NAME}\" with working dir ${PROJECT_SOURCE_DIR}/${TEST_FOLDER} \n _test: ${_py_test}"
    )

    configure_file(${_py_test} ${PY_TEST_NAME} COPYONLY)

    add_test(NAME ${PY_TEST_NAME}
      COMMAND ${Python3_EXECUTABLE} -m unittest ${PY_TEST_NAME}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
  endforeach()
endif()
