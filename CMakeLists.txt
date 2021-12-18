cmake_minimum_required(VERSION 3.10)

# set the project name
project(crows_nest)

# by default CMake selects "ALL_BUILD" as the startup project
# this causes that step to be skipped
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
	PROPERTY VS_STARTUP_PROJECT crows_nest)

# this is necessary because the starting function is WinMain() and not main()
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup")

# add the executable
add_executable(crows_nest src/cpp/main.cpp)

# soloud configurations
set(SOLOUD_BACKEND_NULL OFF CACHE BOOL "" FORCE)
set(SOLOUD_BACKEND_SDL2 OFF CACHE BOOL "" FORCE)
set(SOLOUD_BACKEND_XAUDIO2 ON CACHE BOOL "" FORCE)

# basically used just by soloud
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ext)
if(DEBUG_STACK_TRACE)
  add_backward(${CMAKE_PROJECT_NAME})
endif()

# i don't know what this does but soloud doesn't work without it
target_link_libraries(
  ${PROJECT_NAME} PRIVATE
  soloud
)

# includes for imgui
target_include_directories(
  ${PROJECT_NAME} PRIVATE
  ext/imgui/backends
  ext/imgui
)

# i have no idea if this is necessary or not but i dont really care
if(WIN32)
  add_compile_definitions(WIN32)
endif()

set(HPP_FILES
  src/hpp/audio.hpp
  src/hpp/behavior_tree.hpp
  src/hpp/blob.hpp
  src/hpp/bvh.hpp
  src/hpp/credits.hpp
  src/hpp/d3d11_renderer_impl.hpp
  src/hpp/DDSTextureLoader.hpp
  src/hpp/enemy_behaviors.hpp
  src/hpp/debug_renderer.hpp
  src/hpp/entities.hpp
  src/hpp/frustum_culling.hpp
  src/hpp/game_manager.hpp
  src/hpp/interactible.hpp
  src/hpp/map.hpp
  src/hpp/math_types.hpp
  src/hpp/message.hpp
  src/hpp/minimap.hpp
  src/hpp/mbvh.hpp
  src/hpp/mesh.hpp
  src/hpp/player_behavior.hpp
  src/hpp/pools.hpp
  src/hpp/renderer.hpp
  src/hpp/search_theta.hpp
  src/hpp/super_matrix.hpp
  src/hpp/tile.hpp
  src/hpp/view.hpp
  src/hpp/XTime.hpp
)

set(CPP_FILES
  src/cpp/audio.cpp
  src/cpp/behavior_tree.cpp
  src/cpp/blob.cpp
  src/cpp/credits.cpp
  src/cpp/d3d11_renderer_impl.cpp
  src/cpp/DDSTextureLoader.cpp
  src/cpp/debug_renderer.cpp
  src/cpp/enemy_behaviors.cpp
  src/cpp/entities.cpp
  src/cpp/game_manager.cpp
  src/cpp/gui.cpp
  src/cpp/interactible.cpp
  src/cpp/levels.cpp
  src/cpp/level_helper.cpp
  src/cpp/main.cpp
  src/cpp/map.cpp
  src/cpp/mesh.cpp
  src/cpp/message.cpp
  src/cpp/minimap.cpp
  src/cpp/player_behavior.cpp
  src/cpp/renderer.cpp
  src/cpp/search_theta.cpp
  src/cpp/super_matrix.cpp
  src/cpp/tile.cpp
  src/cpp/tutorial.cpp
  src/cpp/view.cpp
  src/cpp/XTime.cpp
)

# i dunno a more elegant way to load imgui, but this seems to work.
set(IMGUI_FILES
  ext/imgui/imconfig.h
  ext/imgui/imgui.cpp
  ext/imgui/imgui.h
  ext/imgui/imgui_demo.cpp
  ext/imgui/imgui_draw.cpp
  ext/imgui/imgui_internal.h
  ext/imgui/imgui_tables.cpp
  ext/imgui/imgui_widgets.cpp
  ext/imgui/backends/imgui_impl_dx11.cpp
  ext/imgui/backends/imgui_impl_dx11.h
)

set(VERTEX_SHADERS 
  src/shader/vs_billboard.hlsl
  src/shader/vs_coloredvertex.hlsl
  src/shader/vs_cube.hlsl
  src/shader/vs_meshvertex.hlsl
  src/shader/vs_smeshvertex.hlsl
)

set(PIXEL_SHADERS
  src/shader/ps_billboard.hlsl
  src/shader/ps_coloredvertex.hlsl
  src/shader/ps_cube.hlsl
  src/shader/ps_meshvertex.hlsl
  src/shader/ps_smeshvertex.hlsl
  src/shader/ps_simple.hlsl
)

# idek what those other files are so i crammed them all in here
set(OTHER_SHADERS
  src/shader/mcb.hlsli
  src/shader/mvp.hlsli
  src/shader/scb.hlsli
  src/shader/bbb.hlsli
)

target_sources(
  ${PROJECT_NAME} PRIVATE
  ${HPP_FILES}
  ${CPP_FILES}
  ${IMGUI_FILES}
  ${OTHER_SHADERS}
  ${VERTEX_SHADERS}
  ${PIXEL_SHADERS}
)

# puts the imgui files in their own VS folder so that they don't clutter up the solution
source_group("imgui" FILES ${IMGUI_FILES})

set_source_files_properties( ${VERTEX_SHADERS} PROPERTIES 
	VS_SHADER_TYPE Vertex VS_SHADER_MODEL 5.0 VS_SHADER_ENTRYPOINT main
	VS_SHADER_OBJECT_FILE_NAME "$(OutDir)../%(Filename).cso"
	#VS_SHADER_OUTPUT_HEADER_FILE "%(RelativeDir)%(Filename).h"
	#VS_SHADER_VARIABLE_NAME "%(Filename)"
	#VS_SHADER_ENABLE_DEBUG $<$<CONFIG:Debug>:true>
)

set_source_files_properties( ${PIXEL_SHADERS} PROPERTIES 
	VS_SHADER_TYPE Pixel VS_SHADER_MODEL 5.0 VS_SHADER_ENTRYPOINT main 
	VS_SHADER_OBJECT_FILE_NAME "$(OutDir)../%(Filename).cso"
	#VS_SHADER_OUTPUT_HEADER_FILE "%(RelativeDir)%(Filename).h"
	#VS_SHADER_VARIABLE_NAME "%(Filename)"
	#VS_SHADER_ENABLE_DEBUG $<$<CONFIG:Debug>:true>
)
# uncommenting these above lines will cause .h versions of the shaders to spawn

# this copies the res folder into the build folder when cmaking
# kind of a hacky solution (not a good idea for big projects) but should be sufficient for us
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/../res)
