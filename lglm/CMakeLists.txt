cmake_minimum_required(VERSION 3.10)

project(LuaGLM)

add_library(lglm SHARED
    lib.cpp
    glmath.cpp
    vec.cpp
    mat.cpp
)

set_target_properties(lglm PROPERTIES PREFIX "")
target_include_directories(lglm PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(lglm PRIVATE ${LUA_LIBRARY})