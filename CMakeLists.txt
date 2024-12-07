cmake_minimum_required(VERSION 3.20)
project(advent2024 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED 20)
add_compile_options(-Wall -Wextra -Wpedantic -O2)

function(add_challenge name)
    add_executable(${name} ${CMAKE_CURRENT_SOURCE_DIR}/src/${name}.cpp)
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
endfunction(add_challenge)

add_challenge(day1)
add_challenge(day1_part2)
add_challenge(day2_part1)
add_challenge(day3)
add_challenge(day4)
add_challenge(day5)
add_challenge(day7)

add_custom_command(TARGET day1
    COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_SOURCE_DIR}/data" "./data"
)