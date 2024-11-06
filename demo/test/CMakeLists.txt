###################
## Find packages ##
###################

find_package(GTest)


###########
## Build ##
###########

if(GTEST_FOUND)
  file(GLOB_RECURSE _tests CONFIGURE_DEPENDS "*.cpp" "*.cc")

  foreach(_test ${_tests})
    get_filename_component(_test_name ${_test} NAME_WE)
    # make sure we add only one -test to the target
    string(REGEX REPLACE "-test" "" TEST_TARGET_NAME ${_test_name})
    set(TEST_TARGET_NAME ${PROJECT_NAME}-gtest-${TEST_TARGET_NAME})

    message(STATUS
      "Adding gtest unittest \"${TEST_TARGET_NAME}\" with working dir ${PROJECT_SOURCE_DIR}/${TEST_FOLDER} \n _test: ${_test}"
    )

    add_executable(${TEST_TARGET_NAME} ${_test})

    target_include_directories(${TEST_TARGET_NAME} PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
        ${SDL2_INCLUDE_DIR}
    )

    target_link_libraries(${TEST_TARGET_NAME} PUBLIC
      ${GTEST_BOTH_LIBRARIES}
      pthread
      ${PROJECT_NAME}
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
