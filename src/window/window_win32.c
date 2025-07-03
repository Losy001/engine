#include <window/window.h>

typedef struct 
{
	HWND handle;

	u16vec2 size;
	i16vec2 mouse;
	i16vec2 mouse_delta;
	
	WindowResizeCb on_resize;
	WindowPaintCb on_paint;

	bool open;
} Window;

static FreeList(Window, MAX_WINDOWS) windows = fl_default();

static inline __stdcall LRESULT wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
{
	Window* w = (Window*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

	if (!w)
	{
		return DefWindowProcA(hwnd, msg, wparam, lparam);
	}

	switch (msg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}	break;

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			DestroyWindow(hwnd);
			w->open = false;
		} break;

		case WM_ERASEBKGND:
		{
			return 1;
		} break;

		case WM_MOUSEMOVE:
		{
			w->mouse = (i16vec2){ GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
		} break;

		case WM_INPUT:
		{
			uint8_t raw_data[sizeof(RAWINPUT)];
			uint32_t size = 0;

			GetRawInputData((HRAWINPUT)lparam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

			if (size > 0)
			{
				if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, raw_data, &size, sizeof(RAWINPUTHEADER)) == size)
				{
					RAWINPUT* raw_input = (RAWINPUT*)raw_data;

					if (raw_input->header.dwType == RIM_TYPEMOUSE)
					{
						w->mouse_delta = (i16vec2){ raw_input->data.mouse.lLastX, raw_input->data.mouse.lLastY };
					}
				}
			}
		} break;

		case WM_SIZE:
		{
			w->size = (u16vec2){ LOWORD(lparam), HIWORD(lparam) };
			
			if (w->on_resize)
			{
				w->on_resize(w->size.x, w->size.y);
			}

			PostMessageA(hwnd, WM_PAINT, 0, 0);
		};

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			
			if (w->on_paint)
			{
				w->on_paint();
			}
			
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
		} break;

		default:
		{
			return DefWindowProcA(hwnd, msg, wparam, lparam);
		} break;
	}

	return 0;
}

static WNDCLASSA class;

static inline WindowId create_window(uint16_t width, uint16_t height, const char* title)
{
	do_once 
	(
		class.lpfnWndProc = wnd_proc;
		class.hInstance = GetModuleHandleA(nullptr);
		class.lpszClassName = "O_o";
		class.hCursor = LoadCursorA(nullptr, IDC_ARROW);
		
		if (!RegisterClassA(&class))
		{
			char err[512];
			get_error_msg(get_last_error(), err, 512);

			LOG_FATAL("class registration failure.\nreason: %s", err);
		}
	);

	WindowId id = fl_push(&windows, (Window){});
	Window* w = fl_get(&windows, id);
	
	w->size = (u16vec2){ width, height };

	w->open = true;
	w->handle = CreateWindowA(class.lpszClassName, title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, w->size.x, w->size.y, nullptr, nullptr, nullptr, nullptr);

	if (!w->handle)
	{
		char err[512];
		get_error_msg(get_last_error(), err, 512);

		LOG_FATAL("window creation failure.\nreason: %s", err);
	}

	SetWindowLongPtrA(w->handle, GWLP_USERDATA, (LONG_PTR)w);

	RAWINPUTDEVICE rid[] = 
	{
		{
			.usUsagePage = HID_USAGE_PAGE_GENERIC,
			.usUsage = HID_USAGE_GENERIC_MOUSE,
			.dwFlags = 0,
			.hwndTarget = w->handle,
		}
	};

	if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0])))
	{
		char err[512];
		get_error_msg(get_last_error(), err, 512);

		LOG_FATAL("raw input registration failure.\nReason: %s", err);
	}

	return id;
}

static inline void poll_events(WindowId window) 
{
	Window* w = fl_get(&windows, window);

	w->mouse_delta = 0;

	MSG msg;

	while (PeekMessageA(&msg, w->handle, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_CLOSE)
		{
			PostQuitMessage(0);
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}
}

static inline bool is_open(WindowId window)
{
	return fl_get(&windows, window)->open;
}

static inline void* get_native_handle(WindowId window)
{
	return fl_get(&windows, window)->handle;
}

static inline u16vec2 get_size(WindowId window)
{
	return fl_get(&windows, window)->size;
}

static inline i16vec2 get_mouse(WindowId window)
{
	return fl_get(&windows, window)->mouse;
}

static inline i16vec2 get_mouse_delta(WindowId window)
{
	return fl_get(&windows, window)->mouse_delta;
}

static inline bool is_active(WindowId window)
{
	return GetForegroundWindow() == fl_get(&windows, window)->handle;
}

static inline bool is_key_down(WindowId window, uint16_t k)
{
	if (is_active(window))
	{
		return (1 << 15) & GetAsyncKeyState(k);
	}

	return false;
}

static inline void set_resize_cb(WindowId window, WindowResizeCb cb)
{
	fl_get(&windows, window)->on_resize = cb;
}

static inline void set_paint_cb(WindowId window, WindowPaintCb cb)
{
	fl_get(&windows, window)->on_paint = cb;
}