cmake_minimum_required(VERSION 3.20)
project(advent2024 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED 20)
add_compile_options(-Wall -Wextra -Wpedantic -O2)

add_executable(day1 day1.cpp)


add_custom_command(TARGET day1
    COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_SOURCE_DIR}/data" "./data"
)