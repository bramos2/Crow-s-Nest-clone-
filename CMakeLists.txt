cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS)
set(CMAKE_ENABLE_EXPORTS ON)

project(crows_nest C CXX)
add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

# These should never be needed.
set(LIBLAVA_TESTS OFF CACHE BOOL "" FORCE)
set(LIBLAVA_DEMO OFF CACHE BOOL "" FORCE)
# trying to implement SDL2 causes building to fail as SDL2 doesn't exist
set(SOLOUD_BACKEND_SDL2 OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ext)
target_link_libraries(
  ${PROJECT_NAME} PRIVATE
  lava::app
  lava-extras::fbx
  lava-extras::raytracing
  soloud
)

if(WIN32)
  add_compile_definitions(WIN32)
endif()

# Feature flags.
option(DEBUG_IMGUI "Present a Dear ImGUI window with extra information." OFF)
option(DEBUG_RAYTRACING "Provide runtime sanatization for raytracing." OFF)

# Control the debug camera flight controls.
option(BUILD_CONTROLS "Populate the controls for a debug camera." ON)
set(CAMERA_HPP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/debug_camera_control/debug_camera_control.hpp)
if(UNIX)
  set(VHEADER_EXE ${CMAKE_CURRENT_SOURCE_DIR}/debug_camera_control/debug_camera_control)
else()
  set(VHEADER_EXE ${CMAKE_CURRENT_SOURCE_DIR}/debug_camera_control/debug_camera_control.exe)
endif()
set(DEBUG_CONTROLS "-u w -d s -l a -r d" CACHE STRING "List of controls for the debug camera.")
execute_process(COMMAND ${VHEADER_EXE} ${DEBUG_CONTROLS})
message("Generated the debug camera controls at ${CAMERA_HPP_FILE} with command:")
message("${VHEADER_EXE} ${DEBUG_CONTROLS}")

target_sources(
  ${PROJECT_NAME} PRIVATE
  src/main.cpp
  src/hpp/audio.hpp
  src/cpp/audio.cpp
  src/hpp/object.hpp
  src/hpp/component.hpp
  src/hpp/geometry.hpp
  src/cpp/geometry.tpp
  src/hpp/pipeline.hpp
  src/cpp/pipeline.cpp
  src/cpp/pipeline.tpp
  src/hpp/tile.hpp
  src/cpp/tile.cpp
  src/hpp/search_theta.hpp
  src/cpp/search_theta.cpp
  src/hpp/camera.hpp
  src/cpp/camera.cpp
  src/hpp/minimap.hpp
  src/cpp/minimap.cpp
  src/hpp/device.hpp
  src/cpp/device.cpp
  src/hpp/map.hpp
  src/cpp/map.cpp
)
