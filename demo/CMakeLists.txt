cmake_minimum_required(VERSION 3.5.1)
project(arbitration_graphs_pacman_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


###################
## Find packages ##
###################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
find_package(arbitration_graphs REQUIRED)
find_package(EnTT_Pacman REQUIRED)
find_package(Glog)
find_package(SDL2 REQUIRED)
find_package(util_caching REQUIRED)
find_package(Yaml-cpp REQUIRED)

###########
## Build ##
###########

add_library(${PROJECT_NAME} STATIC
  src/astar.cpp
  src/avoid_ghost_behavior.cpp
  src/change_dot_cluster_behavior.cpp
  src/chase_ghost_behavior.cpp
  src/cost_estimator.cpp
  src/cluster.cpp
  src/entities.cpp
  src/environment_model.cpp
  src/move_randomly_behavior.cpp
  src/utils.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    ${SDL2_INCLUDE_DIR}
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    arbitration_graphs
    EnTT_Pacman
    util_caching
    ${SDL2_LIBRARY}
    ${YAML_CPP_LIBRARIES}
)

add_executable(${PROJECT_NAME}_exe
  src/main.cpp
  src/pacman_wrapper.cpp
)
target_include_directories(${PROJECT_NAME}_exe PRIVATE
    include
    ${SDL2_INCLUDE_DIR}
)
target_link_libraries(${PROJECT_NAME}_exe PRIVATE
    ${PROJECT_NAME}
    EnTT_Pacman
    util_caching
    ${SDL2_LIBRARY}
    ${YAML_CPP_LIBRARIES}
)

#############
## Testing ##
#############
find_package(GTest)

if(GTEST_FOUND)
  if(BUILD_TESTS)
    file(GLOB_RECURSE _tests "test/*.cpp" "test/*.cc")

    foreach(_test ${_tests})
      get_filename_component(_test_name ${_test} NAME_WE)
      # make sure we add only one -test to the target
      string(REGEX REPLACE "-test" "" TEST_TARGET_NAME ${_test_name})
      set(TEST_TARGET_NAME ${PROJECT_NAME}-gtest-${TEST_TARGET_NAME})

      message(STATUS
        "Adding gtest unittest \"${TEST_TARGET_NAME}\" with working dir ${PROJECT_SOURCE_DIR}/${TEST_FOLDER} \n _test: ${_test}"
      )

      add_executable(${TEST_TARGET_NAME} ${_test})

      target_include_directories(${TEST_TARGET_NAME}
          PRIVATE
          include
          ${SDL2_INCLUDE_DIR}
      )
      target_link_libraries(${TEST_TARGET_NAME}
          PRIVATE
          ${GTEST_BOTH_LIBRARIES} pthread
          ${PROJECT_NAME}
          EnTT_Pacman
      )
    endforeach()
  endif()
endif()

