#include <lua.h>

typedef struct mat2x2 {
    double m[4];
} mat2x2;

typedef mat2x2 mat2;

typedef struct mat3x3 {
    double m[9];
} mat3x3;

typedef mat3x3 mat3;

typedef struct mat4x4 {
    double m[16];
} mat4x4;

typedef struct matMxN {
    unsigned int M;
    unsigned int N;
    double m[1]; // For dynamic sizing
} matMxN;

int lgl_matrixCompMult(lua_State *L);

int lgl_outerProduct(lua_State *L);

int lgl_transpose(lua_State *L);

int lgl_determinant(lua_State *L);

int lgl_inverse(lua_State *L);



