cmake_minimum_required(VERSION 3.10)

set(supported_languages C CXX)
set(ver 1.0)

project(
    OneIdentityProjectTask
    VERSION ${ver}
    LANGUAGES ${supported_languages}
)

include_directories(include)

include(CheckIncludeFiles unistd.h)

add_definitions(-D_POSIX_C_SOURCE=2 -D_GNU_SOURCE=1)

find_package(Threads REQUIRED)

file(GLOB SOURCE "src/*.c")
list(REMOVE_ITEM SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/main_server.c)
list(REMOVE_ITEM SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/main_client.c)

add_library(OneIdentityTask ${SOURCE})

add_executable(server src/main_server.c )
add_executable(client src/main_client.c )

target_link_libraries (server PRIVATE OneIdentityTask)
target_link_libraries (client PRIVATE OneIdentityTask)

enable_testing()
find_package(GTest REQUIRED)

file(GLOB TEST_SOURCE "tests/src/*.cc")
add_executable(unit_tests ${TEST_SOURCE})

target_link_libraries(unit_tests GTest::gtest GTest::gtest_main OneIdentityTask)
gtest_discover_tests(unit_tests)