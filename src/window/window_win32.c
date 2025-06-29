#include <window/window.h>

typedef struct 
{
	HWND handle;

	uint16_t width;
	uint16_t height;

	int16_t mouse_x;
	int16_t mouse_y;

	int16_t mouse_delta_x;
	int16_t mouse_delta_y;

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
			window->mouse_x = GET_X_LPARAM(lparam);
			window->mouse_y = GET_Y_LPARAM(lparam);
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
						window->mouse_delta_x = raw_input->data.mouse.lLastX;
						window->mouse_delta_y = raw_input->data.mouse.lLastY;
					}
				}
			}
		} break;

		case WM_SIZE:
		{
			window->width = LOWORD(lparam);
			window->height = HIWORD(lparam);
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

	Window w = {};

	w.open = true;
	w.width = width;
	w.height = height;

	w.handle = CreateWindowA(class.lpszClassName, title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, nullptr, nullptr);

	if (!w.handle)
	{
		MessageBoxA(nullptr, "fatal error", "", 0);
		ExitProcess(0);
	}

	WindowId id = fl_push(&windows, w);

	SetWindowLongPtrA(w.handle, GWLP_USERDATA, (LONG_PTR)fl_get(&windows, id));

	return id;
}

static inline void poll_events(WindowId w) 
{
	const Window* window = fl_get(&windows, w);

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

static inline uint16_t get_width(WindowId w)
{
	return fl_get(&windows, w)->width;
}

static inline uint16_t get_height(WindowId w)
{
	return fl_get(&windows, w)->height;
}

static inline int16_t get_mouse_x(WindowId w)
{
	return fl_get(&windows, w)->mouse_x;
}

static inline int16_t get_mouse_y(WindowId w)
{
	return fl_get(&windows, w)->mouse_y;
}