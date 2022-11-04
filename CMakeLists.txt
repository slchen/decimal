cmake_minimum_required (VERSION 3.13)

project(decimal LANGUAGES CXX)

option(DECIMAL_TESTS "Build the tests" ON)

add_library(libdecimal INTERFACE)
add_library(Decimal::Headers ALIAS libdecimal)
add_library(Decimal::Decimal ALIAS libdecimal)

target_include_directories(libdecimal INTERFACE "${CMAKE_CURRENT_LIST_DIR}")

set_target_properties(libdecimal PROPERTIES CXX_STANDARD 17)

if(DECIMAL_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()


