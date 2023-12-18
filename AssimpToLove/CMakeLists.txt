project(AssimpToLove)
if (WIN32)
    set(ASSIMP_ROOT $ENV{LOCALAPPDATA}/assimp)
    # Make sure to set your own directories
    set(ASSIMP_INCLUDE_DIRS
        "${ASSIMP_ROOT}\\include"
        "${ASSIMP_ROOT}\\build\\include\\"
    )
    find_library(ASSIMP_LIBRARIES "assimp-vc142-mt" PATHS "${ASSIMP_ROOT}\\build\\lib\\Release")
elseif (LINUX)
    find_package(assimp)
endif()

if (ASSIMP_LIBRARIES EQUAL "NOTFOUND") # Gracefully avoid compiling if libraries are not found
else()
    add_library(assimp_to_love SHARED
        lib.cpp
    )
    set_target_properties(assimp_to_love PROPERTIES PREFIX "")
    target_include_directories(assimp_to_love PRIVATE ${LUA_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIRS})
    target_link_libraries(assimp_to_love PRIVATE ${LUA_LIBRARY} ${ASSIMP_LIBRARIES})
endif()