#pragma once

#include <core/core.h>
#include <core/math.h>

#include <utils/free_list.h>
#include <utils/macros.h>

#include <window/window.h>

//
// texture
//

static constexpr uint16_t MAX_TEXTURES = 128;

typedef enum : uint8_t
{
	TEXTURE_FORMAT_RGBA
} TextureFormat;

typedef enum TextureId : uint16_t TextureId;

static inline TextureId create_texture(uint32_t width, uint32_t height, uint32_t* pixels, TextureFormat format);

static inline void destroy_texture(TextureId t);

//
// mesh
//

static constexpr uint16_t MAX_MESHES = 64;
static constexpr uint16_t MAX_VERTICES_PER_MESH = 4096;
static constexpr uint16_t MAX_INDICES_PER_MESH = 2048; 

typedef enum MeshId : uint16_t MeshId;

static inline MeshId create_mesh(const float* xyz, const float* uv, size_t vertex_count, uint32_t* indices, size_t index_count);

static inline void render(MeshId m);

//
// render context
//

static constexpr uint16_t MAX_RENDER_CONTEXTS = 4;

typedef enum RenderContextId : uint16_t RenderContextId;

static inline RenderContextId create_context(WindowId w);

static inline void destroy_context(RenderContextId r);

static inline void begin(RenderContextId r);

static inline void end(RenderContextId r);
