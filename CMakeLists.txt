cmake_minimum_required(VERSION 3.21)

project(ByteConverterTest LANGUAGES CXX)

# C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Tests
enable_testing()

# Qt package
find_package(Qt6 REQUIRED COMPONENTS Core)
    # Qt setup
qt_standard_project_setup()

# FetchContent
include(FetchContent)

# GTest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

# ==================================================
# Core
# ==================================================

add_library(ConverterCore
    src/core/Converter.cpp
    src/core/workers/ConverterWorker.cpp
    src/core/utils/ByteDecoder.cpp
    src/core/io/QIODevice/QIODeviceSource.cpp
    src/core/io/QIODevice/QIODeviceSink.cpp
)

target_include_directories(ConverterCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ConverterCore
    PUBLIC
        Qt6::Core
)

# ==================================================
# App
# ==================================================

add_executable(ByteConverterTest
    src/app/main.cpp
)

target_link_libraries(ByteConverterTest
    PRIVATE
        ConverterCore
)

# ==================================================
# Test
# ==================================================


# add_executable(ByteConverterTest_test
#     tests/test_main.cpp
#     tests/ByteDecoderTests.cpp
#     tests/ConverterWorkerTests.cpp
# )

# target_include_directories(ByteConverterTest_test
#     PRIVATE
#         ${CMAKE_CURRENT_SOURCE_DIR}/src
#         ${CMAKE_CURRENT_SOURCE_DIR}/tests
# )

# target_link_libraries(ByteConverterTest_test
#     PRIVATE
#         ConverterCore
#         GTest::gtest
#         GTest::gmock
#         GTest::gtest_main
# )

# # авто-дискавер тестов
# include(GoogleTest)
# gtest_discover_tests(ByteConverterTest_test)