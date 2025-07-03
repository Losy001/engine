#pragma once

#include <core/core.h>

#include <render/texture.h>

static constexpr uint16_t MAX_MESHES = 64;
 
typedef enum MeshId : uint16_t MeshId;

[[clang::overloadable]]
static inline MeshId create_mesh(const float* xyz, const float* uv, size_t vertex_count, uint32_t* indices, size_t index_count);

[[clang::overloadable]]
static inline void render(MeshId mesh, TextureId texture, vec4 tint, mat4 transform);
