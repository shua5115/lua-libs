#include "lib.h"

namespace AssimpToLove
{

static const char REGISTRY_ID = 'E';
enum RegistryKey {
    VERTEX_FORMAT,
    NEW_TRANSFORM,
    REGISTRY_SIZE
};

int get_version(lua_State *L)
{
	lua_pushfstring(L, "%d.%d.%d", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionPatch());
	return 1;
}

int get_postprocess_options(lua_State *L)
{
	lua_createtable(L, post_process_strings.size(), 0);
	{
		int i = 1;
		for(auto e : post_process_strings) {
			lua_pushinteger(L, i);
			lua_pushlstring(L, e.first.data(), e.first.length());
			lua_settable(L, -3);
			i++;
		}
	}
	return 1;
}

// Lua function with signature:
// function import(filedata, filename, post_process_options)
// filedata: FileData or string
// post_process_options: table? An optional list of strings which match the options returned by getPostProcessOptions()
int import(lua_State *L) {
	Assimp::Importer importer;
	std::string extension = "";
    void *file_data = nullptr;
    size_t data_size = 0;
    
    // First step: obtain a FileData love object which contains the asset data

    if(lua_isuserdata(L, 1)) {
        int has_metatable = lua_getmetatable(L, 1);
        if(!has_metatable) {
            lua_pushnil(L);
            lua_pushstring(L, "Provided userdata is not a love Object.");
            return 2;
        }
        lua_getfield(L, -1, "typeOf");
        if(!lua_isfunction(L, -1)) {
            lua_pushnil(L);
            lua_pushstring(L, "Provided userdata is not a love Object.");
            return 2;
        }
        lua_pushvalue(L, 1);
        lua_pushstring(L, "FileData");
        // Stack:
        // -4: table userdata metatable
        // -3: function love.Object.typeOf
        // -2: userdata self
        // -1: string type name
        lua_call(L, 2, 1);
        if(!lua_toboolean(L, -1)) {
            lua_pushnil(L);
            lua_pushstring(L, "Provided userdata is not a FileData.");
            return 2;
        }
        // Now we have verified that the userdata is a FileData
        lua_pushvalue(L, 1);
    } else if(lua_isstring(L, 1)) {
        // pass the args from this function directly into love.filesystem.newFileData()
        lua_getglobal(L, "love");
        lua_getfield(L, -1, "filesystem");
        lua_getfield(L, -1, "newFileData");
        lua_pushvalue(L, 1);
        lua_call(L, 2, 1);
        // Stack:
        // -3: table love
        // -2: table love.filesystem
        // -1: userdata FileData?
        if (!lua_isuserdata(L, -1)) {
            lua_pushnil(L);
            lua_pushstring(L, "Could not load FileData from ");
            lua_pushvalue(L, 1);
            lua_concat(L, 2);
            return 2;
        }
    } else {
        lua_pushnil(L);
        lua_pushstring(L, "Failed to supply a FileData.");
        return 2;
    }

    // Stack:
    // <-1: stuff, but assume it doesn't need to be used. Ignoring from now on...
    // -1: FileData asset data
    lua_getmetatable(L, -1);
    lua_getfield(L, -1, "getExtension");
    lua_pushvalue(L, -3);
    lua_call(L, 1, 1);
    extension = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -1, "getSize");
    lua_pushvalue(L, -3);
    lua_call(L, 1, 1);
    data_size = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -1, "getPointer");
    lua_pushvalue(L, -3);
    lua_call(L, 1, 1);
    file_data = lua_touserdata(L, -1);
    lua_pop(L, 1);
    // Stack:
    // -2: FileData asset data
    // -1: table FileData metatable

    // Always triangulate meshes, since love2d doesn't support vertex mapping with its Lua interface.
	unsigned int opt_post_process = aiProcess_Triangulate;
	if (lua_istable(L, 2)) {
		// get options by iterating through every item in the list
		int i = 1;
		while (true) {
			lua_pushinteger(L, i);
			lua_gettable(L, 2);
			if (lua_isnoneornil(L, -1)) {
				lua_pop(L, 1);
				break;
			}
			if (lua_isstring(L, -1)) {
				const std::string key = lua_tostring(L, -1);
				if(post_process_strings.find(key) != post_process_strings.end()) {
					// printf("Applying post process step: %s\n", key.c_str());
					const unsigned int val = post_process_strings.at(key);
					opt_post_process |= val;
				}
			}
			lua_pop(L, 1);
			i++;
		}
	}

	// At this point all stack modification done by the post-process option parsing should be cleared
	// and the Data object should remain at stack index -1

	const aiScene* scene = importer.ReadFileFromMemory((const char *)file_data, data_size, opt_post_process, extension.c_str());
	if (scene == nullptr) {
		lua_pushnil(L);
		lua_pushstring(L, "Could not import the asset from provided data");
		return 2;
	} else {
		convert(L, scene); // pushes a table
	}
	// Stack:
    // -3: FileData asset data
    // -2: table FileData metatable
    // -1: table imported scene

	// stack debug:
	// printf("Pushed table address: %p\n", lua_topointer(L, -1));
	// printf("Contents of the stack after conversion:\n");
	// for(int i = 0; i <= lua_gettop(L); i++) {
	// 	const char *valstr = lua_tostring(L, i);
	// 	const void *p = lua_topointer(L, i);
	// 	printf("%d. type: %s, value: %s, addr: %p\n", i, luaL_typename(L, i), valstr == nullptr ? "N/A" : valstr, p);
	// }

	// return the scene table
	return 1;
}

int get_vertex_format(lua_State *L) {
    lua_pushlightuserdata(L, (void*) &REGISTRY_ID);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_pushinteger(L, VERTEX_FORMAT);
    lua_gettable(L, -2);
    return 1;
}

static luaL_Reg funcs[] = {
    {"getVersion", get_version},
    {"getPostProcessOptions", get_postprocess_options},
    {"import", import},
    {"getVertexFormat", get_vertex_format},
    {NULL, NULL}
};

extern "C" SHARED_EXPORT int luaopen_assimp_to_lua(lua_State *L) {
    luaL_register(L, "assimp_to_lua", funcs);

    lua_newtable(L); // table to be put in registry 
    lua_pushlightuserdata(L, (void*) &REGISTRY_ID); // unique key
    lua_pushvalue(L, -2);
    lua_settable(L, LUA_REGISTRYINDEX);
    // Stack:
    // -2: module table to be returned by "require"
    // -1: table which was put in registry
    lua_pushinteger(L, VERTEX_FORMAT);
    // I'd rather parse this at runtime than write the
    // whole table definition with the lua API. I hope you understand.
    int literal_result = luaL_loadstring(L,
R"(return {
{"VertexPosition", "float", 3},
{"VertexTexCoord", "float", 2},
{"VertexNormal", "float", 3},
{"VertexTangent", "float", 3},
{"VertexBitangent", "float", 3},
{"VertexColor", "byte", 4}
})"
    );
    lua_call(L, 0, 1);
    lua_settable(L, -3);
    
    lua_getglobal(L, "love");
    lua_getfield(L, -1, "math");
    lua_getfield(L, -1, "newTransform");
    lua_pushinteger(L, NEW_TRANSFORM);
    lua_pushvalue(L, -2);
    // Stack:
    // -7: module table to be returned by require
    // -6: table which was put in registry
    // -5: table love
    // -4: table love.math
    // -3: function newTransform
    // -2: number NEW_TRANSFORM registry table index
    // -1: function newTransform
    lua_settable(L, -6);
    lua_pop(L, 3); // Pop love, math, and newTransform

    // Stack:
    // -2: module table to be returned by require
    // -1: table which was put in registry

    lua_pop(L, 1); // Pop the table stored in the registry
    // Stack:
    // -1: module table to be returned by "require"
    return 1;
}

// Creates a new love.Transform.
// Can set the matrix's values by supplying an elements array of length 16 in row-major order.
// You are responsible for ensuring this length.
// If the elements array is not provided (set to null), then the matrix will be default-initialized.
static int create_transform(lua_State *L, lua_Number *elements = nullptr) {
    lua_pushlightuserdata(L, (void*) &REGISTRY_ID);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_pushinteger(L, NEW_TRANSFORM);
    lua_gettable(L, -2);
    // Stack:
    // -2: table registry[REGISTRY_ID]
    // -1: function love.math.newTransform (cached in registry)
    lua_call(L, 0, 1);
    lua_remove(L, -2);
    // Stack:
    // -1: userdata love.Transform
    if (elements != nullptr) {
        lua_getmetatable(L, -1);
        lua_getfield(L, -1, "setMatrix");
        lua_pushvalue(L, -3);
        for(int i = 0; i < 16; i++) {
            lua_pushnumber(L, elements[i]);
        }
        lua_call(L, 17, 0); // self + 16 elements
        // Stack:
        // -1: table love.Transform metatable
        // -2: userdata love.transform
        lua_pop(L, 1);
    }
    // Stack:
    // -1: userdata love.Transform
    return 1;
}

int convert(lua_State *L, const aiScene *scene) {
    // make a table to temporarily store the node structure
    // store node tree in a flat list, breadth first order
    size_t nodestack_i = 0;
    std::vector<const aiNode *> nodelist;
    std::unordered_map<const aiNode *, size_t> node_indices;
    nodelist.push_back(scene->mRootNode);
    while (nodestack_i < nodelist.size()) {
        const aiNode *current = nodelist[nodestack_i];
        node_indices[current] = nodestack_i;
        for (unsigned int i = 0; i < current->mNumChildren; i++) {
            nodelist.push_back(current->mChildren[i]);
        }
        nodestack_i += 1;
    }
    // then save all converted nodes in the temporary table,
    // replacing all references to nodes with references to tables in this list
    lua_createtable(L, nodelist.size(), nodelist.size() / 2);
    for (int i = 0; i < nodelist.size(); i++) {
        auto node = nodelist[i];
        convert(L, node);
        // refer to the node in the array
        lua_pushinteger(L, i + 1);
        lua_pushvalue(L, -2);
        lua_settable(L, -4);
        // and make it accessable by name
        lua_pushlstring(L, node->mName.data, node->mName.length);
        lua_pushvalue(L, -2);
        lua_settable(L, -4);
        // Stack:
        // -2: node list table
        // -1: current node table
        lua_pop(L, 1);  // remove the extra reference to the node table
    }

    for (int i = 0; i < nodelist.size(); i++) {
        auto node = nodelist[i];
        lua_pushinteger(L, i + 1);
        lua_gettable(L, -2); // push the associated node table
        // Stack:
        // -2: node list table
        // -1: current node table
        if (node->mParent != nullptr) {
            unsigned int parent_idx = node_indices[node->mParent];
            lua_pushinteger(L, parent_idx);
            // Stack:
            // -3: node list table
            // -2: current node table
            // -1: parent node index
            lua_gettable(L, -3);  // push the node table of the parent
            // Stack:
            // -3: node list table
            // -2: current node table
            // -1: parent node table
            lua_setfield(L, -2, "parent");
        }

        lua_createtable(L, node->mNumChildren, 0);  // children table
        for (unsigned int j = 0; j < node->mNumChildren; j++) {
            // Stack:
            // -3: node list table
            // -2: current node table
            // -1: children table
            unsigned int child_idx = node_indices[node->mChildren[j]];
            lua_pushinteger(L, j + 1);  // list insertion index
            lua_pushinteger(L, child_idx);
            // Stack:
            // -5: node list table
            // -4: current node table
            // -3: children table
            // -2: insertion index
            // -1: child node index
            lua_gettable(L, -5);
            // Stack:
            // -5: node list table
            // -4: current node table
            // -3: children table
            // -2: insertion index
            // -1: child node table
            lua_settable(L, -3);
        }
        // Stack:
        // -3: node list table
        // -2: current node table
        // -1: children table
        lua_setfield(L, -2, "children");
        lua_pop(L, 1);  // pop the current node table
        // Stack:
        // -1: node list table
        // We made it! We didn't bloat the stack!
    }

    // this table is the scene table
    lua_newtable(L);
    // Stack:
    // -2: node list table
    // -1: scene table

    lua_pushlstring(L, scene->mName.data, scene->mName.length);
    lua_setfield(L, -2, "name");

    unsigned int flags = scene->mFlags;
    lua_createtable(L, 0, 6);
    lua_pushboolean(L, (flags & AI_SCENE_FLAGS_INCOMPLETE) != 0);
    lua_setfield(L, -2, "incomplete");
    lua_pushboolean(L, (flags & AI_SCENE_FLAGS_VALIDATED) != 0);
    lua_setfield(L, -2, "validated");
    lua_pushboolean(L, (flags & AI_SCENE_FLAGS_VALIDATION_WARNING) != 0);
    lua_setfield(L, -2, "warning");
    lua_pushboolean(L, (flags & AI_SCENE_FLAGS_NON_VERBOSE_FORMAT) != 0);
    lua_setfield(L, -2, "nonverbose");
    lua_pushboolean(L, (flags & AI_SCENE_FLAGS_TERRAIN) != 0);
    lua_setfield(L, -2, "terrain");
    lua_pushboolean(L, (flags & AI_SCENE_FLAGS_ALLOW_SHARED) != 0);
    lua_setfield(L, -2, "allow_shared");
    lua_setfield(L, -2, "flags");

    // Stack:
    // -2: node list table
    // -1: scene table
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "nodes");

    lua_pushinteger(L, 1);  // root node index will always be 1
    lua_gettable(L, -3);    // push the root node table from the node list table
    lua_setfield(L, -2, "root_node");

    lua_remove(L, -2);  // remove the nodes table, we don't want to leak memory.

    lua_createtable(L, scene->mNumMeshes, 0);
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        lua_pushinteger(L, i + 1);
        convert(L, scene->mMeshes[i]);
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "meshes");

    lua_createtable(L, scene->mNumTextures, 0);
    for (unsigned int i = 0; i < scene->mNumTextures; i++) {
        lua_pushinteger(L, i + 1);
        convert(L, scene->mTextures[i]);
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "textures");

    lua_createtable(L, scene->mNumMaterials, 0);
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        lua_pushinteger(L, i + 1);
        convert(L, scene->mMaterials[i]);
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "materials");

    lua_createtable(L, scene->mNumLights, 0);
    for (unsigned int i = 0; i < scene->mNumLights; i++) {
        lua_pushinteger(L, i + 1);
        convert(L, scene->mLights[i]);
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "lights");

    lua_createtable(L, scene->mNumCameras, 0);
    for (unsigned int i = 0; i < scene->mNumCameras; i++) {
        lua_pushinteger(L, i + 1);
        convert(L, scene->mCameras[i]);
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "cameras");

    convert(L, scene->mMetaData);
    lua_setfield(L, -2, "metadata");

    return 1;
}

int convert(lua_State *L, const aiNode *node) {
    lua_createtable(L, 0, 4);

    lua_pushlstring(L, node->mName.data, node->mName.length);
    lua_setfield(L, -2, "name");

    convert(L, &node->mTransformation);
    lua_setfield(L, -2, "transform");

    convert(L, node->mMetaData);
    lua_setfield(L, -2, "metadata");

    lua_createtable(L, node->mNumMeshes, 0);
    for (int i = 0; i < node->mNumMeshes; i++) {
        lua_pushinteger(L, i + 1);
        lua_pushinteger(L, node->mMeshes[i]);
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "meshes");

    return 1;
}

int convert(lua_State *L, const aiMesh *mesh) {
    // using namespace love::graphics;
    // Graphics *g = Module::getInstance<Graphics>(Module::M_GRAPHICS);

    lua_newtable(L);  // Mesh

    lua_pushlstring(L, mesh->mName.data, mesh->mName.length);
    lua_setfield(L, -2, "name");

    convert(L, &mesh->mAABB);
    lua_setfield(L, -2, "aabb");

    lua_newtable(L);
    if ((mesh->mPrimitiveTypes & aiPrimitiveType_POINT) != 0) {
        lua_pushstring(L, "point");
        lua_pushboolean(L, true);
        lua_settable(L, -3);
    }
    if ((mesh->mPrimitiveTypes & aiPrimitiveType_LINE) != 0) {
        lua_pushstring(L, "line");
        lua_pushboolean(L, true);
        lua_settable(L, -3);
    }
    if ((mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) != 0) {
        lua_pushstring(L, "triangle");
        lua_pushboolean(L, true);
        lua_settable(L, -3);
    }
    if ((mesh->mPrimitiveTypes & aiPrimitiveType_POLYGON) != 0) {
        lua_pushstring(L, "polygon");
        lua_pushboolean(L, true);
        lua_settable(L, -3);
    }
    if ((mesh->mPrimitiveTypes & aiPrimitiveType_NGONEncodingFlag) != 0) {
        lua_pushstring(L, "ngon");
        lua_pushboolean(L, true);
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "primitives");

    lua_pushinteger(L, mesh->mMaterialIndex);
    lua_setfield(L, -2, "material_index");

    // Lua pseudo-equivalent:
    // local love_mesh = love.graphics.newMesh(get_vertex_format(), vertex_count, "triangles", "dynamic")
    // for (int i = 0; i < aiMesh.mNumVertices; i++) {
    //     love_mesh:setVertex(i+1, aiMesh vertex properties...)
    // }

    lua_getglobal(L, "love");
    lua_getfield(L, -1, "graphics");
    lua_getfield(L, -1, "newMesh");
    // args
    lua_pushcfunction(L, get_vertex_format);
    lua_call(L, 0, 1);
    lua_pushinteger(L, mesh->mNumVertices);
    lua_pushstring(L, "triangles"); // Assume mesh is triangulated (enforced in preprocessor)
    lua_pushstring(L, "dynamic");
    lua_call(L, 4, 1);
    lua_getmetatable(L, -1);
    lua_getfield(L, -1, "setVertex");
    // Stack:
    // -6 table mesh table
    // -5 table love
    // -4 table love.graphics
    // -3 userdata love.Mesh
    // -2 table Mesh metatable
    // -1 function setVertex
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // setVertex function (push again to maintain reference)
        lua_pushvalue(L, -1);

        // Mesh userdata (self)
        lua_pushvalue(L, -4);

        // Vertex index
        lua_pushinteger(L, i+1);
        
        // Position
        lua_pushnumber(L, mesh->mVertices[i].x);
        lua_pushnumber(L, mesh->mVertices[i].y);
        lua_pushnumber(L, mesh->mVertices[i].z);
        // TexCoord
        if (mesh->mTextureCoords[0] != nullptr) {
            lua_pushnumber(L, mesh->mTextureCoords[0][i].x);
            lua_pushnumber(L, mesh->mTextureCoords[0][i].y);
        } else {
            lua_pushnumber(L, 0);
            lua_pushnumber(L, 0);
        }
        // Normal
        if (mesh->mNormals != nullptr) {
            aiVector3D aiv = mesh->mNormals[i];
            lua_pushnumber(L, aiv.x);
            lua_pushnumber(L, aiv.y);
            lua_pushnumber(L, aiv.z);
        } else {
            lua_pushnumber(L, 0);
            lua_pushnumber(L, 0);
            lua_pushnumber(L, 0);
        }
        if (mesh->mTangents != nullptr) {
            // Tangent
            aiVector3D t = mesh->mTangents[i];
            lua_pushnumber(L, t.x);
            lua_pushnumber(L, t.y);
            lua_pushnumber(L, t.z);
            // Bitangent
            t = mesh->mBitangents[i];
            lua_pushnumber(L, t.x);
            lua_pushnumber(L, t.y);
            lua_pushnumber(L, t.z);
        } else {
            lua_pushnumber(L, 0);
            lua_pushnumber(L, 0);
            lua_pushnumber(L, 0);
            lua_pushnumber(L, 0);
            lua_pushnumber(L, 0);
            lua_pushnumber(L, 0);
        }

        // Color
        if (mesh->mColors != nullptr && mesh->HasVertexColors(0)) {
            aiColor4D col = mesh->mColors[0][i];
            lua_pushnumber(L, col.r);
            lua_pushnumber(L, col.g);
            lua_pushnumber(L, col.b);
            lua_pushnumber(L, col.a);
        } else {
            lua_pushnumber(L, 1.0);
            lua_pushnumber(L, 1.0);
            lua_pushnumber(L, 1.0);
            lua_pushnumber(L, 1.0);
        }
        
        lua_call(L, 20, 0);
    }
    lua_pop(L, 2);
    // Stack:
    // -4 table mesh table
    // -3 table love
    // -2 table love.graphics
    // -1 userdata love.Mesh
    lua_setfield(L, -4, "mesh");
    lua_pop(L, 2);
    // Stack:
    // -1 table mesh table
    return 1;
}

int convert(lua_State *L, const aiFace *face) {
    lua_createtable(L, face->mNumIndices, 0);
    for (unsigned int i = 0; i < face->mNumIndices; i++) {
        lua_pushinteger(L, i + 1);
        lua_pushinteger(L, face->mIndices[i]);
        lua_settable(L, -3);
    }
    return 1;
}

int convert(lua_State *L, const aiAABB *aabb) {
    lua_createtable(L, 0, 2);

    convert(L, &aabb->mMin);
    lua_setfield(L, -2, "min");

    convert(L, &aabb->mMax);
    lua_setfield(L, -2, "max");

    return 1;
}

int convert(lua_State *L, const aiMaterial *mat) {
    lua_createtable(L, mat->mNumProperties, mat->mNumProperties);
    for (unsigned int i = 0; i < mat->mNumProperties; i++) {
        auto prop = mat->mProperties[i];
        convert(L, prop);  // +1
        // First add this property to the array
        lua_pushinteger(L, i + 1);
        lua_pushvalue(L, -2);
        lua_settable(L, -4);
        // Then add it to the dictionary
        lua_pushlstring(L, prop->mKey.data, prop->mKey.length);
        lua_pushvalue(L, -2);
        lua_settable(L, -4);
        // Pop the extra property table reference
        lua_pop(L, 1);
    }
    return 1;
}

int convert(lua_State *L, const aiMaterialProperty *prop) {
    lua_newtable(L);

    lua_pushlstring(L, prop->mKey.data, prop->mKey.length);
    lua_setfield(L, -2, "key");

    lua_pushinteger(L, prop->mIndex);
    lua_setfield(L, -2, "index");

    switch (prop->mType) {
        case aiPropertyTypeInfo::aiPTI_Float:
        case aiPropertyTypeInfo::aiPTI_Double:
        case aiPropertyTypeInfo::aiPTI_Integer:
            lua_pushstring(L, "number");
            break;
        case aiPropertyTypeInfo::aiPTI_String:
            lua_pushstring(L, "string");
            break;
        default:
            lua_pushstring(L, "raw");
            break;
    }
    lua_setfield(L, -2, "type");

    switch (prop->mSemantic) {
        case aiTextureType_DIFFUSE:
            lua_pushstring(L, "diffuse");
            break;
        case aiTextureType_SPECULAR:
            lua_pushstring(L, "specular");
            break;
        case aiTextureType_AMBIENT:
            lua_pushstring(L, "ambient");
            break;
        case aiTextureType_EMISSIVE:
            lua_pushstring(L, "emissive");
            break;
        case aiTextureType_HEIGHT:
            lua_pushstring(L, "height");
            break;
        case aiTextureType_NORMALS:
            lua_pushstring(L, "normals");
            break;
        case aiTextureType_SHININESS:
            lua_pushstring(L, "shininess");
            break;
        case aiTextureType_OPACITY:
            lua_pushstring(L, "opacity");
            break;
        case aiTextureType_DISPLACEMENT:
            lua_pushstring(L, "displacement");
            break;
        case aiTextureType_LIGHTMAP:
            lua_pushstring(L, "lightmap");
            break;
        case aiTextureType_REFLECTION:
            lua_pushstring(L, "reflection");
            break;
        case aiTextureType_BASE_COLOR:
            lua_pushstring(L, "base_color");
            break;
        case aiTextureType_NORMAL_CAMERA:
            lua_pushstring(L, "normal_camera");
            break;
        case aiTextureType_EMISSION_COLOR:
            lua_pushstring(L, "emission_color");
            break;
        case aiTextureType_METALNESS:
            lua_pushstring(L, "metalness");
            break;
        case aiTextureType_DIFFUSE_ROUGHNESS:
            lua_pushstring(L, "diffuse_roughness");
            break;
        case aiTextureType_AMBIENT_OCCLUSION:
            lua_pushstring(L, "ambient_occlusion");
            break;
        case aiTextureType_SHEEN:
            lua_pushstring(L, "sheen");
            break;
        case aiTextureType_CLEARCOAT:
            lua_pushstring(L, "clearcoat");
            break;
        case aiTextureType_TRANSMISSION:
            lua_pushstring(L, "transmission");
            break;
        case aiTextureType_UNKNOWN:
            lua_pushstring(L, "unknown");
            break;
        default:
            lua_pushboolean(L, false);
            break;
    }
    lua_setfield(L, -2, "texture_type");

    lua_pushlstring(L, prop->mData, prop->mDataLength);
    lua_setfield(L, -2, "data");

    return 1;
}

int convert(lua_State *L, const aiTexture *texture) {
    lua_getglobal(L, "love");
    lua_getfield(L, -1, "graphics");
    lua_getfield(L, -2, "image");
    lua_getfield(L, -1, "newImageData");
    if (texture->mHeight > 0) {
        // Stack
        // -4: table love
        // -3: table love.graphics
        // -2: table love.image
        // -1: function love.image.newImageData
        lua_pushinteger(L, texture->mWidth);
        lua_pushinteger(L, texture->mHeight);
        lua_pushstring(L, "rgba8");
        const unsigned int pixelcount = texture->mWidth * texture->mHeight;
        char *newdata = (char *)calloc(pixelcount, 4);
        if (newdata == nullptr) {
            lua_settop(L, 0); // clear stack in case error was caught. Don't leak memory.
            luaL_error(L, "Out of memory.");
        }
        for (unsigned int i = 0; i < pixelcount; i++) {
            auto texel = texture->pcData[i];
            *(newdata + i * 4 + 0) = texel.r;
            *(newdata + i * 4 + 1) = texel.g;
            *(newdata + i * 4 + 2) = texel.b;
            *(newdata + i * 4 + 3) = texel.a;
        }
        lua_pushlstring(L, newdata, pixelcount*4);
        free(newdata); // newdata is memcpy'd when the string was created, safe to free.
        // Stack
        // -8: table love
        // -7: table love.graphics
        // -6: table love.image
        // -5: function love.image.newImageData
        // -4: number width
        // -3: number height
        // -2: string pixel format "rgba8"
        // -1: string raw pixel data
        lua_call(L, 4, 1);
        // Stack
        // -4: table love
        // -3: table love.graphics
        // -2: table love.image
        // -1: userdata love.ImageData
    } else {
        // Stack
        // -4: table love
        // -3: table love.graphics
        // -2: table love.image
        // -1: function love.image.newImageData
        lua_getfield(L, -4, "filesystem");
        lua_getfield(L, -1, "newFileData");
        lua_pushlstring(L, (const char *) texture->pcData, texture->mWidth);
        lua_pushlstring(L, texture->mFilename.data, texture->mFilename.length);
        lua_call(L, 2, 1);
        // Stack
        // -5: table love
        // -4: table love.graphics
        // -3: table love.image
        // -2: function love.image.newImageData
        // -2: table love.filesystem
        // -1: userdata love.FileData
        lua_remove(L, -2);
        lua_call(L, 1, 1);
    }
    // Stack
    // -4: table love
    // -3: table love.graphics
    // -2: table love.image
    // -1: userdata love.ImageData
    lua_getfield(L, -3, "newImage");
    // Stack
    // -5: table love
    // -4: table love.graphics
    // -3: table love.image
    // -2: userdata love.ImageData
    // -1: function love.graphics.newImage
    lua_pushvalue(L, -2);
    lua_call(L, 1, 1);
    // Stack
    // -5: table love
    // -4: table love.graphics
    // -3: table love.image
    // -2: userdata love.ImageData
    // -1: userdata love.Image
    lua_replace(L, -5);
    lua_settop(L, -5);
    // Stack
    // -1: userdata love.Image
    return 1;
}

int convert(lua_State *L, const aiAnimation *anim) {
    lua_newtable(L);

    lua_pushlstring(L, anim->mName.data, anim->mName.length);
    lua_setfield(L, -2, "name");

    lua_pushnumber(L, anim->mDuration);
    lua_setfield(L, -2, "duration");

    lua_pushnumber(L, anim->mTicksPerSecond);
    lua_setfield(L, -2, "fps");

    lua_createtable(L, anim->mNumChannels, 0);
    for (unsigned int i = 0; i < anim->mNumChannels; i++) {
        lua_pushinteger(L, i + 1);
        convert(L, anim->mChannels[i]);
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "node_channels");

    lua_createtable(L, anim->mNumMeshChannels, 0);
    for (unsigned int i = 0; i < anim->mNumMeshChannels; i++) {
        lua_pushinteger(L, i + 1);
        convert(L, anim->mMeshChannels[i]);
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "mesh_channels");

    lua_createtable(L, anim->mNumMorphMeshChannels, 0);
    for (unsigned int i = 0; i < anim->mNumMorphMeshChannels; i++) {
        lua_pushinteger(L, i + 1);
        convert(L, anim->mMorphMeshChannels[i]);
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "morph_channels");

    return 1;
}

int convert(lua_State *L, const aiAnimBehaviour behavior) {
    switch (behavior) {
        case aiAnimBehaviour_CONSTANT:
            lua_pushstring(L, "constant");
            break;
        case aiAnimBehaviour_LINEAR:
            lua_pushstring(L, "linear");
            break;
        case aiAnimBehaviour_REPEAT:
            lua_pushstring(L, "repeat");
            break;
        default:
            lua_pushstring(L, "default");
            break;
    }
    return 1;
}

int convert(lua_State *L, const aiNodeAnim *anim) {
    lua_newtable(L);

    lua_pushlstring(L, anim->mNodeName.data, anim->mNodeName.length);
    lua_setfield(L, -2, "node_name");

    convert(L, anim->mPreState);
    lua_setfield(L, -2, "pre_state");

    convert(L, anim->mPostState);
    lua_setfield(L, -2, "post_state");

    lua_createtable(L, anim->mNumPositionKeys, 0);  // times
    lua_createtable(L, anim->mNumPositionKeys, 0);  // values
    for (unsigned int i = 0; i < anim->mNumPositionKeys; i++) {
        auto key = anim->mPositionKeys[i];
        lua_pushinteger(L, i + 1);
        lua_pushnumber(L, key.mTime);
        lua_settable(L, -4);

        lua_pushinteger(L, i + 1);
        convert(L, &key.mValue);
        lua_settable(L, -3);
    }
    lua_setfield(L, -3, "position_keys");
    lua_setfield(L, -2, "position_times");

    lua_createtable(L, anim->mNumRotationKeys, 0);  // times
    lua_createtable(L, anim->mNumRotationKeys, 0);  // values
    for (unsigned int i = 0; i < anim->mNumRotationKeys; i++) {
        auto key = anim->mRotationKeys[i];
        lua_pushinteger(L, i + 1);
        lua_pushnumber(L, key.mTime);
        lua_settable(L, -4);

        lua_pushinteger(L, i + 1);
        convert(L, &key.mValue);
        lua_settable(L, -3);
    }
    lua_setfield(L, -3, "rotation_keys");
    lua_setfield(L, -2, "rotation_times");

    lua_createtable(L, anim->mNumScalingKeys, 0);  // times
    lua_createtable(L, anim->mNumScalingKeys, 0);  // values
    for (unsigned int i = 0; i < anim->mNumScalingKeys; i++) {
        auto key = anim->mPositionKeys[i];
        lua_pushinteger(L, i + 1);
        lua_pushnumber(L, key.mTime);
        lua_settable(L, -4);

        lua_pushinteger(L, i + 1);
        convert(L, &key.mValue);
        lua_settable(L, -3);
    }
    lua_setfield(L, -3, "scale_keys");
    lua_setfield(L, -2, "scale_times");

    return 1;
}

int convert(lua_State *L, const aiMeshAnim *anim) {
    lua_newtable(L);

    lua_pushlstring(L, anim->mName.data, anim->mName.length);
    lua_setfield(L, -2, "mesh_name");

    lua_createtable(L, anim->mNumKeys, 0);  // times
    lua_createtable(L, anim->mNumKeys, 0);  // values
    for (unsigned int i = 0; i < anim->mNumKeys; i++) {
        auto key = anim->mKeys[i];
        lua_pushinteger(L, i + 1);
        lua_pushnumber(L, key.mTime);
        lua_settable(L, -4);

        lua_pushinteger(L, i + 1);
        lua_pushinteger(L, key.mValue);
        lua_settable(L, -3);
    }
    lua_setfield(L, -3, "keys");
    lua_setfield(L, -2, "times");

    return 1;
}

int convert(lua_State *L, const aiMeshMorphAnim *anim) {
    lua_newtable(L);

    lua_pushlstring(L, anim->mName.data, anim->mName.length);
    lua_setfield(L, -2, "mesh_name");

    lua_createtable(L, anim->mNumKeys, 0);
    for (unsigned int i = 0; i < anim->mNumKeys; i++) {
        lua_pushinteger(L, i + 1);
        lua_pushnumber(L, anim->mKeys[i].mTime);
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "times");

    lua_createtable(L, anim->mNumKeys, 0);
    for (unsigned int i = 0; i < anim->mNumKeys; i++) {
        auto key = anim->mKeys[i];
        lua_pushinteger(L, i + 1);
        lua_createtable(L, key.mNumValuesAndWeights, 0);
        for (unsigned int j; j < key.mNumValuesAndWeights; j++) {
            lua_pushinteger(L, j + 1);
            lua_pushinteger(L, key.mValues[j]);
            lua_settable(L, -3);
        }
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "values");

    lua_createtable(L, anim->mNumKeys, 0);
    for (unsigned int i = 0; i < anim->mNumKeys; i++) {
        auto key = anim->mKeys[i];
        lua_pushinteger(L, i + 1);
        lua_createtable(L, key.mNumValuesAndWeights, 0);
        for (unsigned int j; j < key.mNumValuesAndWeights; j++) {
            lua_pushinteger(L, j + 1);
            lua_pushnumber(L, key.mWeights[j]);
            lua_settable(L, -3);
        }
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "weights");

    return 1;
}

int convert(lua_State *L, const aiBone *bone) {
    lua_newtable(L);

    lua_pushlstring(L, bone->mName.data, bone->mName.length);
    lua_setfield(L, -2, "name");

    convert(L, &bone->mOffsetMatrix);
    lua_setfield(L, -2, "offset");

    lua_createtable(L, bone->mNumWeights, 0);
    for (unsigned int i = 0; i < bone->mNumWeights; i++) {
        lua_pushinteger(L, i + 1);
        lua_pushinteger(L, bone->mWeights[i].mVertexId);
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "vertex_ids");

    lua_createtable(L, bone->mNumWeights, 0);
    for (unsigned int i = 0; i < bone->mNumWeights; i++) {
        lua_pushinteger(L, i + 1);
        lua_pushnumber(L, bone->mWeights[i].mWeight);
        lua_settable(L, -3);
    }
    lua_setfield(L, -2, "weights");

    return 1;
}

int convert(lua_State *L, const aiLight *light) {
    lua_newtable(L);

    lua_pushlstring(L, light->mName.data, light->mName.length);
    lua_setfield(L, -2, "node_name");

    switch (light->mType) {
        case aiLightSource_DIRECTIONAL:
            lua_pushstring(L, "directional");
            break;
        case aiLightSource_POINT:
            lua_pushstring(L, "point");
            break;
        case aiLightSource_SPOT:
            lua_pushstring(L, "spot");
            break;
        case aiLightSource_AMBIENT:
            lua_pushstring(L, "ambient");
            break;
        case aiLightSource_AREA:
            lua_pushstring(L, "area");
            break;
        default:
            lua_pushstring(L, "");
    }
    lua_setfield(L, -2, "type");

    convert(L, &light->mPosition);
    lua_setfield(L, -2, "position");

    convert(L, &light->mSize);
    lua_setfield(L, -2, "size");

    convert(L, &light->mDirection);
    lua_setfield(L, -2, "forward");

    convert(L, &light->mUp);
    lua_setfield(L, -2, "up");

    convert(L, &light->mColorAmbient);
    lua_setfield(L, -2, "ambient");

    convert(L, &light->mColorDiffuse);
    lua_setfield(L, -2, "diffuse");

    convert(L, &light->mColorSpecular);
    lua_setfield(L, -2, "specualar");

    lua_pushnumber(L, light->mAngleInnerCone);
    lua_setfield(L, -2, "inner_cone_angle");

    lua_pushnumber(L, light->mAngleOuterCone);
    lua_setfield(L, -2, "outer_cone_angle");

    lua_pushnumber(L, light->mAttenuationConstant);
    lua_setfield(L, -2, "attenuation_constant");

    lua_pushnumber(L, light->mAttenuationLinear);
    lua_setfield(L, -2, "attenuation_linear");

    lua_pushnumber(L, light->mAttenuationQuadratic);
    lua_setfield(L, -2, "attenuation_quadratic");

    return 1;
}

int convert(lua_State *L, const aiCamera *camera) {
    lua_newtable(L);

    lua_pushlstring(L, camera->mName.data, camera->mName.length);
    lua_setfield(L, -2, "node_name");

    convert(L, &camera->mPosition);
    lua_setfield(L, -2, "position");

    convert(L, &camera->mLookAt);
    lua_setfield(L, -2, "forward");

    convert(L, &camera->mUp);
    lua_setfield(L, -2, "up");

    lua_pushnumber(L, camera->mAspect);
    lua_setfield(L, -2, "aspect");

    bool isOrthographic = camera->mOrthographicWidth != 0.0f;
    if (isOrthographic) {
        lua_pushnumber(L, camera->mOrthographicWidth);
        lua_setfield(L, -2, "fov");
    } else {
        lua_pushnumber(L, camera->mHorizontalFOV);
        lua_setfield(L, -2, "fov");
    }

    lua_pushboolean(L, isOrthographic);
    lua_setfield(L, -2, "orthographic");

    lua_pushnumber(L, camera->mClipPlaneNear);
    lua_setfield(L, -2, "nearclip");

    lua_pushnumber(L, camera->mClipPlaneFar);
    lua_setfield(L, -2, "farclip");

    return 1;
}

int convert(lua_State *L, const aiMetadata *metadata) {
    if (metadata == nullptr) {
        lua_pushnil(L);
        return 1;
    }
    lua_newtable(L);
    for (unsigned int i = 0; i < metadata->mNumProperties; i++) {
        const aiString *key = metadata->mKeys + i;
        lua_pushlstring(L, key->data, key->length);
        convert(L, metadata->mValues + i);
        lua_settable(L, -3);
    }
    return 1;
}

int convert(lua_State *L, const aiMetadataEntry *entry) {
    if (entry->mData == nullptr) {
        lua_pushnil(L);
        return 1;
    }
    switch (entry->mType) {
        case aiMetadataType::AI_BOOL:
            lua_pushboolean(L, *((bool *)entry->mData));
            break;
        case aiMetadataType::AI_INT32:
            lua_pushinteger(L, *((int32_t *)entry->mData));
            break;
        case aiMetadataType::AI_UINT64:
            lua_pushinteger(L, *((uint64_t *)entry->mData));
            break;
        case aiMetadataType::AI_FLOAT:
            lua_pushnumber(L, *((float *)entry->mData));
            break;
        case aiMetadataType::AI_DOUBLE:
            lua_pushnumber(L, *((double *)entry->mData));
            break;
        case aiMetadataType::AI_AISTRING:
            lua_pushlstring(L, ((const aiString *)entry->mData)->data, ((const aiString *)entry->mData)->length);
            break;
        case aiMetadataType::AI_AIVECTOR3D:
            convert(L, (const aiVector3D *)entry->mData);
            break;
        case aiMetadataType::AI_AIMETADATA:
            convert(L, (const aiMetadata *)entry->mData);  // recursion, yay!
            break;
        default:
            lua_pushnil(L);
            break;
    }
    return 1;
}

int convert(lua_State *L, const aiMatrix4x4 *mat4) {
    lua_Number elems[] = {
        mat4->a1, mat4->b2, mat4->a3, mat4->a4,
        mat4->b1, mat4->b2, mat4->b3, mat4->b4,
        mat4->c1, mat4->b2, mat4->c3, mat4->c4,
        mat4->d1, mat4->b2, mat4->d3, mat4->d4};
    create_transform(L, elems);
    return 1;
}

int convert(lua_State *L, const aiMatrix3x3 *mat3) {
    lua_Number elems[] = {
        mat3->a1, mat3->b2, mat3->a3, 0,
        mat3->b1, mat3->b2, mat3->b3, 0,
        mat3->c1, mat3->b2, mat3->c3, 0,
        0, 0, 0, 1};
    create_transform(L, elems);
    return 1;
}

int convert(lua_State *L, const aiVector3D *vec3) {
    lua_newtable(L);
    lua_pushinteger(L, 1);
    lua_pushnumber(L, vec3->x);
    lua_settable(L, -3);
    lua_pushinteger(L, 2);
    lua_pushnumber(L, vec3->y);
    lua_settable(L, -3);
    lua_pushinteger(L, 3);
    lua_pushnumber(L, vec3->z);
    lua_settable(L, -3);
    return 1;
}

int convert(lua_State *L, const aiVector2D *vec2) {
    lua_newtable(L);
    lua_pushinteger(L, 1);
    lua_pushnumber(L, vec2->x);
    lua_settable(L, -3);
    lua_pushinteger(L, 2);
    lua_pushnumber(L, vec2->y);
    lua_settable(L, -3);
    return 1;
}

int convert(lua_State *L, const aiQuaternion *quat) {
    lua_newtable(L);
    lua_pushinteger(L, 1);
    lua_pushnumber(L, quat->x);
    lua_settable(L, -3);
    lua_pushinteger(L, 2);
    lua_pushnumber(L, quat->y);
    lua_settable(L, -3);
    lua_pushinteger(L, 3);
    lua_pushnumber(L, quat->z);
    lua_settable(L, -3);
    lua_pushinteger(L, 4);
    lua_pushnumber(L, quat->w);
    lua_settable(L, -3);
    return 1;
}

int convert(lua_State *L, const aiColor4D *col4) {
    lua_newtable(L);
    lua_pushinteger(L, 1);
    lua_pushnumber(L, col4->r);
    lua_settable(L, -3);
    lua_pushinteger(L, 2);
    lua_pushnumber(L, col4->g);
    lua_settable(L, -3);
    lua_pushinteger(L, 3);
    lua_pushnumber(L, col4->b);
    lua_settable(L, -3);
    lua_pushinteger(L, 4);
    lua_pushnumber(L, col4->a);
    lua_settable(L, -3);
    return 1;
}

int convert(lua_State *L, const aiColor3D *col3) {
    lua_newtable(L);
    lua_pushinteger(L, 1);
    lua_pushnumber(L, col3->r);
    lua_settable(L, -3);
    lua_pushinteger(L, 2);
    lua_pushnumber(L, col3->g);
    lua_settable(L, -3);
    lua_pushinteger(L, 3);
    lua_pushnumber(L, col3->b);
    lua_settable(L, -3);
    return 1;
}

} // namespace AssimpToLove