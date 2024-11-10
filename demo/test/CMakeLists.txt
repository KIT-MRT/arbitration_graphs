cmake_minimum_required(VERSION 3.22)


######################
## Project settings ##
######################

# We support building this as top-level project, e.g. in order to test the lib installation
project(arbitration_graphs_demo_tests
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

  # Allow clangd and others to properly understand this C++ project
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

# Find installed lib and its dependencies, if this is build as top-level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

  find_package(arbitration_graphs_pacman_demo REQUIRED)

  find_package(arbitration_graphs REQUIRED)
  find_package(EnTT_Pacman REQUIRED)
  find_package(Glog REQUIRED)
  find_package(SDL2 REQUIRED)
  find_package(util_caching REQUIRED)
  find_package(Yaml-cpp REQUIRED)
endif()


###########
## Build ##
###########

if(GTEST_FOUND)
  file(GLOB_RECURSE _tests CONFIGURE_DEPENDS "*.cpp" "*.cc")
  list(FILTER _tests EXCLUDE REGEX "${CMAKE_CURRENT_BINARY_DIR}")

  foreach(_test ${_tests})
    get_filename_component(_test_name ${_test} NAME_WE)
    # make sure we add only one -test to the target
    string(REGEX REPLACE "-test" "" TEST_TARGET_NAME ${_test_name})
    set(TEST_TARGET_NAME arbitration_graphs_pacman_demo-gtest-${TEST_TARGET_NAME})

    message(STATUS
      "Adding gtest unittest \"${TEST_TARGET_NAME}\" with working dir ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_FOLDER} \n _test: ${_test}"
    )

    add_executable(${TEST_TARGET_NAME} ${_test})

    target_include_directories(${TEST_TARGET_NAME} PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/arbitration_graphs_pacman_demo>
        ${SDL2_INCLUDE_DIR}
    )

    target_link_libraries(${TEST_TARGET_NAME} PUBLIC
      ${GTEST_BOTH_LIBRARIES}
      pthread
      arbitration_graphs_pacman_demo_lib
      arbitration_graphs
      EnTT_Pacman
      util_caching
      ${SDL2_LIBRARY}
      ${YAML_CPP_LIBRARIES}
    )

    add_test(NAME ${TEST_TARGET_NAME}
      COMMAND ${TEST_TARGET_NAME}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
  endforeach()
else()
  message(WARNING "GTest not found. Cannot compile tests!")
endif()
