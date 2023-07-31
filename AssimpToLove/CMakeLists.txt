project(AssimpToLove)
if (WIN32)
    # Make sure to set your own directories
    set(ASSIMP_INCLUDE_DIRS
        "C:/Program Files/assimp/assimp-5.2.5/include"
        "C:/Program Files/assimp/assimp-5.2.5/build/include/"
    )
    find_library(ASSIMP_LIBRARIES assimp-vc142-mt "C:/Program Files/assimp/assimp-5.2.5/build/lib/Release" REQUIRED)
elseif (LINUX)
    find_package(assimp REQUIRED)
endif()

add_library(assimp_to_love SHARED
    lib.cpp
)

set_target_properties(assimp_to_love PROPERTIES PREFIX "")
target_include_directories(assimp_to_love PRIVATE ${LUA_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(assimp_to_love PRIVATE ${LUA_LIBRARY} ${ASSIMP_LIBRARIES})