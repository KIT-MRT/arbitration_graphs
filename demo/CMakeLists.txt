cmake_minimum_required(VERSION 3.22)
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

if(BUILD_TESTS)
  message("Building with tests")
  add_subdirectory(test)
endif()
