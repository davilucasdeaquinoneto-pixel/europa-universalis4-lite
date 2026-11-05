cmake_minimum_required(VERSION 3.20)
project(EuropaUniversalisLite VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(europa-lite
    src/main.cpp
    src/World.cpp
    src/Game.cpp
    src/MapPreview.cpp
    src/ProvinceRaster.cpp
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

    add_executable(europa-lite-map-tests
        tests/TestMapPreview.cpp
        src/World.cpp
        src/MapPreview.cpp
    )
    target_include_directories(europa-lite-map-tests PRIVATE include)
    target_compile_definitions(europa-lite-map-tests PRIVATE
        EULITE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    )
    add_test(NAME map-preview-tests COMMAND europa-lite-map-tests)

    add_executable(europa-lite-geography-tests
        tests/TestGeography.cpp
        src/World.cpp
    )
    target_include_directories(europa-lite-geography-tests PRIVATE include)
    target_compile_definitions(europa-lite-geography-tests PRIVATE
        EULITE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    )
    add_test(NAME geography-tests COMMAND europa-lite-geography-tests)

    add_executable(europa-lite-raster-tests
        tests/TestProvinceRaster.cpp
        src/ProvinceRaster.cpp
    )
    target_include_directories(europa-lite-raster-tests PRIVATE include)
    add_test(NAME province-raster-tests COMMAND europa-lite-raster-tests)
endif()
