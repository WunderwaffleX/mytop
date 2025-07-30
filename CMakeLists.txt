cmake_minimum_required(VERSION 3.14)
project(mytop)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1  
)
FetchContent_MakeAvailable(fmt)

include_directories(include /usr/include/nvidia)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(mytop ${SOURCES})

target_link_libraries(mytop PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
    fmt::fmt
    nvidia-ml)
