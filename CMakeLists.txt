cmake_minimum_required(VERSION 3.22)


######################
## Project settings ##
######################

# Read the project/package version from ./version file
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/version" ver)
string(REGEX MATCH "VERSION=v([0-9\.]*)" _ ${ver})
set(PACKAGE_VERSION ${CMAKE_MATCH_1})

# Setup project
project(arbitration_graphs
  VERSION ${PACKAGE_VERSION}
  DESCRIPTION "Hierarchical behavior models for complex decision-making and behavior generation in robotics"
  LANGUAGES CXX
)

# Select project features
set(BUILD_GUI "TRUE" CACHE STRING "Compile the project with the WebApp GUI")
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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Glog REQUIRED)
find_package(util_caching REQUIRED)
find_package(Yaml-cpp REQUIRED)


###########
## Build ##
###########

# Declare a cpp library
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)
target_link_libraries(${PROJECT_NAME} INTERFACE
  glog::glog
  util_caching
  ${YAML_CPP_LIBRARIES}
)


###################
## Cmake Package ##
###################

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)


#############
##   GUI   ##
#############

if(BUILD_GUI)
  message("Building with WebApp GUI")
  add_subdirectory(gui)
endif()


#############
## Testing ##
#############

# Testing only available if this is the main project
# Emergency override ARBITRATION_GRAPHS_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR ARBITRATION_GRAPHS_CMAKE_BUILD_TESTING)
   AND BUILD_TESTS)
  add_subdirectory(test)
endif()


#############
## Install ##
#############

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        COMPONENT core
        LIBRARY DESTINATION lib COMPONENT Runtime
        ARCHIVE DESTINATION lib COMPONENT Development
        RUNTIME DESTINATION bin COMPONENT Runtime
        PUBLIC_HEADER DESTINATION include COMPONENT Development
        BUNDLE DESTINATION bin COMPONENT Runtime)
install(EXPORT ${PROJECT_NAME}Targets
        DESTINATION lib/cmake/${PROJECT_NAME}
        COMPONENT core)
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME}
        COMPONENT core)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION include
        COMPONENT core
        PATTERN "gui" EXCLUDE)


###############
## Packaging ##
###############

include(cpack_config)
include(CPack)

message(STATUS "Components to pack: ${CPACK_COMPONENTS_ALL}")