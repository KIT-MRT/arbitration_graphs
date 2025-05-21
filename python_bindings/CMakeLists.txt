cmake_minimum_required(VERSION 3.22)


######################
## Project settings ##
######################

# Read the project/package version from ./version file
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/version" ver)
string(REGEX MATCH "VERSION=v([0-9\.]*)" _ ${ver})
set(PACKAGE_VERSION ${CMAKE_MATCH_1})

project(
  arbitration_graphs_py
  VERSION ${PACKAGE_VERSION}
  DESCRIPTION "Python bindings for the arbitration_graphs library"
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
endif()


###################
## Find packages ##
###################

find_package(arbitration_graphs REQUIRED)
find_package(Glog REQUIRED)
find_package(pybind11 CONFIG)
find_package(util_caching REQUIRED)
find_package(Yaml-cpp REQUIRED)


###########
## Build ##
###########

pybind11_add_module(${PROJECT_NAME}
  src/bindings.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE
  include
)
target_link_libraries(${PROJECT_NAME} PUBLIC
  arbitration_graphs
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
  PROJECT_VERSION="${PROJECT_VERSION}"
)


#############
## Install ##
#############

install(TARGETS ${PROJECT_NAME} DESTINATION "arbitration_graphs")

