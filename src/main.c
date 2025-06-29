
#include <window/window.h>
#include <render/render.h>
#include <platform/platform.h>

static inline void draw(RenderContextId r, WindowId w)
{
	poll_events(w);

	const uint16_t width = get_width(w);
	const uint16_t height = get_height(w);

	begin(r);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_TRIANGLES);

	glColor4ub(255, 255, 255, 255);
	glVertex2i(get_mouse_x(w), get_mouse_y(w));
	glColor4ub(255, 0, 255, 255);
	glVertex2i(100, 50);
	glColor4ub(255, 255, 0, 255);
	glVertex2i(75, 25);

	glEnd();

	end(r);
}

void entry()
{
	WindowId w1 = create_window(1280, 720, "hello world");
	RenderContextId r1 = create_context(w1);

	WindowId w2 = create_window(1280, 720, "goodbye moon men");
	RenderContextId r2 = create_context(w2);

	while (is_open(w1))
	{
		draw(r2, w2);
		draw(r1, w1);
	}

	die(0);
}

#ifdef _WIN32
	#include <window/window_win32.c>
	#include <render/render_wgl.c>
	#include <platform/platform_win32.c>
#endif