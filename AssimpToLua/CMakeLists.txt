project(AssimpToLua)

find_package(assimp REQUIRED)

add_library(assimp_to_lua SHARED
    lib.cpp
)

set_target_properties(assimp_to_lua PROPERTIES PREFIX "")
target_include_directories(assimp_to_lua PRIVATE ${LUA_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(assimp_to_lua PRIVATE ${LUA_LIBRARY} ${ASSIMP_LIBRARIES})