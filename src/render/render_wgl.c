#include <render/render.h>

//
// texture
//

static inline TextureId create_texture(uint32_t width, uint32_t height, uint32_t* pixels, TextureFormat format)
{

}

static inline void destroy_texture(TextureId t)
{

}

//
// mesh
//

typedef struct 
{
	uint32_t id;
} Mesh;

static FreeList(Mesh, MAX_MESHES) meshes = fl_default();

static inline MeshId create_mesh(const float* xyz, const float* uv, size_t vertex_count, uint32_t* indices, size_t index_count)
{
	MeshId id = fl_push(&meshes, (Mesh){});
	Mesh* m = fl_get(&meshes, id);

	m->id = glGenLists(1);

	glNewList(m->id, GL_COMPILE);
	
	glPushMatrix();
	
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

	glPopMatrix();

	glEndList();

	return id;
}

static inline void render(MeshId m)
{
	glCallList(fl_get(&meshes, m)->id);
}

//
// render context
//

typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALPROC)(int);
static PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT;

typedef struct 
{
	WindowId window;
	HDC dc;
	HGLRC glrc;
} RenderContext;

static FreeList(RenderContext, MAX_RENDER_CONTEXTS) render_contexts = fl_default();

static inline RenderContextId create_context(WindowId w)
{
	RenderContextId id = fl_push(&render_contexts, (RenderContext){});
	RenderContext* r = fl_get(&render_contexts, id);

	r->window = w;
	r->dc = GetDC(get_native_handle(w));

	PIXELFORMATDESCRIPTOR desc = {};

	desc.nSize = sizeof(desc);
	desc.nVersion	= 1;
	desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	desc.iPixelType = PFD_TYPE_RGBA;
	desc.cColorBits = 24;
	desc.cAlphaBits = 8;
	desc.cDepthBits = 32;

	SetPixelFormat(r->dc, ChoosePixelFormat(r->dc, &desc), &desc);

	r->glrc = wglCreateContext(r->dc);
	wglMakeCurrent(r->dc, r->glrc);

	// TODO: move this somewhere else?
	if (!wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");
		wglSwapIntervalEXT(1);
	}

	glEnable(GL_DEPTH_TEST);

	return id;
}

static inline void destroy_context(RenderContextId r)
{
	const RenderContext* render_context = fl_get(&render_contexts, r);
	
	wglDeleteContext(render_context->glrc);
	fl_remove(&render_contexts, r);
}

static inline void begin(RenderContextId r)
{
	const RenderContext* rc = fl_get(&render_contexts, r);

	wglMakeCurrent(rc->dc, rc->glrc);

	glViewport(0, 0, get_width(rc->window), get_height(rc->window));
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static inline void end(RenderContextId r)
{
	const RenderContext* rc = fl_get(&render_contexts, r);

	SwapBuffers(rc->dc);
	wglMakeCurrent(nullptr, nullptr);
}
