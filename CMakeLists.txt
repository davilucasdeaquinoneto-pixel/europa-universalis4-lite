cmake_minimum_required(VERSION 3.20)
project(EuropaUniversalisLite VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(europa-lite
    src/main.cpp
    src/World.cpp
    src/Game.cpp
)

target_include_directories(europa-lite PRIVATE include)

if(MSVC)
    target_compile_options(europa-lite PRIVATE /W4 /permissive-)
else()
    target_compile_options(europa-lite PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Keep runtime data beside the executable in the build directory.
file(COPY data DESTINATION ${CMAKE_BINARY_DIR})
