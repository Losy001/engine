#pragma once

#include <core/core.h>

#include <render/texture.h>

static constexpr uint16_t MAX_MESHES = 64;
 
typedef enum MeshId : uint16_t MeshId;

static inline MeshId create_mesh(const float* xyz, const float* uv, size_t vertex_count, uint32_t* indices, size_t index_count);

static inline void render(MeshId mesh, TextureId texture, mat4 transform);
