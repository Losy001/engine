
#include <window/window.h>
#include <render/render.h>
#include <platform/platform.h>

void entry()
{
	WindowId w = create_window(1280, 720, "hello world");
	RenderContextId r = create_context(w);

	MeshId m;

	{
		float xyz[] =
		{ 
			0.0f,  0.0f,   0.0f,
			50.0f, 50.0f,  0.0f,
			0.0f,  100.0f, 0.0f  
		};

		float uv[] = 
		{
			0.0f, 0.0f, 
			0.0f, 0.0f,
			0.0f, 0.0f
		};

		m = create_mesh(xyz, uv, 3, nullptr, 0);
	}

	while (is_open(w))
	{	
		poll_events(w);

		begin(r);
	
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, get_width(w), get_height(w), 0, -1, 1);
	
		render(m);
	
		end(r);
	}

	die(0);
}

#ifdef _WIN32
	#include <window/window_win32.c>
	#include <render/render_wgl.c>
	#include <platform/platform_win32.c>
#endif