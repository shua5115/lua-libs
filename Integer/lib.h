#ifndef LUALIBINTEGER
#define LUALIBINTEGER

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

typedef ptrdiff_t LuaLibInteger;

extern "C" SHARED_EXPORT LuaLibInteger check_integer(lua_State *L, int idx);
extern "C" SHARED_EXPORT LuaLibInteger opt_integer(lua_State *L, int idx, LuaLibInteger def);
extern "C" SHARED_EXPORT int push_integer(lua_State *L, LuaLibInteger val);
extern "C" SHARED_EXPORT int luaopen_integer(lua_State *L);

#endif // mylualib