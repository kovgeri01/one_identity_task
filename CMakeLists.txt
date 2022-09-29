cmake_minimum_required(VERSION 3.10)

set(supported_languages C)
set(ver 1.0)

project(
    OneIdentityProjectTask
    VERSION ${ver}
    LANGUAGES ${supported_languages}
)

include_directories(include)

file(GLOB SOURCE "src/*.cc")
list(REMOVE_ITEM SOURCE "src/server.c")
list(REMOVE_ITEM SOURCE "src/client.c")
add_executable(server src/server.c)
add_executable(client src/client.c)

add_subdirectory(tests)