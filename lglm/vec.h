#include <lua.h>

typedef struct vec2 {
    double x, y, z;
} vec2;

typedef struct vec3 {
    double x, y, z;
} vec3;

typedef struct vec4 {
    double x, y, z, w;
} vec4;

int lgl_vec_to_table(lua_State *L);

int lgl_vec2(lua_State *L);

int lgl_vec3(lua_State *L);

int lgl_vec4(lua_State *L);

int lgl_swizzle(lua_State *L);

int lgl_dot(lua_State *L);

int lgl_cross(lua_State *L);

int lgl_normalize(lua_State *L);

int lgl_faceforward(lua_State *L);

int lgl_reflect(lua_State *L);

int lgl_refract(lua_State *L);