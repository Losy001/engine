#include <core/log.h>
#include <core/math.h>

#include <window/window.h>
#include <render/render.h>
#include <platform/platform.h>

static WindowId window;
static RenderContextId rc;

static ResourceId vb;

static inline void loop()
{
	Camera* camera = get_camera(rc);

	do_once
	(
		camera->fov = 75.0f; 
		camera->position = (vec3){ 0.0f, 1.0f, 0.0f };
	);

	float sy = sinf(camera->yaw);
	float cy = cosf(camera->yaw);
			
	bool key_forward = is_key_down(window, 'W');
	bool key_back = is_key_down(window, 'S');
	bool key_left = is_key_down(window, 'A');
	bool key_right = is_key_down(window, 'D');
		
	vec3 velocity = {};
		
	if (key_left && !key_right)
	{
		velocity.xz = (vec2){ -cy, -sy };
	}
	else if (key_right && !key_left)
	{
		velocity.xz = (vec2){ cy, sy };
	}
		
	if (key_forward && !key_back)
	{
		velocity.xz += (vec2){ -sy, cy };
	}
	else if (key_back && !key_forward)
	{
		velocity.xz += (vec2){ sy, -cy };
	}
		
	if (is_key_down(window, VK_SPACE)) 
	{
		velocity.y = 1.0f;
	}
	if (is_key_down(window, VK_SHIFT)) 
	{
		velocity.y = -1.0f;
	}
		
	camera->position += velocity * 0.1f;
	
	const i16vec2 delta = get_mouse_delta(window);
		
	camera->yaw -= radians(delta.x * 0.1f);
	camera->pitch -= radians(delta.y * 0.1f);
	
	camera->yaw = fmodf(camera->yaw, TAU);
	camera->pitch = clamp(camera->pitch, radians(-89.0f), radians(89.0f));

	static int x = 0;

	x += 1;

	/*if (x % 10 == 0)
	{
		uint32_t pixels[] =
		{
			0xFFFF00FF, 0xFFFFFFFF,	0xFFFF00FF, 0xFFFFFFFF,
			0xFFFFFFFF, 0xFFFF00FF,	0xFFFFFFFF, 0xFFFF00FF,
			0xFFFF00FF, 0xFFFFFFFF,	0xFFFF00FF, 0xFFFFFFFF,
			0xFFFFFFFF, 0xFFFF00FF,	0xFFFFFFFF, 0xFFFF00FF,
		};

		update(texture, 4, 4, pixels, 0);
	}
	else
	{
		uint32_t pixels[] =
		{
			0xFFFF00FF, 0xFFFFFFFF,
			0xFFFFFFFF, 0xFFFF00FF,
		};

		update(texture, 2, 2, pixels, 0);
	}*/

	poll_events(window);

	begin(rc);

	clear_background((vec4){ 0.2f, 0.3f, 0.3f, 1.0f });

	begin_3d(rc);

	render(vb, rotate_x(radians(90.0f)));
	render(vb, rotate_x(radians(90.0f)) * translate(5.0f));
	
	end_3d();

	render(vb, identity());

	end();
}

static inline void on_resize(WindowId window) 
{
	resize_viewport(rc);
	loop();
}

static inline void entry()
{
	window = create_window(1280, 720, "hello world");
	rc = create_context(window);

	set_on_resize(window, on_resize);
	
	Vertex vertices[] =
	{ 
		{ 0.0f,  0.0f,  0.0f, 0.0f, 0.0f, },
		{ 0.0f,  50.0f, 0.0f, 0.0f, 1.0f, },
		{ 50.0f, 50.0f, 0.0f, 1.0f, 1.0f, },
		{ 50.0f, 50.0f, 0.0f, 1.0f, 1.0f, },
		{ 50.0f, 0.0f,  0.0f, 1.0f, 0.0f, },
		{ 0.0f,  0.0f,  0.0f, 0.0f, 0.0f  },
	};

	vb = create_vertex_buffer(vertices, count_of(vertices));

	while (is_open(window))
	{	
		loop();
	}

	die(0);
}

#if defined _WIN32
	#include <window/window_win32.c>
	#include <render/render_gl1.c>
	#include <platform/platform_win32.c>
#endif