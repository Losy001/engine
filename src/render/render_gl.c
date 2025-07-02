#include <render/render.h>

// utility

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

typedef struct 
{
	u16vec2 size;
	uint32_t id;
} Texture;

typedef struct 
{
	uint32_t id;
} Mesh;

typedef struct 
{
	WindowId window;

	#if defined _WIN32
		HDC dc;
		HGLRC glrc;
	#endif
} RenderContext;

static FreeList(Texture, MAX_TEXTURES) textures = fl_default();
static FreeList(Mesh, MAX_MESHES) meshes = fl_default();
static FreeList(RenderContext, MAX_RENDER_CONTEXTS) render_contexts = fl_default();

//
// texture
//

static inline TextureId create_texture(uint32_t width, uint32_t height, uint32_t *pixels, TextureFormat format)
{
	TextureId id = fl_push(&textures, (Texture){});
	Texture* t = fl_get(&textures, id);

	t->size = (u16vec2){ width, height };

	glGenTextures(1, &t->id);
	glBindTexture(GL_TEXTURE_2D, t->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return id;
}

static inline void destroy_texture(TextureId texture)
{
	const Texture* t = fl_get(&textures, texture);

	glDeleteTextures(1, &t->id);

	fl_remove(&textures, texture);
}

static inline void use(TextureId texture)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fl_get(&textures, texture)->id);
}

//
// mesh
//

static inline MeshId create_mesh(const float* xyz, const float* uv, size_t vertex_count, uint32_t* indices, size_t index_count)
{
	MeshId id = fl_push(&meshes, (Mesh){});
	Mesh* m = fl_get(&meshes, id);

	m->id = glGenLists(1);

	glNewList(m->id, GL_COMPILE);
	
	glColor3f(1.0f, 1.0f, 1.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, xyz);
	glTexCoordPointer(2, GL_FLOAT, 0, uv);

	if (indices)
	{
		glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, indices);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, vertex_count);
	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glEndList();

	return id;
}

static inline void render(MeshId mesh, TextureId texture, mat4 transform)
{
	use(texture);

	mul(GL_MODELVIEW, transform);

	glCallList(fl_get(&meshes, mesh)->id);
}

//
// render context
//

#if defined _WIN32
	typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALPROC)(int);
	static PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT;
#endif

static inline RenderContextId create_context(WindowId w)
{
	RenderContextId id = fl_push(&render_contexts, (RenderContext){});
	RenderContext* rc = fl_get(&render_contexts, id);

	rc->window = w;

	#if defined _WIN32
		rc->dc = GetDC(get_native_handle(w));

		PIXELFORMATDESCRIPTOR desc = {};

		desc.nSize = sizeof(desc);
		desc.nVersion	= 1;
		desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		desc.iPixelType = PFD_TYPE_RGBA;
		desc.cColorBits = 24;
		desc.cAlphaBits = 8;
		desc.cDepthBits = 32;
	
		SetPixelFormat(rc->dc, ChoosePixelFormat(rc->dc, &desc), &desc);
	
		rc->glrc = wglCreateContext(rc->dc);
		wglMakeCurrent(rc->dc, rc->glrc);
	
		// TODO: move this somewhere else?
		if (!wglSwapIntervalEXT)
		{
			wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");
			wglSwapIntervalEXT(1);
		}
	#endif

	glEnable(GL_DEPTH_TEST);

	return id;
}

static inline void destroy_context(RenderContextId render_context)
{
	const RenderContext* rc = fl_get(&render_contexts, render_context);
	
	#if defined _WIN32
		wglDeleteContext(rc->glrc);
	#endif

	fl_remove(&render_contexts, render_context);
}

static inline void begin(RenderContextId render_context)
{
	const RenderContext* rc = fl_get(&render_contexts, render_context);

	#if defined _WIN32
		wglMakeCurrent(rc->dc, rc->glrc);
	#endif

	const u16vec2 size = get_size(rc->window);

	glViewport(0, 0, size.x, size.y);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	load(GL_PROJECTION, ortho(0.0f, size.x, size.y, 0.0f, -1.0f, 1.0f));
}

static inline void end(RenderContextId render_context)
{
	const RenderContext* rc = fl_get(&render_contexts, render_context);

	#if defined _WIN32
		SwapBuffers(rc->dc);
		wglMakeCurrent(nullptr, nullptr);
	#endif
}

static inline void begin_3d(RenderContextId render_context, const Camera* camera)
{
	const RenderContext* rc = fl_get(&render_contexts, render_context);

	const u16vec2 size = get_size(rc->window);

	load(GL_PROJECTION, perspective(camera->fov, (float)size.x / (float)size.y, 0.01f));
	load(GL_MODELVIEW, rotate_x(camera->pitch) * rotate_y(camera->yaw) * translate(-camera->position));
}

static inline void end_3d(RenderContextId render_context)
{
	const RenderContext* rc = fl_get(&render_contexts, render_context);
	
	const u16vec2 size = get_size(rc->window);	
	
	load(GL_PROJECTION, ortho(0.0f, size.x, size.y, 0.0f, -1.0f, 1.0f));
}