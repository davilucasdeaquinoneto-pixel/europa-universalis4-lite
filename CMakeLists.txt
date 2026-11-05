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

file(COPY data DESTINATION ${CMAKE_BINARY_DIR})

include(CTest)
if(BUILD_TESTING)
    add_executable(europa-lite-tests
        tests/TestCore.cpp
        src/World.cpp
    )
    target_include_directories(europa-lite-tests PRIVATE include)
    target_compile_definitions(europa-lite-tests PRIVATE
        EULITE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    )
    add_test(NAME core-tests COMMAND europa-lite-tests)

    add_executable(europa-lite-validation-tests
        tests/TestWorldValidation.cpp
        src/World.cpp
    )
    target_include_directories(europa-lite-validation-tests PRIVATE include)
    add_test(NAME world-validation-tests COMMAND europa-lite-validation-tests)
endif()
