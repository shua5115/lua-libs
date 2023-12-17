cmake_minimum_required(VERSION 3.10)

project(Integer)

add_library(integer SHARED
    lib.cpp
)

set_target_properties(integer PROPERTIES PREFIX "")
target_include_directories(integer PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(integer PRIVATE ${LUA_LIBRARY})