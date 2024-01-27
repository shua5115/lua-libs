#ifndef LGLM_H
#define LGLM_H

#if defined(_MSC_VER)
#	define SHARED_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
#	define SHARED_EXPORT __attribute__((visibility("default")))
#else
#	define SHARED_EXPORT
#endif

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

extern "C" SHARED_EXPORT int luaopen_luaglm(lua_State *L);

#endif // lglm_h