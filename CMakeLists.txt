cmake_minimum_required(VERSION 3.16)

project(BasicInterpreter2026 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BASIC_SOURCES
    src/basic_error.cpp
    src/expression.cpp
    src/lexer.cpp
    src/parser.cpp
    src/token.cpp
)

add_library(basic_interpreter STATIC ${BASIC_SOURCES})
target_include_directories(basic_interpreter PUBLIC include)
target_compile_options(basic_interpreter PRIVATE -Wall -Wextra -Wpedantic)

add_executable(code src/main.cpp)
target_link_libraries(code PRIVATE basic_interpreter)
target_compile_options(code PRIVATE -Wall -Wextra -Wpedantic)
