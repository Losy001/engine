#pragma once

#include <core/core.h>

#include <window/window.h>

// resources

typedef struct 
{
	float x, y, z;
	float u, v;
} Vertex;

typedef enum ResourceId : uint16_t ResourceId;

[[clang::overloadable]]
static inline ResourceId create_vertex_buffer(Vertex* vertices, size_t count);

[[clang::overloadable]]
static inline ResourceId create_index_buffer(uint32_t* indices, size_t count);

[[clang::overloadable]]
static inline ResourceId create_texture(uint16_t width, uint16_t height, uint32_t* data);

// context 

typedef struct 
{
	vec3 position;
	float pitch, yaw;
	float fov;
} Camera;

typedef enum RenderContextId : uint8_t RenderContextId; 

[[clang::overloadable]]
static inline RenderContextId create_context(WindowId window);

[[clang::overloadable]]
static inline void destroy_context(RenderContextId render_context);

[[clang::overloadable]]
static inline void clear_background(vec4 color);

[[clang::overloadable]]
static inline void bind_texture(ResourceId texture);

[[clang::overloadable]]
static inline void begin(RenderContextId render_context);

[[clang::overloadable]]
static inline void end();

[[clang::overloadable]]
static inline void begin_3d(RenderContextId render_context);

[[clang::overloadable]]
static inline void end_3d();

[[clang::overloadable]]
static inline void resize_viewport(RenderContextId render_context);

[[clang::overloadable]]
static inline void render_indexed(ResourceId vertex_buffer, ResourceId index_buffer, mat4 transform);

[[clang::overloadable]]
static inline void render(ResourceId vertex_buffer, mat4 transform);

[[clang::overloadable]]
static inline Camera* get_camera(RenderContextId render_context);

[[clang::overloadable]]
static inline RenderContextId get_current_context();