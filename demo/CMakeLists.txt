cmake_minimum_required(VERSION 3.22)


######################
## Project settings ##
######################

# Setup project
project(arbitration_graphs_pacman_demo
  DESCRIPTION "Demo and tutorial project for teaching arbitration_graphs"
  LANGUAGES CXX
)

# Select project features
set(BUILD_TESTS "FALSE" CACHE STRING "Compile the project with all unit tests")


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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
find_package(arbitration_graphs REQUIRED)
find_package(EnTT_Pacman REQUIRED)
find_package(Glog)
find_package(SDL2 REQUIRED)
find_package(util_caching REQUIRED)
find_package(Yaml-cpp REQUIRED)

find_package(Crow)

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
    Crow::Crow
    EnTT_Pacman
    util_caching
    ${SDL2_LIBRARY}
    ${YAML_CPP_LIBRARIES}
)


#############
## Testing ##
#############

# Testing only available if this is the main project
# Emergency override ARBITRATION_GRAPHS_DEMO_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR ARBITRATION_GRAPHS_DEMO_CMAKE_BUILD_TESTING)
   AND BUILD_TESTS)
  add_subdirectory(test)
endif()
