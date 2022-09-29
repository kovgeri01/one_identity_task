cmake_minimum_required(VERSION 3.10)

set(supported_languages C)

project(
    OneIdentityProjectTaskTests
    VERSION ${ver}
    LANGUAGES ${supported_languages}
    )
    
include_directories(include)

enable_testing()
find_package(GTest REQUIRED)

file(GLOB TEST "test/*.cc")
list(REMOVE_ITEM SOURCE "test/main.c")
add_executable(tests src/main.c)

target_link_libraries(tests GTest::gtest GTest::gtest_main)
gtest_discover_tests(tests)
