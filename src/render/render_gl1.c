
#include <render/render.h>

#include <core/math.h>

#include <utils/free_list.h>
#include <utils/macros.h>

#define PENGOL_IMPL
#include <utils/pl_gl.h>

// utility functions

static inline void load(GLenum mode, mat4 m)
{
	glMatrixMode(mode);
	glPushMatrix();
	glLoadMatrixf((const float*)&m);
}

static inline void mul(GLenum mode, mat4 m)
{
	glMatrixMode(mode);
	glPushMatrix();
	glMultMatrixf((const float*)&m);
}

// resources

static constexpr uint16_t MAX_VERTEX_BUFFERS = 1024;
static constexpr uint16_t MAX_VERTICES = 4096;

typedef struct 
{
	uint32_t id;
	uint32_t count;
} VertexBuffer;

static FreeList(VertexBuffer, MAX_VERTEX_BUFFERS) vertex_buffers = fl_default();

[[clang::overloadable]]
static inline ResourceId create_vertex_buffer(Vertex* vertices, size_t count)
{
	ResourceId id = fl_push(&vertex_buffers, (VertexBuffer){});
	VertexBuffer* vb = fl_get(&vertex_buffers, id);

	vb->count = count;

	glGenBuffers(1, &vb->id);
	glBindBuffer(GL_ARRAY_BUFFER, vb->id);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(count * sizeof(Vertex)), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return id;
}

static constexpr uint16_t MAX_INDEX_BUFFERS = MAX_VERTEX_BUFFERS;

typedef struct 
{
	uint32_t id;
	uint32_t count;
} IndexBuffer;

static FreeList(IndexBuffer, MAX_INDEX_BUFFERS) index_buffers = fl_default();

[[clang::overloadable]]
static inline ResourceId create_index_buffer(uint32_t* indices, size_t count)
{
	ResourceId id = fl_push(&index_buffers, (IndexBuffer){});
	IndexBuffer* ib = fl_get(&index_buffers, id);

	ib->count = count;

	glGenBuffers(1, &ib->id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(count * sizeof(uint32_t)), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return id;
}

static constexpr uint16_t MAX_TEXTURES = 512;

typedef struct 
{
	uint32_t id;
	u16vec2 size;
} Texture;

static FreeList(Texture, MAX_TEXTURES) textures = fl_default();

[[clang::overloadable]]
static inline ResourceId create_texture(uint16_t width, uint16_t height, const uint8_t* data, size_t size, TextureFormat format)
{
	ResourceId id = fl_push(&textures, (Texture){});
	Texture* t = fl_get(&textures, id);

	t->size = (u16vec2){ width, height };

	glGenTextures(1, &t->id);
	glBindTexture(GL_TEXTURE_2D, t->id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	switch (format) 
	{
		case TEXTURE_FORMAT_RGBA:
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		} break;

		case TEXTURE_FORMAT_DXT5:
		{
			// GL_COMPRESSED_RGBA_S3TC_DXT5_EXT = 0x83F3
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, 0x83F3, width, height, 0, size, data);
		} break;
	}

	return id;
}

// context


#if defined _WIN32
	typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALPROC)(int);
	static PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT;
#endif

static constexpr uint8_t MAX_RENDER_CONTEXTS = MAX_WINDOWS;

typedef struct 
{
	WindowId window;

	#if defined _WIN32
		HDC dc;
		HGLRC glrc;
	#endif

	Camera camera;
	bool is_3d;
} RenderContext;

static RenderContextId ctx = -1;

static FreeList(RenderContext, MAX_RENDER_CONTEXTS) render_contexts = fl_default();

[[clang::overloadable]]
static inline RenderContextId create_context(WindowId window)
{
	RenderContextId id = fl_push(&render_contexts, (RenderContext){});
	RenderContext* rc = fl_get(&render_contexts, id);

	rc->window = window;

	#if defined _WIN32
		rc->dc = GetDC(get_native_handle(rc->window));

		PIXELFORMATDESCRIPTOR desc = {};

		desc.nSize = sizeof(desc);
		desc.nVersion	= 1;
		desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		desc.iPixelType = PFD_TYPE_RGBA;
	
		SetPixelFormat(rc->dc, ChoosePixelFormat(rc->dc, &desc), &desc);
	
		rc->glrc = wglCreateContext(rc->dc);
		wglMakeCurrent(rc->dc, rc->glrc);
	
		do_once
		(
			wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");
			wglSwapIntervalEXT(1);
		
			pl_load_gl();
		);
	#endif

	return id;
}

[[clang::overloadable]]
static inline void destroy_context(RenderContextId render_context)
{
	const RenderContext* rc = fl_get(&render_contexts, render_context);
		
	#if defined _WIN32
		wglDeleteContext(rc->glrc);
		ReleaseDC(get_native_handle(rc->window), rc->dc);
	#endif

	fl_remove(&render_contexts, render_context);
}

[[clang::overloadable]]
static inline void clear_background(vec4 color)
{
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

[[clang::overloadable]]
static inline void use(ResourceId texture)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fl_get(&textures, texture)->id);
}

[[clang::overloadable]]
static inline void begin(RenderContextId render_context)
{
	ctx = render_context;

	const RenderContext* rc = fl_get(&render_contexts, ctx);

	#if defined _WIN32
		wglMakeCurrent(rc->dc, rc->glrc);
	#endif

	const u16vec2 size = get_size(rc->window);

	glViewport(0, 0, size.x, size.y);

	load(GL_PROJECTION, ortho(0.0f, size.x, size.y, 0.0f, -1.0f, 1.0f));
	load(GL_MODELVIEW, identity());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_TEXTURE_2D);
}

[[clang::overloadable]]
static inline void end()
{
	if (ctx == (RenderContextId)-1)
	{	
		LOG_FATAL("end: invalid context");
	}

	const RenderContext* rc = fl_get(&render_contexts, ctx);

	#if defined _WIN32
		SwapBuffers(rc->dc);
	#endif

	ctx = (RenderContextId)-1;
}

[[clang::overloadable]]
static inline void begin_3d(RenderContextId render_context)
{
	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_GREATER);
	glClearDepth(0.0f);

	RenderContext* rc = fl_get(&render_contexts, ctx);

	rc->is_3d = true;

	const u16vec2 size = get_size(rc->window);

	load(GL_PROJECTION, perspective(rc->camera.fov, (float)size.x / (float)size.y, 0.01f));
	load(GL_MODELVIEW, rotate_x(rc->camera.pitch) * rotate_y(rc->camera.yaw) * translate(-rc->camera.position));
}

[[clang::overloadable]]
static inline void end_3d()
{
	RenderContext* rc = fl_get(&render_contexts, ctx);

	if (!rc->is_3d)
	{	
		LOG_FATAL("end_3d: missing begin_3d");
	}

	rc->is_3d = false;

	const u16vec2 size = get_size(rc->window);	
	
	load(GL_PROJECTION, ortho(0.0f, size.x, size.y, 0.0f, -1.0f, 1.0f));
	load(GL_MODELVIEW, identity());
}

[[clang::overloadable]]
static inline void resize_viewport(RenderContextId render_context)
{
	const RenderContext* rc = fl_get(&render_contexts, render_context);
	const u16vec2 size = get_size(rc->window);

	glViewport(0, 0, size.x, size.y);

	if (rc->is_3d)
	{
		load(GL_PROJECTION, perspective(rc->camera.fov, (float)size.x / (float)size.y, 0.01f));
	}
	else
	{
		load(GL_PROJECTION, ortho(0.0f, size.x, size.y, 0.0f, -1.0f, 1.0f));
	}
}

[[clang::overloadable]]
static inline void render_indexed(ResourceId vertex_buffer, ResourceId index_buffer, mat4 transform)
{
	const VertexBuffer* vb = fl_get(&vertex_buffers, vertex_buffer);

	glBindBuffer(GL_ARRAY_BUFFER, vb->id);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)(__builtin_offsetof(Vertex, u)));

	const IndexBuffer* ib = fl_get(&index_buffers, index_buffer);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id);

	const RenderContext* rc = fl_get(&render_contexts, ctx);

	mat4 m;

	if (rc->is_3d) 
	{
		m = rotate_x(rc->camera.pitch) 
			* rotate_y(rc->camera.yaw) 
			* translate(-rc->camera.position)
			* transform;
	}
	else 
	{
		m = transform;
	}

	load(GL_MODELVIEW, m);

	glDrawElements(GL_TRIANGLES, ib->count, GL_UNSIGNED_INT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

[[clang::overloadable]]
static inline void render(ResourceId vertex_buffer, mat4 transform)
{
	const VertexBuffer* vb = fl_get(&vertex_buffers, vertex_buffer);

	glBindBuffer(GL_ARRAY_BUFFER, vb->id);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)(__builtin_offsetof(Vertex, u)));
	
	const RenderContext* rc = fl_get(&render_contexts, ctx);

	mat4 m;

	if (rc->is_3d) 
	{
		m = rotate_x(rc->camera.pitch) 
			* rotate_y(rc->camera.yaw) 
			* translate(-rc->camera.position)
			* transform;
	}
	else 
	{
		m = transform;
	}

	load(GL_MODELVIEW, m);

	glDrawArrays(GL_TRIANGLES, 0, vb->count);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

[[clang::overloadable]]
static inline Camera* get_camera(RenderContextId render_context)
{
	return &fl_get(&render_contexts, render_context)->camera;
}

[[clang::overloadable]]
static inline RenderContextId get_current_context()
{
	return ctx;
}