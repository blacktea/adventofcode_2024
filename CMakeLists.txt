cmake_minimum_required(VERSION 3.20)
project(advent2024 LANGUAGES CXX)

option(DAY7_CONSTEXPR "Enables computing solution at compile-time" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic -O2)

function(add_challenge name)
    add_executable(${name} ${CMAKE_CURRENT_SOURCE_DIR}/src/${name}.cpp)
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    set_property(TARGET ${name} PROPERTY CXX_STANDARD 20)
endfunction(add_challenge)

add_challenge(day1)
add_challenge(day1_part2)
add_challenge(day2)
add_challenge(day3)
add_challenge(day4)
add_challenge(day5)
add_challenge(day6)

### Day 7
add_challenge(day7)
target_compile_options(day7 PRIVATE -fconstexpr-ops-limit=3355443200)
if(DAY7_CONSTEXPR)
    target_compile_options(day7 PRIVATE -DDAY7_CONSTEXPR)
endif()

# Read the content of the input file
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/data/2024/day7.txt FILE_CONTENT)

# Quote the content
set(FILE_CONTENT "R\"(${FILE_CONTENT})\"")

message("${CMAKE_CURRENT_BINARY_DIR}")
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/data/2024/day7_constexpr.txt ${FILE_CONTENT})


add_challenge(day8)
# add_challenge(day9)
add_challenge(day10)
add_challenge(day11)
add_challenge(day12)
add_challenge(day13)
add_challenge(day14)
# add_challenge(day15)
# add_challenge(day16)
# add_challenge(day17)
# add_challenge(day18)
add_challenge(day19)
add_challenge(day24)
add_challenge(day25)

add_custom_command(TARGET day1
    COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_SOURCE_DIR}/data" "${CMAKE_CURRENT_BINARY_DIR}/data"
)

find_package(CURL)
if(CURL_FOUND)
    add_executable(input_downloader src/utils/input_downloader.cpp)
    target_link_libraries(input_downloader PRIVATE ${CURL_LIBRARIES})
endif()