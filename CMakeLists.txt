cmake_minimum_required(VERSION 3.16)

project(TapeSort LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TAPESORT_BUILD_TOOLS "Build helper tools" ON)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(nlohmann_json)

add_executable(tape_sort
    APP/main.cpp
    APP/app.cpp
    APP/config_reader.cpp

    CORE/file_tape.cpp
    CORE/tape_sorter.cpp
    CORE/run_reader.cpp
    CORE/run_writer.cpp
)

target_include_directories(tape_sort PRIVATE
    APP
    CORE
    COMMON
)

target_link_libraries(tape_sort PRIVATE
    nlohmann_json::nlohmann_json
)

if (TAPESORT_BUILD_TOOLS)
    add_executable(text_to_binary
        TOOLS/text_to_binary.cpp
    )

    add_executable(binary_to_text
        TOOLS/binary_to_text.cpp
    )
endif()