#include "lib.h"

#include <math.h>

/* get value of Integer userdata or Lua number at index, or die */
extern "C" SHARED_EXPORT LuaLibInteger check_int(lua_State* L, int idx) {
    if (lua_islightuserdata(L, idx)) {  //&& luaL_checkudata(L, idx, "integer")) {
        return (LuaLibInteger)lua_touserdata(L, idx);
    } else if (lua_isnumber(L, idx)) {
        return (LuaLibInteger)lua_tointeger(L, idx);
    } else {
        lua_pushstring(L, "Invalid operand. Expected 'integer' or 'number'");
        lua_error(L);
        return 0; /* will never get here */
    }
}

extern "C" SHARED_EXPORT LuaLibInteger opt_int(lua_State* L, int idx, LuaLibInteger def) {
    if (lua_islightuserdata(L, idx) && luaL_checkudata(L, idx, "integer")) {
        return (LuaLibInteger)lua_touserdata(L, idx);
    } else if (lua_isnumber(L, idx)) {
        return (LuaLibInteger)lua_tonumber(L, idx);
    } else {
        return def;
    }
}

extern "C" SHARED_EXPORT int push_integer(lua_State* L, LuaLibInteger val) {
    lua_pushlightuserdata(L, (void*)val);
    luaL_getmetatable(L, "integer");
    lua_setmetatable(L, -2);
    return 1;
}

static int int_new(lua_State* L) { return push_integer(L, luaL_optinteger(L, 1, 0)); }
static int int_add(lua_State* L) { return push_integer(L, check_int(L, 1) + check_int(L, 2)); }
static int int_sub(lua_State* L) { return push_integer(L, check_int(L, 1) - check_int(L, 2)); }
static int int_mul(lua_State* L) { return push_integer(L, check_int(L, 1) * check_int(L, 2)); }
static int int_div(lua_State* L) {
    LuaLibInteger denom = check_int(L, 2);
    if (denom != 0 ) {
        return push_integer(L, check_int(L, 1)/denom);
    }
    luaL_error(L, "Integer divide by zero");
    return 0;
}
static int int_mod(lua_State* L) {
    LuaLibInteger denom = check_int(L, 2);
    if (denom != 0 ) {
        return push_integer(L, check_int(L, 1)%denom);
    }
    luaL_error(L, "Integer divide by zero");
    return 0;
}
static int int_band(lua_State* L) { return push_integer(L, check_int(L, 1) & check_int(L, 2)); }
static int int_bor(lua_State* L) { return push_integer(L, check_int(L, 1) | check_int(L, 2)); }
static int int_bxor(lua_State* L) { return push_integer(L, check_int(L, 1) ^ check_int(L, 2)); }
static int int_bnot(lua_State* L) { return push_integer(L, ~check_int(L, 1)); }
static int int_shl(lua_State* L) { return push_integer(L, check_int(L, 1) << check_int(L, 2)); }
static int int_shr(lua_State* L) { return push_integer(L, check_int(L, 1) >> check_int(L, 2)); }
static int int_pow(lua_State* L) { return push_integer(L, (LuaLibInteger)(pow((lua_Number)check_int(L, 1), (lua_Number)check_int(L, 2)) + 0.5)); }
static int int_unm(lua_State* L) { return push_integer(L, -check_int(L, 1)); }
static int int_eq(lua_State* L) {
    lua_pushboolean(L, check_int(L, 1) == check_int(L, 2));
    return 1;
}
static int int_lt(lua_State* L) {
    lua_pushboolean(L, check_int(L, 1) < check_int(L, 2));
    return 1;
}
static int int_le(lua_State* L) {
    lua_pushboolean(L, check_int(L, 1) <= check_int(L, 2));
    return 1;
}
static int int_tostring(lua_State* L) {
    lua_pushinteger(L, (lua_Integer)check_int(L, 1));
    lua_tostring(L, -1);
    return 1;
}
static int int_to_number(lua_State* L) {
    lua_pushinteger(L, (lua_Integer)check_int(L, 1));
    return 1;
}

static int int_abs(lua_State* L) { return push_integer(L, abs(check_int(L, 1))); }
static int int_sqrt(lua_State* L) { return push_integer(L, (LuaLibInteger)sqrt((lua_Number)check_int(L, 1))); }
static int int_min(lua_State* L) {
    int n = lua_gettop(L); /* number of arguments */
    LuaLibInteger dmin = check_int(L, 1);
    int i;
    for (i = 2; i <= n; i++) {
        LuaLibInteger d = check_int(L, i);
        if (d < dmin) { dmin = d; }
    }
    return push_integer(L, dmin);
}
static int int_max(lua_State* L) {
    int n = lua_gettop(L); /* number of arguments */
    LuaLibInteger dmax = check_int(L, 1);
    int i;
    for (i = 2; i <= n; i++) {
        LuaLibInteger d = check_int(L, i);
        if (d > dmax) { dmax = d; }
    }
    return push_integer(L, dmax);
}

extern "C" SHARED_EXPORT int luaopen_integer(lua_State* L) {
    static const struct luaL_reg integermt[] = {
        {"__add", int_add},
        {"__sub", int_sub},
        {"__mul", int_mul},
        {"__div", int_div},
        {"__idiv", int_div},
        {"__mod", int_mod},
        {"__pow", int_pow},
        {"__unm", int_unm},
        {"__eq", int_eq},
        {"__lt", int_lt},
        {"__le", int_le},
        {"__band", int_band},
        {"__bor", int_bor},
        {"__bxor", int_bxor},
        {"__shl", int_shl},
        {"__shr", int_shr},
        {"__bnot", int_bnot},
        {"__tostring", int_tostring},
        // Aliases for bitwise functions for versions of lua without them built-in
        {"band", int_band},
        {"bor", int_bor},
        {"bxor", int_bxor},
        {"shl", int_shl},
        {"shr", int_shr},
        {"bnot", int_bnot},
        {"tonumber", int_to_number},
        // Replacements for some int-specific math library functions
        {"abs", int_abs},
        {"max", int_max},
        {"min", int_min},
        {"pow", int_pow},
        {"sqrt", int_sqrt},
        NULL, NULL
    };
    luaL_newmetatable(L, "integer");
    // Register all above functions in the created metatable
    lua_pushvalue(L, -1);
    luaL_register(L, NULL, integermt);
    // Set the metatable's __index to reference itself
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);  // rawset(metatable, "__index", metatable)
    // Set global "integer" and return the constructor function
    lua_register(L, "integer", int_new);
    lua_pushcfunction(L, int_new);
    return 1;
}