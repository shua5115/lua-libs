project(AssimpToLove)

find_package(assimp REQUIRED)

add_library(assimp_to_love SHARED
    lib.cpp
)

set_target_properties(assimp_to_love PROPERTIES PREFIX "")
target_include_directories(assimp_to_love PRIVATE ${LUA_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(assimp_to_love PRIVATE ${LUA_LIBRARY} ${ASSIMP_LIBRARIES})