cmake_minimum_required(VERSION 3.12)

project(CBEAR VERSION 1.2.3.4 LANGUAGES CXX)

option(ENABLE_WARNINGS_SETTINGS "Allow target_set_warnings to add flags and defines.
                                 Set this to OFF if you want to provide your own warning parameters." OFF)
option(ENABLE_LTO "Enable link time optimization" ON)
option(ENABLE_DOCTESTS "Include tests in the library. Setting this to OFF will remove all doctest related code.
                        Tests in tests/*.cpp will still be enabled." OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(ConfigSafeGuards)
include(Colors)
include(CTest)
include(Doctest)
include(Documentation)
include(LTO)
include(Misc)
include(Warnings)

find_lto(CXX)


set(SOURCES          # All .cpp files in src/
        src/port_manager.cpp src/packet_manager.cpp src/bear_sdk.cpp)
set(TESTFILES        # All .cpp files in tests/
        tests/main.cpp
        )
set(LIBRARY_NAME cbear)



add_library(${LIBRARY_NAME} OBJECT ${SOURCES})

target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${LIBRARY_NAME} PUBLIC doctest)

target_set_warnings(${LIBRARY_NAME} ENABLE ALL AS_ERROR ALL DISABLE Annoying)
target_compile_options(${LIBRARY_NAME} PRIVATE -O3 )  # For setting manually.

add_executable(sample_main app/sample_main/sample_main.cpp)
target_link_libraries(sample_main PRIVATE ${LIBRARY_NAME})
target_set_warnings(sample_main ENABLE ALL AS_ERROR ALL DISABLE Annoying)
target_enable_lto(sample_main optimized)

set_target_properties(
        ${LIBRARY_NAME} sample_main
        PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

add_subdirectory(tests)
