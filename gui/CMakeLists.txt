# Call this from the top-level CMakeLists.txt

cmake_minimum_required(VERSION 3.22)


###################
## Find packages ##
###################

find_package(Crow REQUIRED)


###########
## Build ##
###########

# Declare a cpp library
add_library(${PROJECT_NAME}_gui SHARED
  src/websocket_server.cpp
  include/arbitration_graphs/gui/websocket_server.hpp
)
target_include_directories(${PROJECT_NAME}_gui PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/gui/include/${PROJECT_NAME}>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)
target_link_libraries(${PROJECT_NAME}_gui PUBLIC
  Crow::Crow
)


#############
## Install ##
#############

set(INSTALL_TARGETS ${INSTALL_TARGETS} ${PROJECT_NAME}_gui PARENT_SCOPE)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)


#############
## Testing ##
#############

# Testing only available if this is the main project
# Emergency override ARBITRATION_GRAPHS_GUI_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR ARBITRATION_GRAPHS_GUI_CMAKE_BUILD_TESTING)
   AND BUILD_TESTS)
  add_subdirectory(test)
endif()