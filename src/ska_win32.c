//
// sk_app - Windows Win32 platform backend

#include "ska_internal.h"

#ifdef SKA_PLATFORM_WIN32

#include <windowsx.h>

// Scancode translation table (VK codes to ska_scancode_)
static ska_scancode_ ska_win32_scancode_table[256];

// UTF-8 ↔ UTF-16 conversion helpers
wchar_t* ska_utf8_to_wide(const char* utf8) {
	if (!utf8) return NULL;

	int32_t len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	if (len == 0) return NULL;

	wchar_t* wide = (wchar_t*)malloc(len * sizeof(wchar_t));
	if (!wide) return NULL;

	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wide, len);
	return wide;
}

char* ska_wide_to_utf8(const wchar_t* wide) {
	if (!wide) return NULL;

	int32_t len = WideCharToMultiByte(CP_UTF8, 0, wide, -1, NULL, 0, NULL, NULL);
	if (len == 0) return NULL;

	char* utf8 = (char*)malloc(len);
	if (!utf8) return NULL;

	WideCharToMultiByte(CP_UTF8, 0, wide, -1, utf8, len, NULL, NULL);
	return utf8;
}

void ska_free_string(void* str) {
	free(str);
}

static void ska_init_scancode_table(void) {
	// Initialize all to unknown
	for (int32_t i = 0; i < 256; i++) {
		ska_win32_scancode_table[i] = ska_scancode_unknown;
	}

	// Letters
	ska_win32_scancode_table['A'] = ska_scancode_a;
	ska_win32_scancode_table['B'] = ska_scancode_b;
	ska_win32_scancode_table['C'] = ska_scancode_c;
	ska_win32_scancode_table['D'] = ska_scancode_d;
	ska_win32_scancode_table['E'] = ska_scancode_e;
	ska_win32_scancode_table['F'] = ska_scancode_f;
	ska_win32_scancode_table['G'] = ska_scancode_g;
	ska_win32_scancode_table['H'] = ska_scancode_h;
	ska_win32_scancode_table['I'] = ska_scancode_i;
	ska_win32_scancode_table['J'] = ska_scancode_j;
	ska_win32_scancode_table['K'] = ska_scancode_k;
	ska_win32_scancode_table['L'] = ska_scancode_l;
	ska_win32_scancode_table['M'] = ska_scancode_m;
	ska_win32_scancode_table['N'] = ska_scancode_n;
	ska_win32_scancode_table['O'] = ska_scancode_o;
	ska_win32_scancode_table['P'] = ska_scancode_p;
	ska_win32_scancode_table['Q'] = ska_scancode_q;
	ska_win32_scancode_table['R'] = ska_scancode_r;
	ska_win32_scancode_table['S'] = ska_scancode_s;
	ska_win32_scancode_table['T'] = ska_scancode_t;
	ska_win32_scancode_table['U'] = ska_scancode_u;
	ska_win32_scancode_table['V'] = ska_scancode_v;
	ska_win32_scancode_table['W'] = ska_scancode_w;
	ska_win32_scancode_table['X'] = ska_scancode_x;
	ska_win32_scancode_table['Y'] = ska_scancode_y;
	ska_win32_scancode_table['Z'] = ska_scancode_z;

	// Numbers
	ska_win32_scancode_table['0'] = ska_scancode_0;
	ska_win32_scancode_table['1'] = ska_scancode_1;
	ska_win32_scancode_table['2'] = ska_scancode_2;
	ska_win32_scancode_table['3'] = ska_scancode_3;
	ska_win32_scancode_table['4'] = ska_scancode_4;
	ska_win32_scancode_table['5'] = ska_scancode_5;
	ska_win32_scancode_table['6'] = ska_scancode_6;
	ska_win32_scancode_table['7'] = ska_scancode_7;
	ska_win32_scancode_table['8'] = ska_scancode_8;
	ska_win32_scancode_table['9'] = ska_scancode_9;

	// Function keys
	ska_win32_scancode_table[VK_RETURN] = ska_scancode_return;
	ska_win32_scancode_table[VK_ESCAPE] = ska_scancode_escape;
	ska_win32_scancode_table[VK_BACK] = ska_scancode_backspace;
	ska_win32_scancode_table[VK_TAB] = ska_scancode_tab;
	ska_win32_scancode_table[VK_SPACE] = ska_scancode_space;

	// Symbols
	ska_win32_scancode_table[VK_OEM_MINUS] = ska_scancode_minus;
	ska_win32_scancode_table[VK_OEM_PLUS] = ska_scancode_equals;
	ska_win32_scancode_table[VK_OEM_4] = ska_scancode_leftbracket;
	ska_win32_scancode_table[VK_OEM_6] = ska_scancode_rightbracket;
	ska_win32_scancode_table[VK_OEM_5] = ska_scancode_backslash;
	ska_win32_scancode_table[VK_OEM_1] = ska_scancode_semicolon;
	ska_win32_scancode_table[VK_OEM_7] = ska_scancode_apostrophe;
	ska_win32_scancode_table[VK_OEM_3] = ska_scancode_grave;
	ska_win32_scancode_table[VK_OEM_COMMA] = ska_scancode_comma;
	ska_win32_scancode_table[VK_OEM_PERIOD] = ska_scancode_period;
	ska_win32_scancode_table[VK_OEM_2] = ska_scancode_slash;

	ska_win32_scancode_table[VK_CAPITAL] = ska_scancode_capslock;

	// F keys
	ska_win32_scancode_table[VK_F1] = ska_scancode_f1;
	ska_win32_scancode_table[VK_F2] = ska_scancode_f2;
	ska_win32_scancode_table[VK_F3] = ska_scancode_f3;
	ska_win32_scancode_table[VK_F4] = ska_scancode_f4;
	ska_win32_scancode_table[VK_F5] = ska_scancode_f5;
	ska_win32_scancode_table[VK_F6] = ska_scancode_f6;
	ska_win32_scancode_table[VK_F7] = ska_scancode_f7;
	ska_win32_scancode_table[VK_F8] = ska_scancode_f8;
	ska_win32_scancode_table[VK_F9] = ska_scancode_f9;
	ska_win32_scancode_table[VK_F10] = ska_scancode_f10;
	ska_win32_scancode_table[VK_F11] = ska_scancode_f11;
	ska_win32_scancode_table[VK_F12] = ska_scancode_f12;

	ska_win32_scancode_table[VK_SNAPSHOT] = ska_scancode_printscreen;
	ska_win32_scancode_table[VK_SCROLL] = ska_scancode_scrolllock;
	ska_win32_scancode_table[VK_PAUSE] = ska_scancode_pause;
	ska_win32_scancode_table[VK_INSERT] = ska_scancode_insert;

	// Navigation
	ska_win32_scancode_table[VK_HOME] = ska_scancode_home;
	ska_win32_scancode_table[VK_PRIOR] = ska_scancode_pageup;
	ska_win32_scancode_table[VK_DELETE] = ska_scancode_delete;
	ska_win32_scancode_table[VK_END] = ska_scancode_end;
	ska_win32_scancode_table[VK_NEXT] = ska_scancode_pagedown;
	ska_win32_scancode_table[VK_RIGHT] = ska_scancode_right;
	ska_win32_scancode_table[VK_LEFT] = ska_scancode_left;
	ska_win32_scancode_table[VK_DOWN] = ska_scancode_down;
	ska_win32_scancode_table[VK_UP] = ska_scancode_up;

	// Modifiers
	ska_win32_scancode_table[VK_LCONTROL] = ska_scancode_lctrl;
	ska_win32_scancode_table[VK_LSHIFT] = ska_scancode_lshift;
	ska_win32_scancode_table[VK_LMENU] = ska_scancode_lalt;
	ska_win32_scancode_table[VK_LWIN] = ska_scancode_lgui;
	ska_win32_scancode_table[VK_RCONTROL] = ska_scancode_rctrl;
	ska_win32_scancode_table[VK_RSHIFT] = ska_scancode_rshift;
	ska_win32_scancode_table[VK_RMENU] = ska_scancode_ralt;
	ska_win32_scancode_table[VK_RWIN] = ska_scancode_rgui;
}

static ska_window_t* ska_find_window_by_hwnd(HWND hwnd) {
	for (uint32_t i = 0; i < SKA_MAX_WINDOWS; i++) {
		if (g_ska.windows[i] && g_ska.windows[i]->hwnd == hwnd) {
			return g_ska.windows[i];
		}
	}
	return NULL;
}

static uint16_t ska_win32_get_modifiers(void) {
	uint16_t mods = 0;
	if (GetKeyState(VK_SHIFT) & 0x8000) mods |= ska_keymod_shift;
	if (GetKeyState(VK_CONTROL) & 0x8000) mods |= ska_keymod_ctrl;
	if (GetKeyState(VK_MENU) & 0x8000) mods |= ska_keymod_alt;
	if ((GetKeyState(VK_LWIN) & 0x8000) || (GetKeyState(VK_RWIN) & 0x8000)) {
		mods |= ska_keymod_gui;
	}
	return mods;
}

static LRESULT CALLBACK ska_win32_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	ska_window_t* window = ska_find_window_by_hwnd(hwnd);
	if (!window && msg != WM_CREATE) {
		return DefWindowProcW(hwnd, msg, wparam, lparam);
	}

	ska_event_t event = {0};
	event.timestamp = (uint32_t)ska_time_get_elapsed_ms();

	switch (msg) {
		case WM_CREATE: {
			CREATESTRUCTW* cs = (CREATESTRUCTW*)lparam;
			window = (ska_window_t*)cs->lpCreateParams;
			if (window) {
				window->hwnd = hwnd;
				window->hdc = GetDC(hwnd);
			}
			return 0;
		}

		case WM_CLOSE:
			if (window) {
				event.type = ska_event_window_close;
				event.window.window_id = window->id;
				window->should_close = true;
				ska_post_event(&event);
			}
			return 0;

		case WM_SIZE: {
			if (window) {
				int32_t width = LOWORD(lparam);
				int32_t height = HIWORD(lparam);

				if (wparam == SIZE_MINIMIZED) {
					event.type = ska_event_window_minimized;
					event.window.window_id = window->id;
					ska_post_event(&event);
				} else if (wparam == SIZE_MAXIMIZED) {
					event.type = ska_event_window_maximized;
					event.window.window_id = window->id;
					ska_post_event(&event);
				} else if (wparam == SIZE_RESTORED) {
					event.type = ska_event_window_restored;
					event.window.window_id = window->id;
					ska_post_event(&event);
				}

				if (width != window->width || height != window->height) {
					event.type = ska_event_window_resized;
					event.window.window_id = window->id;
					event.window.data1 = width;
					event.window.data2 = height;
					window->width = width;
					window->height = height;
					ska_post_event(&event);
				}
			}
			return 0;
		}

		case WM_MOVE: {
			if (window) {
				int32_t x = (int)(short)LOWORD(lparam);
				int32_t y = (int)(short)HIWORD(lparam);

				if (x != window->x || y != window->y) {
					event.type = ska_event_window_moved;
					event.window.window_id = window->id;
					event.window.data1 = x;
					event.window.data2 = y;
					window->x = x;
					window->y = y;
					ska_post_event(&event);
				}
			}
			return 0;
		}

		case WM_SETFOCUS:
			if (window) {
				event.type = ska_event_window_focus_gained;
				event.window.window_id = window->id;
				window->has_focus = true;
				ska_post_event(&event);
			}
			return 0;

		case WM_KILLFOCUS:
			if (window) {
				event.type = ska_event_window_focus_lost;
				event.window.window_id = window->id;
				window->has_focus = false;
				ska_post_event(&event);
			}
			return 0;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP: {
			if (window) {
				bool pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
				bool repeat = (lparam & 0x40000000) != 0;

				// Distinguish between left and right modifier keys
				UINT vk = (UINT)wparam;
				UINT scancode = (lparam >> 16) & 0xFF;
				bool is_extended = (lparam & 0x01000000) != 0;

				if (vk == VK_SHIFT) {
					vk = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
				} else if (vk == VK_CONTROL) {
					vk = is_extended ? VK_RCONTROL : VK_LCONTROL;
				} else if (vk == VK_MENU) {
					vk = is_extended ? VK_RMENU : VK_LMENU;
				}

				event.type = pressed ? ska_event_key_down : ska_event_key_up;
				event.keyboard.window_id = window->id;
				event.keyboard.pressed = pressed;
				event.keyboard.repeat = repeat;
				event.keyboard.scancode = ska_win32_scancode_table[vk];
				event.keyboard.modifiers = ska_win32_get_modifiers();

				// Update input state
				if (event.keyboard.scancode != ska_scancode_unknown) {
					g_ska.input_state.keyboard[event.keyboard.scancode] = pressed ? 1 : 0;
				}
				g_ska.input_state.key_modifiers = event.keyboard.modifiers;

				ska_post_event(&event);
			}
			return 0;
		}

		case WM_CHAR:
		case WM_SYSCHAR: {
			if (window && wparam > 0 && wparam < 0x10000) {
				wchar_t utf16[2] = { (wchar_t)wparam, 0 };
				char* utf8 = ska_wide_to_utf8(utf16);
				if (utf8) {
					event.type = ska_event_text_input;
					event.text.window_id = window->id;
					strncpy(event.text.text, utf8, sizeof(event.text.text) - 1);
					ska_post_event(&event);
					ska_free_string(utf8);
				}
			}
			return 0;
		}

		case WM_MOUSEMOVE: {
			if (window) {
				int32_t x = GET_X_LPARAM(lparam);
				int32_t y = GET_Y_LPARAM(lparam);

				if (!window->tracking_mouse_leave) {
					TRACKMOUSEEVENT tme = {0};
					tme.cbSize = sizeof(tme);
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = hwnd;
					TrackMouseEvent(&tme);
					window->tracking_mouse_leave = true;

					event.type = ska_event_window_mouse_enter;
					event.window.window_id = window->id;
					window->mouse_inside = true;
					ska_post_event(&event);
				}

				event.type = ska_event_mouse_motion;
				event.mouse_motion.window_id = window->id;
				event.mouse_motion.x = x;
				event.mouse_motion.y = y;
				event.mouse_motion.xrel = x - g_ska.input_state.mouse_x;
				event.mouse_motion.yrel = y - g_ska.input_state.mouse_y;

				g_ska.input_state.mouse_x = x;
				g_ska.input_state.mouse_y = y;
				g_ska.input_state.mouse_xrel = event.mouse_motion.xrel;
				g_ska.input_state.mouse_yrel = event.mouse_motion.yrel;

				ska_post_event(&event);
			}
			return 0;
		}

		case WM_MOUSELEAVE:
			if (window) {
				window->tracking_mouse_leave = false;
				event.type = ska_event_window_mouse_leave;
				event.window.window_id = window->id;
				window->mouse_inside = false;
				ska_post_event(&event);
			}
			return 0;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP: {
			if (window) {
				bool pressed = (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN ||
							   msg == WM_MBUTTONDOWN || msg == WM_XBUTTONDOWN);

				ska_mouse_button_ button;
				if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP) {
					button = ska_mouse_button_left;
				} else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP) {
					button = ska_mouse_button_right;
				} else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP) {
					button = ska_mouse_button_middle;
				} else {
					button = GET_XBUTTON_WPARAM(wparam) == XBUTTON1 ?
							ska_mouse_button_x1 : ska_mouse_button_x2;
				}

				event.type = pressed ? ska_event_mouse_button_down : ska_event_mouse_button_up;
				event.mouse_button.window_id = window->id;
				event.mouse_button.button = button;
				event.mouse_button.pressed = pressed;
				event.mouse_button.clicks = 1;
				event.mouse_button.x = GET_X_LPARAM(lparam);
				event.mouse_button.y = GET_Y_LPARAM(lparam);

				// Update button state
				uint32_t button_mask = (1 << (button - 1));
				if (pressed) {
					g_ska.input_state.mouse_buttons |= button_mask;
				} else {
					g_ska.input_state.mouse_buttons &= ~button_mask;
				}

				ska_post_event(&event);
			}
			return 0;
		}

		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL: {
			if (window) {
				int32_t delta = GET_WHEEL_DELTA_WPARAM(wparam);
				float precise = (float)delta / (float)WHEEL_DELTA;

				event.type = ska_event_mouse_wheel;
				event.mouse_wheel.window_id = window->id;

				if (msg == WM_MOUSEWHEEL) {
					event.mouse_wheel.x = 0;
					event.mouse_wheel.y = delta / WHEEL_DELTA;
					event.mouse_wheel.precise_x = 0.0f;
					event.mouse_wheel.precise_y = precise;
				} else {
					event.mouse_wheel.x = delta / WHEEL_DELTA;
					event.mouse_wheel.y = 0;
					event.mouse_wheel.precise_x = precise;
					event.mouse_wheel.precise_y = 0.0f;
				}

				ska_post_event(&event);
			}
			return 0;
		}

		case WM_SHOWWINDOW:
			if (window) {
				bool shown = (wparam == TRUE);
				event.type = shown ? ska_event_window_shown : ska_event_window_hidden;
				event.window.window_id = window->id;
				window->is_visible = shown;
				ska_post_event(&event);
			}
			return 0;
	}

	return DefWindowProcW(hwnd, msg, wparam, lparam);
}

bool ska_platform_init(void) {
	g_ska.hinstance = GetModuleHandle(NULL);

	// Enable high-DPI awareness
	SetProcessDPIAware();

	// Register window class
	g_ska.window_class.cbSize = sizeof(WNDCLASSEXW);
	g_ska.window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	g_ska.window_class.lpfnWndProc = ska_win32_window_proc;
	g_ska.window_class.hInstance = g_ska.hinstance;
	g_ska.window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	g_ska.window_class.lpszClassName = L"ska_window";

	if (!RegisterClassExW(&g_ska.window_class)) {
		ska_set_error("Failed to register window class");
		return false;
	}

	g_ska.window_class_registered = true;

	// Initialize scancode table
	ska_init_scancode_table();

	return true;
}

void ska_platform_shutdown(void) {
	if (g_ska.window_class_registered) {
		UnregisterClassW(L"ska_window", g_ska.hinstance);
		g_ska.window_class_registered = false;
	}
}

bool ska_platform_window_create(
	ska_window_t* window,
	const char* title,
	int32_t x, int32_t y,
	int32_t w, int32_t h,
	uint32_t flags
) {
	// Convert title to wide string
	wchar_t* wtitle = ska_utf8_to_wide(title);
	if (!wtitle) {
		ska_set_error("Failed to convert title to UTF-16");
		return false;
	}

	// Determine window style
	DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	DWORD ex_style = WS_EX_APPWINDOW;

	if (flags & ska_window_borderless) {
		style |= WS_POPUP;
	} else {
		style |= WS_OVERLAPPEDWINDOW;
		if (!(flags & ska_window_resizable)) {
			style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
		}
	}

	if (flags & ska_window_hidden) {
		style &= ~WS_VISIBLE;
	} else {
		style |= WS_VISIBLE;
	}

	// Adjust for window decorations
	RECT rect = { 0, 0, w, h };
	AdjustWindowRectEx(&rect, style, FALSE, ex_style);
	int32_t adj_width = rect.right - rect.left;
	int32_t adj_height = rect.bottom - rect.top;

	// Center window if requested
	if (x == -1 || y == -1) {
		int32_t screen_width = GetSystemMetrics(SM_CXSCREEN);
		int32_t screen_height = GetSystemMetrics(SM_CYSCREEN);
		x = (screen_width - adj_width) / 2;
		y = (screen_height - adj_height) / 2;
	}

	// Create window
	window->hwnd = CreateWindowExW(
		ex_style,
		L"ska_window",
		wtitle,
		style,
		x, y,
		adj_width, adj_height,
		NULL, NULL,
		g_ska.hinstance,
		window  // Pass window pointer for WM_CREATE
	);

	ska_free_string(wtitle);

	if (!window->hwnd) {
		ska_set_error("Failed to create window: error %lu", GetLastError());
		return false;
	}

	// Store title
	window->title = strdup(title);

	// Get actual window position and size
	RECT client_rect;
	GetClientRect(window->hwnd, &client_rect);
	window->width = client_rect.right - client_rect.left;
	window->height = client_rect.bottom - client_rect.top;

	RECT window_rect;
	GetWindowRect(window->hwnd, &window_rect);
	window->x = window_rect.left;
	window->y = window_rect.top;

	// Set initial window state
	if (flags & ska_window_maximized) {
		ShowWindow(window->hwnd, SW_MAXIMIZE);
	} else if (flags & ska_window_minimized) {
		ShowWindow(window->hwnd, SW_MINIMIZE);
	}

	return true;
}

void ska_platform_window_destroy(ska_window_t* window) {
	if (window->hdc) {
		ReleaseDC(window->hwnd, window->hdc);
		window->hdc = NULL;
	}

	if (window->hwnd) {
		DestroyWindow(window->hwnd);
		window->hwnd = NULL;
	}
}

void ska_platform_window_set_title(ska_window_t* window, const char* title) {
	wchar_t* wtitle = ska_utf8_to_wide(title);
	if (wtitle) {
		SetWindowTextW(window->hwnd, wtitle);
		ska_free_string(wtitle);
	}

	if (window->title) {
		free(window->title);
	}
	window->title = strdup(title);
}

void ska_platform_window_set_position(ska_window_t* window, int32_t x, int32_t y) {
	SetWindowPos(window->hwnd, NULL, x, y, 0, 0,
				 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void ska_platform_window_set_size(ska_window_t* window, int32_t w, int32_t h) {
	DWORD style = GetWindowLong(window->hwnd, GWL_STYLE);
	DWORD ex_style = GetWindowLong(window->hwnd, GWL_EXSTYLE);

	RECT rect = { 0, 0, w, h };
	AdjustWindowRectEx(&rect, style, FALSE, ex_style);

	SetWindowPos(window->hwnd, NULL, 0, 0,
				 rect.right - rect.left, rect.bottom - rect.top,
				 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void ska_platform_window_show(ska_window_t* window) {
	ShowWindow(window->hwnd, SW_SHOW);
	window->is_visible = true;
}

void ska_platform_window_hide(ska_window_t* window) {
	ShowWindow(window->hwnd, SW_HIDE);
	window->is_visible = false;
}

void ska_platform_window_maximize(ska_window_t* window) {
	ShowWindow(window->hwnd, SW_MAXIMIZE);
}

void ska_platform_window_minimize(ska_window_t* window) {
	ShowWindow(window->hwnd, SW_MINIMIZE);
}

void ska_platform_window_restore(ska_window_t* window) {
	ShowWindow(window->hwnd, SW_RESTORE);
}

void ska_platform_window_raise(ska_window_t* window) {
	SetForegroundWindow(window->hwnd);
	SetFocus(window->hwnd);
}

void ska_platform_window_get_drawable_size(ska_window_t* window, int32_t* opt_out_width, int32_t* opt_out_height) {
	// For Win32, drawable size equals client size unless using high-DPI scaling
	window->drawable_width = window->width;
	window->drawable_height = window->height;
	(void)opt_out_width;
	(void)opt_out_height;
}

void ska_platform_warp_mouse(ska_window_t* window, int32_t x, int32_t y) {
	POINT pt = { x, y };
	ClientToScreen(window->hwnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

void ska_platform_set_cursor(ska_system_cursor_ cursor) {
	static HCURSOR win32_cursors[ska_system_cursor_count_] = {0};

	// Win32 system cursor mappings
	const LPWSTR win32_cursor_ids[] = {
		[ska_system_cursor_arrow]      = IDC_ARROW,
		[ska_system_cursor_ibeam]      = IDC_IBEAM,
		[ska_system_cursor_wait]       = IDC_WAIT,
		[ska_system_cursor_crosshair]  = IDC_CROSS,
		[ska_system_cursor_waitarrow]  = IDC_APPSTARTING,
		[ska_system_cursor_sizenwse]   = IDC_SIZENWSE,
		[ska_system_cursor_sizenesw]   = IDC_SIZENESW,
		[ska_system_cursor_sizewe]     = IDC_SIZEWE,
		[ska_system_cursor_sizens]     = IDC_SIZENS,
		[ska_system_cursor_sizeall]    = IDC_SIZEALL,
		[ska_system_cursor_no]         = IDC_NO,
		[ska_system_cursor_hand]       = IDC_HAND,
	};

	if (cursor >= ska_system_cursor_count_) {
		return;
	}

	// Load cursor if not already cached
	if (win32_cursors[cursor] == NULL) {
		win32_cursors[cursor] = LoadCursorW(NULL, win32_cursor_ids[cursor]);
	}

	SetCursor(win32_cursors[cursor]);
}

void ska_platform_show_cursor(bool show) {
	ShowCursor(show ? TRUE : FALSE);
}

bool ska_platform_set_relative_mouse_mode(bool enabled) {
	if (enabled) {
		// Clip cursor to client area
		ska_window_t* window = g_ska.windows[0];  // Use first window
		if (window && window->hwnd) {
			RECT rect;
			GetClientRect(window->hwnd, &rect);
			ClientToScreen(window->hwnd, (POINT*)&rect.left);
			ClientToScreen(window->hwnd, (POINT*)&rect.right);
			ClipCursor(&rect);
		}
	} else {
		ClipCursor(NULL);
	}
	return true;
}

void ska_platform_pump_events(void) {
	MSG msg;
	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			ska_event_t event = {0};
			event.type = ska_event_quit;
			event.timestamp = (uint32_t)ska_time_get_elapsed_ms();
			ska_post_event(&event);
		}
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

/////////////////////////////////////////
// Win32 specific subset of Vulkan header
/////////////////////////////////////////

typedef VkFlags VkWin32SurfaceCreateFlagsKHR;
typedef struct VkWin32SurfaceCreateInfoKHR {
	VkStructureType                 sType;
	const void*                     pNext;
	VkWin32SurfaceCreateFlagsKHR    flags;
	HINSTANCE                       hinstance;
	HWND                            hwnd;
} VkWin32SurfaceCreateInfoKHR;

typedef VkResult (VKAPI_PTR *PFN_vkCreateWin32SurfaceKHR)(VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo, const /*VkAllocationCallbacks*/ void* pAllocator, VkSurfaceKHR* pSurface);

/////////////////////////////////////////

const char** ska_platform_vk_get_instance_extensions(uint32_t* out_count) {
	static const char* extensions[] = {
		"VK_KHR_surface",
		"VK_KHR_win32_surface"
	};
	*out_count = 2;
	return extensions;
}

bool ska_platform_vk_create_surface(const ska_window_t* window, VkInstance instance, VkSurfaceKHR* out_surface) {
	HMODULE module = LoadLibraryA("vulkan-1.dll");
	if (!module) {
		ska_set_error("Failed to load Vulkan DLL");
		return false;
	}

	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)(void(*)(void))GetProcAddress(module, "vkGetInstanceProcAddr");
	if (!vkGetInstanceProcAddr) {
		ska_set_error("Failed to load vkGetInstanceProcAddr");
		return false;
	}

	PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
	if (!vkCreateWin32SurfaceKHR) {
		ska_set_error("Failed to load vkCreateWin32SurfaceKHR");
		return false;
	}

	VkWin32SurfaceCreateInfoKHR create_info = {0};
	create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	create_info.hinstance = g_ska.hinstance;
	create_info.hwnd = window->hwnd;

	VkResult result = vkCreateWin32SurfaceKHR(instance, &create_info, NULL, out_surface);
	if (result != VK_SUCCESS) {
		ska_set_error("Failed to create Vulkan Win32 surface: %d", result);
		return false;
	}

	return true;
}

// ========== Text Input Platform Functions ==========

void ska_platform_show_virtual_keyboard(bool visible, ska_text_input_type_ type) {
	// Win32 - desktop platform, no virtual keyboard
	(void)visible;
	(void)type;
}

// ========== Clipboard Platform Functions ==========

size_t ska_platform_clipboard_get_text(char* opt_out_buffer, size_t buffer_size) {
	if (!OpenClipboard(NULL)) {
		return 0;
	}

	HANDLE hdata = GetClipboardData(CF_UNICODETEXT);
	if (!hdata) {
		CloseClipboard();
		return 0;
	}

	wchar_t* wide_text = (wchar_t*)GlobalLock(hdata);
	if (!wide_text) {
		CloseClipboard();
		return 0;
	}

	// Convert to UTF-8
	char* utf8_text = ska_wide_to_utf8(wide_text);
	GlobalUnlock(hdata);
	CloseClipboard();

	if (!utf8_text) {
		return 0;
	}

	// Calculate size including null terminator
	size_t text_size = strlen(utf8_text) + 1;

	// If buffer is provided, copy the text
	if (opt_out_buffer && buffer_size > 0) {
		size_t copy_size = (text_size < buffer_size) ? text_size : buffer_size;
		memcpy(opt_out_buffer, utf8_text, copy_size - 1);
		opt_out_buffer[copy_size - 1] = '\0';
	}

	ska_free_string(utf8_text);
	return text_size;
}

bool ska_platform_clipboard_set_text(const char* text) {
	if (!text) {
		ska_set_error("ska_platform_clipboard_set_text: text cannot be NULL");
		return false;
	}

	// Convert UTF-8 to wide string
	wchar_t* wide_text = ska_utf8_to_wide(text);
	if (!wide_text) {
		ska_set_error("ska_platform_clipboard_set_text: UTF-8 conversion failed");
		return false;
	}

	size_t wide_len = wcslen(wide_text) + 1;
	size_t buffer_size = wide_len * sizeof(wchar_t);

	if (!OpenClipboard(NULL)) {
		ska_free_string(wide_text);
		ska_set_error("ska_platform_clipboard_set_text: OpenClipboard failed");
		return false;
	}

	EmptyClipboard();

	HGLOBAL hglob = GlobalAlloc(GMEM_MOVEABLE, buffer_size);
	if (!hglob) {
		CloseClipboard();
		ska_free_string(wide_text);
		ska_set_error("ska_platform_clipboard_set_text: GlobalAlloc failed");
		return false;
	}

	wchar_t* clipboard_buffer = (wchar_t*)GlobalLock(hglob);
	if (!clipboard_buffer) {
		GlobalFree(hglob);
		CloseClipboard();
		ska_free_string(wide_text);
		ska_set_error("ska_platform_clipboard_set_text: GlobalLock failed");
		return false;
	}

	memcpy(clipboard_buffer, wide_text, buffer_size);
	GlobalUnlock(hglob);

	if (!SetClipboardData(CF_UNICODETEXT, hglob)) {
		GlobalFree(hglob);
		CloseClipboard();
		ska_free_string(wide_text);
		ska_set_error("ska_platform_clipboard_set_text: SetClipboardData failed");
		return false;
	}

	CloseClipboard();
	ska_free_string(wide_text);
	return true;
}

#endif // SKA_PLATFORM_WIN32
