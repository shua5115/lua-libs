cmake_minimum_required(VERSION 3.10)

project(MyLuaLib)

add_library(mylualib SHARED
    lib.cpp
)

set_target_properties(mylualib PROPERTIES PREFIX "")
target_include_directories(mylualib PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(mylualib PRIVATE ${LUA_LIBRARY})