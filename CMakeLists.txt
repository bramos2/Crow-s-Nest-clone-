cmake_minimum_required(VERSION 3.20)
project(crows_nest CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ENABLE_EXPORTS ON)
set(CMAKE_CONFIGURATION_TYPES Debug;Release)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/liblava ${CMAKE_CURRENT_BINARY_DIR}/liblava)

add_compile_definitions(fn=auto)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

target_sources(
  ${PROJECT_NAME} PRIVATE
  src/main.cpp
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message("Generated DEBUG Files!")
else()
  message("Generated RELEASE Files!")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE lava::app)
