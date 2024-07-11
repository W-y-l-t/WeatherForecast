cmake_minimum_required(VERSION 3.12)
project(
        labwork7
        VERSION 1.0
        LANGUAGES CXX
)

set(CMAKE_CXX_FLAGS_INIT "-static" "--clean-first" "Wall" "-O2")
set(CMAKE_CXX_STANDARD 20)


if(WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()


include(FetchContent)
FetchContent_Declare(cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 3b15fa8
)
FetchContent_MakeAvailable(cpr)

include(CPM.cmake)
CPMAddPackage("gh:CodeFinder2/rapidxml@1.13")


FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG v5.0.0
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
    FetchContent_Populate(ftxui)
    add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.11.2
)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
        GIT_TAG v3.11.
)

FetchContent_MakeAvailable(json)

add_subdirectory(lib)
add_subdirectory(bin)
link_directories(lib)
