cmake_minimum_required(VERSION 3.5.1)
project(arbitration_graphs_pacman_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


###################
## Find packages ##
###################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
find_package(SDL2 REQUIRED)
find_package(Glog)
find_package(arbitration_graphs REQUIRED)
find_package(Yaml-cpp REQUIRED)



########################
## Build dependencies ##
########################
add_subdirectory(EnTT-Pacman)
# find_package(EnTT_Pacman REQUIRED)


###########
## Build ##
###########
add_library(lib${PROJECT_NAME} STATIC
  src/environment_model.cpp
  src/run_away_from_ghost_behavior.cpp
)
target_include_directories(lib${PROJECT_NAME}
    PRIVATE
    include
    EnTT-Pacman/src
    EnTT-Pacman/third_party
    ${SDL2_INCLUDE_DIR}
)
target_link_libraries(lib${PROJECT_NAME}
    PRIVATE
    EnTT_Pacman
    ${SDL2_LIBRARY}
    ${YAML_CPP_LIBRARIES}
)

add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME}
    PRIVATE
    include
    EnTT-Pacman/src
    EnTT-Pacman/third_party
    ${SDL2_INCLUDE_DIR}
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    lib${PROJECT_NAME}
    EnTT_Pacman
    ${SDL2_LIBRARY}
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
          EnTT-Pacman/src
          EnTT-Pacman/third_party
          ${SDL2_INCLUDE_DIR}
      )
      target_link_libraries(${TEST_TARGET_NAME}
          PRIVATE
          ${GTEST_BOTH_LIBRARIES} pthread
          lib${PROJECT_NAME}
          EnTT_Pacman
      )
    endforeach()
  endif()
endif()