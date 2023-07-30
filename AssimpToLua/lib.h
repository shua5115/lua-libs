#ifndef ASSIMP_TO_LUA_LIB_H
#define ASSIMP_TO_LUA_LIB_H

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

#include <string>
#include <unordered_map>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>
#include <assimp/Importer.hpp>

namespace AssimpToLove
{

int import(lua_State *L);

extern "C" SHARED_EXPORT int luaopen_assimp_to_lua(lua_State *L);

int convert(lua_State *L, const aiScene *scene);

int convert(lua_State *L, const aiNode *node);

int convert(lua_State *L, const aiMesh *mesh);

int convert(lua_State *L, const aiFace *face);

int convert(lua_State *L, const aiAABB *aabb);

int convert(lua_State *L, const aiMaterial *mat);

int convert(lua_State *L, const aiMaterialProperty *prop);

int convert(lua_State *L, const aiTexture *texture);

int convert(lua_State *L, const aiAnimation *anim);

int convert(lua_State *L, const aiAnimBehaviour behavior);

int convert(lua_State *L, const aiNodeAnim *anim);

int convert(lua_State *L, const aiMeshAnim *anim);

int convert(lua_State *L, const aiMeshMorphAnim *anim);

int convert(lua_State *L, const aiBone *bone);

int convert(lua_State *L, const aiLight *light);

int convert(lua_State *L, const aiCamera *camera);

// Leaves a table on the stack containing key-value pairs
int convert(lua_State *L, const aiMetadata *metadata);

// Leaves a single value on the stack.
// Its type will depend on the type of the metadata entry.
// The value may be nil.
int convert(lua_State *L, const aiMetadataEntry *entry);

// Leaves a Transform on the stack
int convert(lua_State *L, const aiMatrix4x4 *mat4);

// Leaves a Transform on the stack
int convert(lua_State *L, const aiMatrix3x3 *mat3);

// Leaves a table of length 3 on the stack
int convert(lua_State *L, const aiVector3D *vec3);

// Leaves a table of length 2 on the stack
int convert(lua_State *L, const aiVector2D *vec2);

// Leaves a table of length 4 on the stack
int convert(lua_State *L, const aiQuaternion *quat);

// Leaves a table of length 4 on the stack
int convert(lua_State *L, const aiColor4D *col4);

// Leaves a table of length 3 on the stack
int convert(lua_State *L, const aiColor3D *col3);

// static const std::vector<love::graphics::Mesh::AttribFormat> mesh_format = {
//     {"VertexPosition", love::graphics::vertex::DATA_FLOAT, 3},
//     {"VertexTexCoord", love::graphics::vertex::DATA_FLOAT, 2},
//     {"VertexNormal", love::graphics::vertex::DATA_FLOAT, 3},
//     {"VertexTangent", love::graphics::vertex::DATA_FLOAT, 3},
//     {"VertexBitangent", love::graphics::vertex::DATA_FLOAT, 3},
//     {"VertexColor", love::graphics::vertex::DATA_UNORM8, 4},
// };

static const std::unordered_map<std::string, unsigned int> post_process_strings = {
    {"calc_tangent_space", aiProcess_CalcTangentSpace},
    {"join_identical_vertices", aiProcess_JoinIdenticalVertices},
    {"make_left_handed", aiProcess_MakeLeftHanded},
    {"triangulate", aiProcess_Triangulate},
    {"remove_components", aiProcess_RemoveComponent},
    {"gen_normals", aiProcess_GenNormals},
    {"gen_smooth_normals", aiProcess_GenSmoothNormals},
    {"split_large_meshes", aiProcess_SplitLargeMeshes},
    {"pre_transform_vertices", aiProcess_PreTransformVertices},
    {"limit_bone_weights", aiProcess_LimitBoneWeights},
    {"validate_data", aiProcess_ValidateDataStructure},
    {"improve_cache_locality", aiProcess_ImproveCacheLocality},
    {"remove_redundant_materials", aiProcess_RemoveRedundantMaterials},
    {"fix_in_facing_normals", aiProcess_FixInfacingNormals},
    {"populate_armature_data", aiProcess_PopulateArmatureData},
    {"sort_by_primitive_type", aiProcess_SortByPType},
    {"find_degenerates", aiProcess_FindDegenerates},
    {"find_invalid_data", aiProcess_FindInvalidData},
    {"gen_uv_coords", aiProcess_GenUVCoords},
    {"transform_uv_coords", aiProcess_TransformUVCoords},
    {"find_instances", aiProcess_FindInstances},
    {"optimize_meshes", aiProcess_OptimizeMeshes},
    {"optimize_graph", aiProcess_OptimizeGraph},
    {"flip_uvs", aiProcess_FlipUVs},
    {"flip_winding_order", aiProcess_FlipWindingOrder},
    {"split_by_bone_count", aiProcess_SplitByBoneCount},
    {"debone", aiProcess_Debone},
    {"global_scale", aiProcess_GlobalScale},
    {"embed_textures", aiProcess_EmbedTextures},
    {"force_gen_normals", aiProcess_ForceGenNormals},
    {"drop_normals", aiProcess_DropNormals},
    {"gen_bounding_boxes", aiProcess_GenBoundingBoxes},
    {"convert_to_left_handed", aiProcess_ConvertToLeftHanded},
    {"target_realtime_fast", aiProcessPreset_TargetRealtime_Fast},
    {"target_realtime_quality", aiProcessPreset_TargetRealtime_Quality},
    {"target_realtime_max_quality", aiProcessPreset_TargetRealtime_MaxQuality},
};

}
#endif