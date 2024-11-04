# Call this from the top-level CMakeLists.txt

cmake_minimum_required(VERSION 3.22)


################
## Crow setup ##
################

# Pass install data directory to the Crow web server,
# as the Web App files will be located there

include(GNUInstallDirs)

set(CROW_STATIC_DIRECTORY ${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME})
add_compile_definitions(CROW_STATIC_DIRECTORY="${CROW_STATIC_DIRECTORY}")
message("Install directory for WebApp GUI: CROW_STATIC_DIRECTORY=${CROW_STATIC_DIRECTORY}")


###################
## Find packages ##
###################

find_package(Crow REQUIRED)


###########
## Build ##
###########

# Declare a cpp library
add_library(${PROJECT_NAME}_gui SHARED
  src/web_server.cpp
  include/arbitration_graphs/gui/web_server.hpp
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
install(DIRECTORY htmlroot/ DESTINATION ${CROW_STATIC_DIRECTORY})
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