#include "lib.h"
#include <iostream>

int alloc_table(lua_State *L) {
    int arr = luaL_optinteger(L, 1, 0);
    int rec = luaL_optinteger(L, 2, 0);
    lua_createtable(L, arr, rec);
    return 1;
}

static const luaL_reg funcs[] = {
    {"createtable", alloc_table},
    {NULL, NULL}
};

extern "C" SHARED_EXPORT int luaopen_mylualib(lua_State *L) {
    luaL_register(L, "mylualib", funcs);
    return 1;
}