cmake_minimum_required(VERSION 3.15)

project(ArtillerySimulator VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDART 11)
set(CMAKE_C_STANDART_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_executable(artillery
  src/main.c
  src/simulation.c
)

target_include_directories(artillery PRIVATE include)

if(NOT MSVC)
  target_link_libraries(artillery PRIVATE m)
endif()

if (MSVC)
  target_compile_options(artillery PRIVATE /W4)
else()
  target_compile_options(artillery PRIVATE -Wall -Wextra -Wpedantic)
endif()

