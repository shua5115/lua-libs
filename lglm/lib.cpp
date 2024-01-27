#include "lib.h"
#include <math.h>
#include "vec.h"


static luaL_Reg lib[] = {
    {NULL, NULL}
};

extern "C" SHARED_EXPORT int luaopen_luaglm(lua_State *L) {
    lua_createtable(L, 0, 0); // library table
    luaL_register(L, NULL, lib);
    return 1;
}