# Call this from the top-level CMakeLists.txt

cmake_minimum_required(VERSION 3.22)


###############
## GUI setup ##
###############

# Pass install data directory to the Crow web server,
# as the Web App files will be located there

include(GNUInstallDirs)

set(INSTALL_APP_DIRECTORY ${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME})
set(LOCAL_APP_DIRECTORY ${CMAKE_SOURCE_DIR}/gui/app/${PROJECT_NAME})
message(STATUS "Install directory for WebApp GUI: INSTALL_APP_DIRECTORY=${INSTALL_APP_DIRECTORY}")
message(STATUS "Local directory for WebApp GUI: LOCAL_APP_DIRECTORY=${LOCAL_APP_DIRECTORY}")
configure_file (
  "${PROJECT_SOURCE_DIR}/gui/include/arbitration_graphs/gui/crow_config.hpp.in"
  "${PROJECT_BINARY_DIR}/include/arbitration_graphs/gui/crow_config.hpp"
)


###################
## Find packages ##
###################

find_package(Crow REQUIRED)


###########
## Build ##
###########

# Declare a cpp library
add_library(${PROJECT_NAME}_gui INTERFACE
  include/arbitration_graphs/gui/web_server.hpp
)
target_include_directories(${PROJECT_NAME}_gui INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/gui/include/${PROJECT_NAME}>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}/gui>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)
target_link_libraries(${PROJECT_NAME}_gui INTERFACE
  Crow::Crow
)


#############
## Install ##
#############

install(TARGETS ${PROJECT_NAME}_gui
        EXPORT ${PROJECT_NAME}_guiTargets
        COMPONENT gui
        LIBRARY DESTINATION lib COMPONENT Runtime
        ARCHIVE DESTINATION lib COMPONENT Development
        RUNTIME DESTINATION bin COMPONENT Runtime
        PUBLIC_HEADER DESTINATION include COMPONENT Development
        BUNDLE DESTINATION bin COMPONENT Runtime)
install(DIRECTORY app/
        TYPE DATA
        COMPONENT gui)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include
        COMPONENT gui
        PATTERN "*.in" EXCLUDE)
install(DIRECTORY ${CMAKE_BINARY_DIR}/include/
        DESTINATION include
        COMPONENT gui)


#############
## Testing ##
#############

# Testing only available if this is the main project
# Emergency override ARBITRATION_GRAPHS_GUI_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR ARBITRATION_GRAPHS_GUI_CMAKE_BUILD_TESTING)
   AND BUILD_TESTS)
  add_subdirectory(test)
endif()