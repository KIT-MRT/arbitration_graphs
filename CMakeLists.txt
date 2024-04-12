cmake_minimum_required(VERSION 3.5.1)
project(arbitration_graphs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


###################
## Find packages ##
###################

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Glog)

find_package(util_caching REQUIRED)

find_package(Yaml-cpp REQUIRED)


############################
## Read source code files ##
############################
file(GLOB_RECURSE PROJECT_HEADER_FILES_INC "include/${PROJECT_NAME}/*.h" "include/${PROJECT_NAME}/*.hpp")


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
    VERSION 0.1
    COMPATIBILITY AnyNewerVersion
)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)


#############
## Install ##
#############


install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)
install(EXPORT ${PROJECT_NAME}Targets DESTINATION lib/cmake/${PROJECT_NAME})
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)


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

      target_link_libraries(${TEST_TARGET_NAME} PUBLIC
          ${GTEST_BOTH_LIBRARIES} pthread
          ${PROJECT_NAME}
      )
    endforeach()
  endif()
endif()
