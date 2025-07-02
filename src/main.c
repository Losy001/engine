
#include <window/window.h>
#include <render/render.h>
#include <platform/platform.h>

#include <core/log.h>

static inline void entry()
{
	WindowId window = create_window(1280, 720, "hello world");
	RenderContextId rc = create_context(window);

	float xyz[] =
	{ 
		0.0f,  0.0f,  0.0f,
		0.0f,  50.0f, 0.0f,
		50.0f, 50.0f, 0.0f,
		50.0f, 50.0f, 0.0f,
		50.0f, 0.0f,  0.0f,
		0.0f,  0.0f,  0.0f,
	};

	float uv[] = 
	{
		0.0f, 0.0f, 
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f
	};

	MeshId mesh = create_mesh(xyz, uv, 6, nullptr, 0);

	uint32_t pixels[] =
	{
		0xFFFF00FF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFF00FF
	};

	TextureId texture = create_texture(2, 2, pixels, TEXTURE_FORMAT_RGBA);

	Camera camera = { .fov = 75.0f, .position = { 0.0f, 1.0f, 0.0f } };

	float x = 0.0f;

	while (is_open(window))
	{	
		x += 0.01f;

		// camera movement
		float sy = sinf(camera.yaw);
		float cy = cosf(camera.yaw);
				
		bool key_forward = is_key_down('W');
		bool key_back = is_key_down('S');
		bool key_left = is_key_down('A');
		bool key_right = is_key_down('D');
		
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
		
		if (is_key_down(VK_SPACE)) 
		{
			velocity.y = 1.0f;
		}
		if (is_key_down(VK_SHIFT)) 
		{
			velocity.y = -1.0f;
		}
		
		camera.position += velocity * 0.1f;
	
		const i16vec2 delta = get_mouse_delta(window);
		
		camera.yaw -= radians(delta.x * 0.1f);
		camera.pitch -= radians(delta.y * 0.1f);
	
		camera.yaw = fmodf(camera.yaw, TAU);
		camera.pitch = clamp(camera.pitch, radians(-89.0f), radians(89.0f));

		poll_events(window);

		begin(rc);

		begin_3d(rc, camera);

		render(mesh, texture, rotate_x(radians(90.0f)));
		render(mesh, texture, rotate_x(radians(90.0f)) * translate(5.0f));
	
		end_3d(rc);

		render(mesh, texture, identity());

		end(rc);
	}

	die(0);
}

#if defined _WIN32
	#include <window/window_win32.c>
	#include <render/render_gl.c>
	#include <platform/platform_win32.c>
#endif