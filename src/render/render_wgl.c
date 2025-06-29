#include <render/render.h>

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
	RenderContext r = {};

	r.window = w;
	r.dc = GetDC(get_native_handle(w));

	PIXELFORMATDESCRIPTOR desc = {};

	desc.nSize = sizeof(desc);
	desc.nVersion	= 1;
	desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	desc.iPixelType = PFD_TYPE_RGBA;
	desc.cColorBits = 24;
	desc.cAlphaBits = 8;
	desc.cDepthBits = 32;

	SetPixelFormat(r.dc, ChoosePixelFormat(r.dc, &desc), &desc);

	r.glrc = wglCreateContext(r.dc);
	wglMakeCurrent(r.dc, r.glrc);

	if (!wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");
		wglSwapIntervalEXT(1);
	}

	return fl_push(&render_contexts, r);
}

static inline void begin(RenderContextId r)
{
	const RenderContext* render_context = fl_get(&render_contexts, r);

	wglMakeCurrent(render_context->dc, render_context->glrc);
	glViewport(0, 0, get_width(render_context->window), get_height(render_context->window));
}

static inline void end(RenderContextId r)
{
	const RenderContext* render_context = fl_get(&render_contexts, r);

	SwapBuffers(fl_get(&render_contexts, r)->dc);
	wglMakeCurrent(nullptr, nullptr);
}