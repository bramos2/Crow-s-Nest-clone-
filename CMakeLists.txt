cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS)
set(CMAKE_ENABLE_EXPORTS ON)

project(crows_nest CXX)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ext)
target_link_libraries(${PROJECT_NAME} PRIVATE
  lava::app
)

# This must be defined after adding the ext/ subdirectory.
add_compile_definitions(fn=auto)

# Control the debug camera flight controls.
option(BUILD_CONTROLS ON)
set(CAMERA_HPP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/debug_camera_control/debug_camera_control.hpp)

if (BUILD_CONTROLS)
  if (UNIX)
    set(VHEADER_EXE ${CMAKE_CURRENT_SOURCE_DIR}/debug_camera_control/debug_camera_control)
  else()
    set(VHEADER_EXE ${CMAKE_CURRENT_SOURCE_DIR}/debug_camera_control/debug_camera_control.exe)
  endif()

  set(DEBUG_CONTROLS "-u w -d s -l a -r d" CACHE STRING "List of controls for the debug camera.")

  add_custom_command(
    OUTPUT ${CAMERA_HPP_FILE}
    COMMAND ${VHEADER_EXE} ${DEBUG_CONTROLS}
    DEPENDS ${VHEADER_EXE}
    COMMENT "Building camera config."
    VERBATIM
  )
  add_custom_target(debug_camera_header
    ALL
    DEPENDS ${CAMERA_HPP_FILE}
  )
  add_dependencies(${CMAKE_PROJECT_NAME} debug_camera_header)

  message("Generated the debug camera controls with command:")
  message("${VHEADER_EXE} ${DEBUG_CONTROLS}")
else()
  set(CAMERA_HPP_CONTENT "#include <liblava/lava.hpp>
lava::keys_ref debug_key_up = {}\;
lava::keys_ref debug_key_down = {}\;
lava::keys_ref debug_key_left = {}\;
lava::keys_ref debug_key_right = {}\;")
  file(WRITE ${CAMERA_HPP_FILE} ${CAMERA_HPP_CONTENT} )
endif()

target_sources(
  ${PROJECT_NAME} PRIVATE
  src/main.cpp
)
