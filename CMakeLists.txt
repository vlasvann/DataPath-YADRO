cmake_minimum_required(VERSION 3.16)

project(TapeSort LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TAPESORT_BUILD_TOOLS "Build helper tools" ON)
option(TAPESORT_BUILD_TESTS "Build unit tests" OFF)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(nlohmann_json)

add_library(tape_sort_core STATIC
    CORE/file_tape.cpp
    CORE/tape_sorter.cpp
    CORE/run_reader.cpp
    CORE/run_writer.cpp
)

target_include_directories(tape_sort_core PUBLIC
    CORE
    COMMON
)

target_link_libraries(tape_sort_core PUBLIC
    nlohmann_json::nlohmann_json
)

add_executable(tape_sort
    APP/main.cpp
    APP/app.cpp
    APP/config_reader.cpp
)

target_include_directories(tape_sort PRIVATE
    APP
)

target_link_libraries(tape_sort PRIVATE
    tape_sort_core
)

if (TAPESORT_BUILD_TOOLS)
    add_executable(text_to_binary
        TOOLS/text_to_binary.cpp
    )

    add_executable(binary_to_text
        TOOLS/binary_to_text.cpp
    )
endif()

if (TAPESORT_BUILD_TESTS)
    enable_testing()

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_MakeAvailable(googletest)

    add_executable(tape_sort_tests
        tests/file_tape_tests.cpp
        tests/tape_sorter_tests.cpp
        tests/config_reader_tests.cpp

        APP/config_reader.cpp
    )

    target_include_directories(tape_sort_tests PRIVATE
        APP
        CORE
        COMMON
    )

    target_link_libraries(tape_sort_tests PRIVATE
        tape_sort_core
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(tape_sort_tests)
endif()