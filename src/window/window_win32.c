#include <window/window.h>

typedef struct 
{
	HWND handle;

	u16vec2 size;
	i16vec2 mouse;
	i16vec2 mouse_delta;
	
	bool open;
} Window;

static FreeList(Window, MAX_WINDOWS) windows = fl_default();

static inline __stdcall LRESULT wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
{
	Window* window = (Window*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

	if (!window)
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
			window->open = false;
		} break;

		case WM_ERASEBKGND:
		{
			return 1;
		} break;

		case WM_MOUSEMOVE:
		{
			window->mouse = (i16vec2){ GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
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
						window->mouse_delta = (i16vec2){ raw_input->data.mouse.lLastX, raw_input->data.mouse.lLastY };
					}
				}
			}
		} break;

		case WM_SIZE:
		{
			window->size = (u16vec2){ LOWORD(lparam), HIWORD(lparam) };
		} break;

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
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
			MessageBoxA(nullptr, "fatal error", "", 0);
			ExitProcess(0);
		}
	);

	WindowId id = fl_push(&windows, (Window){});
	Window* w = fl_get(&windows, id);
	
	w->size = (u16vec2){ width, height };

	w->open = true;
	w->handle = CreateWindowA(class.lpszClassName, title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, w->size.x, w->size.y, nullptr, nullptr, nullptr, nullptr);

	if (!w->handle)
	{
		MessageBoxA(nullptr, "fatal error", "", 0);
		ExitProcess(0);
	}

	SetWindowLongPtrA(w->handle, GWLP_USERDATA, (LONG_PTR)w);

	// device registration
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
		MessageBoxA(nullptr, "fatal error", "", 0);
		ExitProcess(0);
	}

	return id;
}

static inline void poll_events(WindowId w) 
{
	Window* window = fl_get(&windows, w);

	window->mouse_delta = 0;

	MSG msg;

	while (PeekMessageA(&msg, window->handle, 0, 0, PM_REMOVE))
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

static inline bool is_open(WindowId w)
{
	return fl_get(&windows, w)->open;
}

static inline void* get_native_handle(WindowId w)
{
	return fl_get(&windows, w)->handle;
}

static inline u16vec2 get_size(WindowId w)
{
	return fl_get(&windows, w)->size;
}

static inline i16vec2 get_mouse(WindowId w)
{
	return fl_get(&windows, w)->mouse;
}

static inline i16vec2 get_mouse_delta(WindowId w)
{
	return fl_get(&windows, w)->mouse_delta;
}

static inline bool is_key_down(uint16_t k)
{
	return (1 << 15) & GetAsyncKeyState(k);
}