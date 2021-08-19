cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS)
set(CMAKE_ENABLE_EXPORTS ON)

project(crows_nest C CXX)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)
# add_executable(${CMAKE_PROJECT_NAME})

set(LIBLAVA_TESTS OFF CACHE BOOL "" FORCE)
set(LIBLAVA_DEMO OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ext)
target_link_libraries(${PROJECT_NAME} PRIVATE
  lava::app
  lava-extras::fbx
)

# Control the debug camera flight controls.
option(BUILD_CONTROLS "Whether to populate the controls for a debug camera." ON)
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
lava::keys_ref debug_key_up = {lava::key::w}\;
lava::keys_ref debug_key_down = {lava::key::s}\;
lava::keys_ref debug_key_left = {lava::key::a}\;
lava::keys_ref debug_key_right = {lava::key::d}\;")
  file(WRITE ${CAMERA_HPP_FILE} ${CAMERA_HPP_CONTENT} )
endif()

add_compile_definitions(ROOT_PATH="${CMAKE_SOURCE_DIR}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  message("DEBUGGING!")
  add_compile_definitions(_DEBUG)
else()
  message("NOT DEBUGGING!")
endif()

target_sources(
  ${PROJECT_NAME} PRIVATE
  src/main.cpp
  src/hpp/object.hpp
  # src/cpp/object.cpp
  src/hpp/component.hpp
  # src/hpp/component_box_collision.hpp
  # src/hpp/component_player.hpp
  # src/cpp/component_player.cpp
  # src/hpp/component_sphere_collision.hpp
  src/hpp/geometry.hpp
  src/cpp/geometry.tpp
  src/hpp/pipeline.hpp
  src/cpp/pipeline.cpp
  src/cpp/pipeline.tpp
  src/hpp/tile.hpp
  src/cpp/tile.cpp
  src/hpp/camera.hpp
  src/cpp/camera.cpp
  src/hpp/search_theta.hpp
  src/cpp/search_theta.cpp
  src/hpp/minimap.hpp
)
