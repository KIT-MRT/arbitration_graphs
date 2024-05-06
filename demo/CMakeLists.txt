cmake_minimum_required(VERSION 3.5.1)
project(arbitration_graphs_pacman_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


###################
## Find packages ##
###################

# list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Glog)

# find_package(util_caching REQUIRED)
find_package(arbitration_graphs REQUIRED)

# find_package(Yaml-cpp REQUIRED)



########################
## Build dependencies ##
########################

# add_subdirectory(EnTT-Pacman)
find_package(EnTT_Pacman REQUIRED)


############################
## Read source code files ##
############################
# file(GLOB_RECURSE PROJECT_HEADER_FILES_INC "include/${PROJECT_NAME}/*.h" "include/${PROJECT_NAME}/*.hpp")


###########
## Build ##
###########
# Declare a cpp library
add_executable(${PROJECT_NAME} src/main.cpp)
# target_include_directories(${PROJECT_NAME} INTERFACE
#     $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}>
#     $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
# )
target_link_libraries(${PROJECT_NAME}
  glog::glog
  EnTT_Pacman
)
