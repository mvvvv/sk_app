//
// sk_app - Linux X11 platform backend

#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include "ska_internal.h"

#ifdef SKA_PLATFORM_LINUX

#include <X11/keysym.h>
#include <X11/Xresource.h>
#include <locale.h>
#include <sys/select.h>
#include <unistd.h>

// Scancode translation table (X11 keycodes to ska_scancode_)
static ska_scancode_ ska_x11_scancode_table[256];

static void ska_init_scancode_table(void) {
	// Initialize all to unknown
	for (int32_t i = 0; i < 256; i++) {
		ska_x11_scancode_table[i] = ska_scancode_unknown;
	}

	// Letters
	ska_x11_scancode_table[38] = ska_scancode_a;
	ska_x11_scancode_table[56] = ska_scancode_b;
	ska_x11_scancode_table[54] = ska_scancode_c;
	ska_x11_scancode_table[40] = ska_scancode_d;
	ska_x11_scancode_table[26] = ska_scancode_e;
	ska_x11_scancode_table[41] = ska_scancode_f;
	ska_x11_scancode_table[42] = ska_scancode_g;
	ska_x11_scancode_table[43] = ska_scancode_h;
	ska_x11_scancode_table[31] = ska_scancode_i;
	ska_x11_scancode_table[44] = ska_scancode_j;
	ska_x11_scancode_table[45] = ska_scancode_k;
	ska_x11_scancode_table[46] = ska_scancode_l;
	ska_x11_scancode_table[58] = ska_scancode_m;
	ska_x11_scancode_table[57] = ska_scancode_n;
	ska_x11_scancode_table[32] = ska_scancode_o;
	ska_x11_scancode_table[33] = ska_scancode_p;
	ska_x11_scancode_table[24] = ska_scancode_q;
	ska_x11_scancode_table[27] = ska_scancode_r;
	ska_x11_scancode_table[39] = ska_scancode_s;
	ska_x11_scancode_table[28] = ska_scancode_t;
	ska_x11_scancode_table[30] = ska_scancode_u;
	ska_x11_scancode_table[55] = ska_scancode_v;
	ska_x11_scancode_table[25] = ska_scancode_w;
	ska_x11_scancode_table[53] = ska_scancode_x;
	ska_x11_scancode_table[29] = ska_scancode_y;
	ska_x11_scancode_table[52] = ska_scancode_z;

	// Numbers
	ska_x11_scancode_table[10] = ska_scancode_1;
	ska_x11_scancode_table[11] = ska_scancode_2;
	ska_x11_scancode_table[12] = ska_scancode_3;
	ska_x11_scancode_table[13] = ska_scancode_4;
	ska_x11_scancode_table[14] = ska_scancode_5;
	ska_x11_scancode_table[15] = ska_scancode_6;
	ska_x11_scancode_table[16] = ska_scancode_7;
	ska_x11_scancode_table[17] = ska_scancode_8;
	ska_x11_scancode_table[18] = ska_scancode_9;
	ska_x11_scancode_table[19] = ska_scancode_0;

	// Function keys
	ska_x11_scancode_table[36] = ska_scancode_return;
	ska_x11_scancode_table[9] = ska_scancode_escape;
	ska_x11_scancode_table[22] = ska_scancode_backspace;
	ska_x11_scancode_table[23] = ska_scancode_tab;
	ska_x11_scancode_table[65] = ska_scancode_space;

	// Symbols
	ska_x11_scancode_table[20] = ska_scancode_minus;
	ska_x11_scancode_table[21] = ska_scancode_equals;
	ska_x11_scancode_table[34] = ska_scancode_leftbracket;
	ska_x11_scancode_table[35] = ska_scancode_rightbracket;
	ska_x11_scancode_table[51] = ska_scancode_backslash;
	ska_x11_scancode_table[47] = ska_scancode_semicolon;
	ska_x11_scancode_table[48] = ska_scancode_apostrophe;
	ska_x11_scancode_table[49] = ska_scancode_grave;
	ska_x11_scancode_table[59] = ska_scancode_comma;
	ska_x11_scancode_table[60] = ska_scancode_period;
	ska_x11_scancode_table[61] = ska_scancode_slash;

	ska_x11_scancode_table[66] = ska_scancode_capslock;

	// F keys
	ska_x11_scancode_table[67] = ska_scancode_f1;
	ska_x11_scancode_table[68] = ska_scancode_f2;
	ska_x11_scancode_table[69] = ska_scancode_f3;
	ska_x11_scancode_table[70] = ska_scancode_f4;
	ska_x11_scancode_table[71] = ska_scancode_f5;
	ska_x11_scancode_table[72] = ska_scancode_f6;
	ska_x11_scancode_table[73] = ska_scancode_f7;
	ska_x11_scancode_table[74] = ska_scancode_f8;
	ska_x11_scancode_table[75] = ska_scancode_f9;
	ska_x11_scancode_table[76] = ska_scancode_f10;
	ska_x11_scancode_table[95] = ska_scancode_f11;
	ska_x11_scancode_table[96] = ska_scancode_f12;

	ska_x11_scancode_table[107] = ska_scancode_printscreen;
	ska_x11_scancode_table[78] = ska_scancode_scrolllock;
	ska_x11_scancode_table[127] = ska_scancode_pause;
	ska_x11_scancode_table[118] = ska_scancode_insert;

	// Navigation
	ska_x11_scancode_table[110] = ska_scancode_home;
	ska_x11_scancode_table[112] = ska_scancode_pageup;
	ska_x11_scancode_table[119] = ska_scancode_delete;
	ska_x11_scancode_table[115] = ska_scancode_end;
	ska_x11_scancode_table[117] = ska_scancode_pagedown;
	ska_x11_scancode_table[114] = ska_scancode_right;
	ska_x11_scancode_table[113] = ska_scancode_left;
	ska_x11_scancode_table[116] = ska_scancode_down;
	ska_x11_scancode_table[111] = ska_scancode_up;

	// Modifiers
	ska_x11_scancode_table[37] = ska_scancode_lctrl;
	ska_x11_scancode_table[50] = ska_scancode_lshift;
	ska_x11_scancode_table[64] = ska_scancode_lalt;
	ska_x11_scancode_table[133] = ska_scancode_lgui;
	ska_x11_scancode_table[105] = ska_scancode_rctrl;
	ska_x11_scancode_table[62] = ska_scancode_rshift;
	ska_x11_scancode_table[108] = ska_scancode_ralt;
	ska_x11_scancode_table[134] = ska_scancode_rgui;
}

static ska_window_t* ska_find_window_by_xwindow(Window xwin) {
	for (uint32_t i = 0; i < SKA_MAX_WINDOWS; i++) {
		if (g_ska.windows[i] && g_ska.windows[i]->xwindow == xwin) {
			return g_ska.windows[i];
		}
	}
	return NULL;
}

static uint16_t ska_x11_get_modifiers(uint32_t state) {
	uint16_t mods = 0;
	if (state & ShiftMask) mods |= ska_keymod_shift;
	if (state & ControlMask) mods |= ska_keymod_ctrl;
	if (state & Mod1Mask) mods |= ska_keymod_alt;
	if (state & Mod4Mask) mods |= ska_keymod_gui;
	return mods;
}

bool ska_platform_init(void) {
	// Set locale for X11
	setlocale(LC_ALL, "");
	XSetLocaleModifiers("");

	g_ska.x_display = XOpenDisplay(NULL);
	if (!g_ska.x_display) {
		ska_set_error("Failed to open X11 display");
		return false;
	}

	g_ska.x_screen = DefaultScreen(g_ska.x_display);
	g_ska.x_root = RootWindow(g_ska.x_display, g_ska.x_screen);

	// Initialize input method
	g_ska.xim = XOpenIM(g_ska.x_display, NULL, NULL, NULL);
	if (!g_ska.xim) {
		ska_log(ska_log_warn, "Failed to open X Input Method");
	}

	// Initialize Xrm database (required before using XrmGetResource for DPI queries)
	XrmInitialize();

	// Get WM atoms
	g_ska.wm_protocols                = XInternAtom(g_ska.x_display, "WM_PROTOCOLS", False);
	g_ska.wm_delete_window            = XInternAtom(g_ska.x_display, "WM_DELETE_WINDOW", False);
	g_ska.net_wm_state                = XInternAtom(g_ska.x_display, "_NET_WM_STATE", False);
	g_ska.net_wm_state_fullscreen     = XInternAtom(g_ska.x_display, "_NET_WM_STATE_FULLSCREEN", False);
	g_ska.net_wm_state_maximized_vert = XInternAtom(g_ska.x_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
	g_ska.net_wm_state_maximized_horz = XInternAtom(g_ska.x_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
	g_ska.resource_manager            = XInternAtom(g_ska.x_display, "RESOURCE_MANAGER", False);

	// Watch root window for property changes (for DPI change detection via xrdb)
	XSelectInput(g_ska.x_display, g_ska.x_root, PropertyChangeMask);

	// Cache initial DPI scale
	g_ska.cached_dpi_scale = 0.0f; // Will be set on first window creation

	// Initialize scancode table
	ska_init_scancode_table();

	// Check for XInput2
	int32_t xi_event, xi_error;
	if (!XQueryExtension(g_ska.x_display, "XInputExtension", &g_ska.xi_opcode, &xi_event, &xi_error)) {
		ska_log(ska_log_warn, "XInput extension not available");
	}

	return true;
}

void ska_platform_shutdown(void) {
	if (g_ska.xim) {
		XCloseIM(g_ska.xim);
		g_ska.xim = NULL;
	}

	if (g_ska.x_display) {
		XCloseDisplay(g_ska.x_display);
		g_ska.x_display = NULL;
	}
}

bool ska_platform_window_create(
	ska_window_t* window,
	const char* title,
	int32_t x, int32_t y,
	int32_t w, int32_t h,
	uint32_t flags
) {
	// Set window attributes
	XSetWindowAttributes wa = {0};
	wa.event_mask = KeyPressMask | KeyReleaseMask |
					ButtonPressMask | ButtonReleaseMask |
					PointerMotionMask |
					EnterWindowMask | LeaveWindowMask |
					FocusChangeMask |
					StructureNotifyMask |
					ExposureMask;
	wa.colormap = XCreateColormap(g_ska.x_display, g_ska.x_root,
								   DefaultVisual(g_ska.x_display, g_ska.x_screen),
								   AllocNone);

	// Center window if requested
	if (x == -1 || y == -1) {
		Screen* screen = DefaultScreenOfDisplay(g_ska.x_display);
		x = (WidthOfScreen(screen) - w) / 2;
		y = (HeightOfScreen(screen) - h) / 2;
	}

	// Create window
	window->xwindow = XCreateWindow(
		g_ska.x_display,
		g_ska.x_root,
		x, y, w, h,
		0,
		CopyFromParent,
		InputOutput,
		CopyFromParent,
		CWEventMask | CWColormap,
		&wa
	);

	if (!window->xwindow) {
		ska_set_error("Failed to create X11 window");
		return false;
	}

	// Set window title
	XStoreName(g_ska.x_display, window->xwindow, title);
	XSetIconName(g_ska.x_display, window->xwindow, title);

	// Set WM protocols
	XSetWMProtocols(g_ska.x_display, window->xwindow, &g_ska.wm_delete_window, 1);

	// Create input context
	if (g_ska.xim) {
		window->xic = XCreateIC(
			g_ska.xim,
			XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
			XNClientWindow, window->xwindow,
			XNFocusWindow, window->xwindow,
			NULL
		);
	}

	// Store title
	window->title = strdup(title);

	// Apply window flags
	if (flags & ska_window_borderless) {
		// Remove decorations using MWM hints
		struct {
			unsigned long flags;
			unsigned long functions;
			unsigned long decorations;
			long input_mode;
			unsigned long status;
		} hints = {0};

		hints.flags = 2; // MWM_HINTS_DECORATIONS
		hints.decorations = 0;

		Atom mwm_hints = XInternAtom(g_ska.x_display, "_MOTIF_WM_HINTS", False);
		XChangeProperty(g_ska.x_display, window->xwindow, mwm_hints, mwm_hints,
					   32, PropModeReplace, (unsigned char*)&hints, 5);
	}

	// Set size hints
	XSizeHints* size_hints = XAllocSizeHints();
	if (size_hints) {
		size_hints->flags = PPosition | PSize;
		if (!(flags & ska_window_resizable)) {
			size_hints->flags |= PMinSize | PMaxSize;
			size_hints->min_width = size_hints->max_width = w;
			size_hints->min_height = size_hints->max_height = h;
		}
		XSetWMNormalHints(g_ska.x_display, window->xwindow, size_hints);
		XFree(size_hints);
	}

	// Ensure it knows its process id
	Atom  net_wm_pid = XInternAtom(g_ska.x_display, "_NET_WM_PID", False);
	pid_t pid        = getpid();
	XChangeProperty(g_ska.x_display, window->xwindow, net_wm_pid, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&pid, 1);

	window->x = x;
	window->y = y;
	window->width = w;
	window->height = h;
	window->drawable_width = w;
	window->drawable_height = h;
	window->dpi_scale = ska_platform_get_dpi_scale(window);

	// Cache DPI scale for change detection (first window sets it)
	if (g_ska.cached_dpi_scale == 0.0f) {
		g_ska.cached_dpi_scale = window->dpi_scale;
	}

	return true;
}

void ska_platform_window_destroy(ska_window_t* window) {
	if (window->xic) {
		XDestroyIC(window->xic);
	}

	if (window->xwindow) {
		XDestroyWindow(g_ska.x_display, window->xwindow);
		XFlush(g_ska.x_display);
	}
}

void ska_platform_window_set_title(ska_window_t* window, const char* title) {
	if (window->title) {
		free(window->title);
	}
	window->title = strdup(title);
	XStoreName(g_ska.x_display, window->xwindow, title);
	XSetIconName(g_ska.x_display, window->xwindow, title);
	XFlush(g_ska.x_display);
}

void ska_platform_get_frame_extents(const ska_window_t* window, int32_t* out_left, int32_t* out_right, int32_t* out_top, int32_t* out_bottom) {
	int32_t left = 0, right = 0, top = 0, bottom = 0;

	if (window && window->xwindow) {
		Atom net_frame_extents = XInternAtom(g_ska.x_display, "_NET_FRAME_EXTENTS", False);
		Atom actual_type;
		int32_t actual_format;
		unsigned long nitems, bytes_after;
		unsigned char* data = NULL;

		if (XGetWindowProperty(g_ska.x_display, window->xwindow, net_frame_extents,
		                       0, 4, False, XA_CARDINAL,
		                       &actual_type, &actual_format, &nitems, &bytes_after, &data) == Success) {
			if (data && nitems == 4) {
				long* extents = (long*)data;
				left   = (int32_t)extents[0];
				right  = (int32_t)extents[1];
				top    = (int32_t)extents[2];
				bottom = (int32_t)extents[3];
			}
			if (data) XFree(data);
		}
	}

	if (out_left)   *out_left   = left;
	if (out_right)  *out_right  = right;
	if (out_top)    *out_top    = top;
	if (out_bottom) *out_bottom = bottom;
}

void ska_platform_window_set_frame_position(ska_window_t* window, int32_t x, int32_t y) {
	XMoveWindow(g_ska.x_display, window->xwindow, x, y);
	XFlush(g_ska.x_display);

	// Update cached content position
	int32_t left, top;
	ska_platform_get_frame_extents(window, &left, NULL, &top, NULL);
	window->x = x + left;
	window->y = y + top;
}

void ska_platform_window_set_frame_size(ska_window_t* window, int32_t w, int32_t h) {
	XResizeWindow(g_ska.x_display, window->xwindow, w, h);
	XFlush(g_ska.x_display);
}

void ska_platform_window_show(ska_window_t* window) {
	XMapWindow(g_ska.x_display, window->xwindow);
	XFlush(g_ska.x_display);
	window->is_visible = true;
}

void ska_platform_window_hide(ska_window_t* window) {
	XUnmapWindow(g_ska.x_display, window->xwindow);
	XFlush(g_ska.x_display);
	window->is_visible = false;
}

void ska_platform_window_maximize(ska_window_t* window) {
	XEvent event = {0};
	event.type = ClientMessage;
	event.xclient.window = window->xwindow;
	event.xclient.message_type = g_ska.net_wm_state;
	event.xclient.format = 32;
	event.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD
	event.xclient.data.l[1] = g_ska.net_wm_state_maximized_vert;
	event.xclient.data.l[2] = g_ska.net_wm_state_maximized_horz;

	XSendEvent(g_ska.x_display, g_ska.x_root, False,
			   SubstructureNotifyMask | SubstructureRedirectMask, &event);
	XFlush(g_ska.x_display);
}

void ska_platform_window_minimize(ska_window_t* window) {
	XIconifyWindow(g_ska.x_display, window->xwindow, g_ska.x_screen);
	XFlush(g_ska.x_display);
}

void ska_platform_window_restore(ska_window_t* window) {
	XMapWindow(g_ska.x_display, window->xwindow);
	XFlush(g_ska.x_display);
}

void ska_platform_window_raise(ska_window_t* window) {
	XRaiseWindow(g_ska.x_display, window->xwindow);
	XSetInputFocus(g_ska.x_display, window->xwindow, RevertToParent, CurrentTime);
	XFlush(g_ska.x_display);
}

void ska_platform_window_get_drawable_size(ska_window_t* window, int32_t* opt_out_width, int32_t* opt_out_height) {
	// For X11, drawable size equals window size unless using high-DPI
	window->drawable_width = window->width;
	window->drawable_height = window->height;
	(void)opt_out_width;
	(void)opt_out_height;
}

float ska_platform_get_dpi_scale(const ska_window_t* window) {
	(void)window;

	// Query Xft.dpi from Xresources (this is how GNOME/KDE/etc communicate scaling)
	char* resource_string = XResourceManagerString(g_ska.x_display);
	if (resource_string) {
		XrmDatabase db = XrmGetStringDatabase(resource_string);
		if (db) {
			XrmValue value;
			char* type = NULL;
			if (XrmGetResource(db, "Xft.dpi", "Xft.Dpi", &type, &value)) {
				if (type && strcmp(type, "String") == 0 && value.addr) {
					float dpi = (float)atof(value.addr);
					XrmDestroyDatabase(db);
					if (dpi > 0) {
						return dpi / 96.0f; // 96 DPI is the baseline (100% scale)
					}
				}
			}
			XrmDestroyDatabase(db);
		}
	}

	// Fallback: try to get DPI from screen dimensions
	// This is less reliable but better than nothing
	int32_t screen_width_px = DisplayWidth(g_ska.x_display, g_ska.x_screen);
	int32_t screen_width_mm = DisplayWidthMM(g_ska.x_display, g_ska.x_screen);
	if (screen_width_mm > 0) {
		float dpi = (float)screen_width_px / ((float)screen_width_mm / 25.4f);
		// Only use this if it's significantly different from 96
		// (some systems report incorrect physical dimensions)
		if (dpi >= 120.0f) {
			return dpi / 96.0f;
		}
	}

	return 1.0f;
}

void ska_platform_warp_mouse(ska_window_t* ref_window, int32_t x, int32_t y) {
	ref_window->mouse_warped = true;
	XWarpPointer(g_ska.x_display, None, ref_window->xwindow, 0, 0, 0, 0, x, y);
	XFlush(g_ska.x_display);
}

// Cursor cache and state (shared between ska_platform_set_cursor and ska_platform_show_cursor)
static Cursor g_x_cursors[ska_system_cursor_count_] = {0};
static ska_system_cursor_ g_current_cursor = ska_system_cursor_arrow;

void ska_platform_set_cursor(ska_system_cursor_ cursor) {
	// Freedesktop cursor specification names
	const char* xcursor_names[] = {
		[ska_system_cursor_arrow]      = "default",
		[ska_system_cursor_ibeam]      = "text",
		[ska_system_cursor_wait]       = "wait",
		[ska_system_cursor_crosshair]  = "crosshair",
		[ska_system_cursor_waitarrow]  = "progress",
		[ska_system_cursor_sizenwse]   = "nwse-resize",
		[ska_system_cursor_sizenesw]   = "nesw-resize",
		[ska_system_cursor_sizewe]     = "ew-resize",
		[ska_system_cursor_sizens]     = "ns-resize",
		[ska_system_cursor_sizeall]    = "all-scroll",
		[ska_system_cursor_no]         = "not-allowed",
		[ska_system_cursor_hand]       = "pointer",
	};

	// X11 cursor font fallbacks
	const uint32_t x11_cursors[] = {
		[ska_system_cursor_arrow]      = XC_left_ptr,
		[ska_system_cursor_ibeam]      = XC_xterm,
		[ska_system_cursor_wait]       = XC_watch,
		[ska_system_cursor_crosshair]  = XC_crosshair,
		[ska_system_cursor_waitarrow]  = XC_watch,
		[ska_system_cursor_sizenwse]   = XC_top_left_corner,
		[ska_system_cursor_sizenesw]   = XC_top_right_corner,
		[ska_system_cursor_sizewe]     = XC_sb_h_double_arrow,
		[ska_system_cursor_sizens]     = XC_sb_v_double_arrow,
		[ska_system_cursor_sizeall]    = XC_fleur,
		[ska_system_cursor_no]         = XC_X_cursor,
		[ska_system_cursor_hand]       = XC_hand2,
	};

	if (cursor >= ska_system_cursor_count_) {
		return;
	}

	if (g_x_cursors[cursor] == None) {
		// Try themed cursor first
		g_x_cursors[cursor] = XcursorLibraryLoadCursor(g_ska.x_display, xcursor_names[cursor]);

		// Fall back to X11 cursor font
		if (g_x_cursors[cursor] == None) {
			g_x_cursors[cursor] = XCreateFontCursor(g_ska.x_display, x11_cursors[cursor]);
		}
	}

	// Remember current cursor for ska_platform_show_cursor
	g_current_cursor = cursor;

	// Apply to all windows
	for (uint32_t i = 0; i < SKA_MAX_WINDOWS; i++) {
		if (g_ska.windows[i]) {
			XDefineCursor(g_ska.x_display, g_ska.windows[i]->xwindow, g_x_cursors[cursor]);
		}
	}
	XFlush(g_ska.x_display);
}

void ska_platform_show_cursor(bool show) {
	if (show) {
		// Restore the current cursor (don't use XUndefineCursor which resets to parent's cursor)
		for (uint32_t i = 0; i < SKA_MAX_WINDOWS; i++) {
			if (g_ska.windows[i]) {
				XDefineCursor(g_ska.x_display, g_ska.windows[i]->xwindow, g_x_cursors[g_current_cursor]);
			}
		}
	} else {
		// Create invisible cursor
		static Cursor invisible_cursor = None;
		if (invisible_cursor == None) {
			char data[1] = {0};
			Pixmap blank = XCreateBitmapFromData(g_ska.x_display, g_ska.x_root, data, 1, 1);
			XColor color = {0};
			invisible_cursor = XCreatePixmapCursor(g_ska.x_display, blank, blank, &color, &color, 0, 0);
			XFreePixmap(g_ska.x_display, blank);
		}

		for (uint32_t i = 0; i < SKA_MAX_WINDOWS; i++) {
			if (g_ska.windows[i]) {
				XDefineCursor(g_ska.x_display, g_ska.windows[i]->xwindow, invisible_cursor);
			}
		}
	}
	XFlush(g_ska.x_display);
}

bool ska_platform_set_relative_mouse_mode(bool enabled) {
	// Just hide cursor and center it in relative mode
	ska_platform_show_cursor(!enabled);
	return true;
}

void ska_platform_pump_events(void) {
	while (XPending(g_ska.x_display)) {
		XEvent xev;
		XNextEvent(g_ska.x_display, &xev);

		// Filter through input method first
		if (XFilterEvent(&xev, None)) {
			continue;
		}

		// Handle root window events (DPI change detection)
		if (xev.xany.window == g_ska.x_root) {
			if (xev.type == PropertyNotify && xev.xproperty.atom == g_ska.resource_manager) {
				// RESOURCE_MANAGER changed - check if DPI scale changed
				float new_scale = ska_platform_get_dpi_scale(NULL);
				if (new_scale != g_ska.cached_dpi_scale && g_ska.cached_dpi_scale > 0.0f) {
					g_ska.cached_dpi_scale = new_scale;

					// Send DPI changed event to all windows
					for (uint32_t i = 0; i < SKA_MAX_WINDOWS; i++) {
						ska_window_t* win = g_ska.windows[i];
						if (win) {
							win->dpi_scale = new_scale;

							ska_event_t event = {0};
							event.timestamp          = (uint32_t)ska_time_get_elapsed_ms();
							event.type               = ska_event_window_dpi_changed;
							event.window.window_id   = win->id;
							event.window.data1       = (int32_t)(new_scale * 100.0f + 0.5f);
							ska_post_event(&event);
						}
					}
				}
			}
			continue;
		}

		ska_window_t* window = ska_find_window_by_xwindow(xev.xany.window);
		if (!window) {
			continue;
		}

		ska_event_t event = {0};
		event.timestamp = (uint32_t)ska_time_get_elapsed_ms();

		switch (xev.type) {
			case KeyPress:
			case KeyRelease: {
				event.type = (xev.type == KeyPress) ? ska_event_key_down : ska_event_key_up;
				event.keyboard.window_id = window->id;
				event.keyboard.pressed = (xev.type == KeyPress);
				event.keyboard.repeat = false; // X11 sends release+press for repeats
				event.keyboard.scancode = ska_x11_scancode_table[xev.xkey.keycode];
				event.keyboard.modifiers = ska_x11_get_modifiers(xev.xkey.state);

				// Update input state
				if (event.keyboard.scancode != ska_scancode_unknown) {
					g_ska.input_state.keyboard[event.keyboard.scancode] = event.keyboard.pressed ? 1 : 0;
				}
				g_ska.input_state.key_modifiers = event.keyboard.modifiers;

				ska_post_event(&event);

				// Handle text input
				if (xev.type == KeyPress && window->xic) {
					char buffer[32];
					KeySym keysym;
					Status status;
					int32_t len = Xutf8LookupString(window->xic, &xev.xkey, buffer, sizeof(buffer) - 1, &keysym, &status);
					if (len > 0 && (status == XLookupChars || status == XLookupBoth)) {
						buffer[len] = '\0';
						event.type = ska_event_text_input;
						event.text.window_id = window->id;
						strncpy(event.text.text, buffer, sizeof(event.text.text) - 1);
						ska_post_event(&event);
					}
				}
				break;
			}

			case ButtonPress:
			case ButtonRelease: {
				if (xev.xbutton.button >= Button4 && xev.xbutton.button <= 7) {
					// Mouse wheel (vertical: Button4/Button5, horizontal: Button6/Button7)
					if (xev.type == ButtonPress) {
						event.type = ska_event_mouse_wheel;
						event.mouse_wheel.window_id = window->id;

						if (xev.xbutton.button == Button4 || xev.xbutton.button == Button5) {
							// Vertical scroll
							event.mouse_wheel.x = 0;
							event.mouse_wheel.y = (xev.xbutton.button == Button4) ? 1 : -1;
							event.mouse_wheel.precise_x = 0.0f;
							event.mouse_wheel.precise_y = (float)event.mouse_wheel.y;
						} else {
							// Horizontal scroll (Button6 = left, Button7 = right)
							event.mouse_wheel.x = (xev.xbutton.button == 6) ? -1 : 1;
							event.mouse_wheel.y = 0;
							event.mouse_wheel.precise_x = (float)event.mouse_wheel.x;
							event.mouse_wheel.precise_y = 0.0f;
						}
						ska_post_event(&event);
					}
				} else {
					// Mouse button
					event.type = (xev.type == ButtonPress) ? ska_event_mouse_button_down : ska_event_mouse_button_up;
					event.mouse_button.window_id = window->id;
					event.mouse_button.button = xev.xbutton.button;
					event.mouse_button.pressed = (xev.type == ButtonPress);
					event.mouse_button.clicks = 1;
					event.mouse_button.x = xev.xbutton.x;
					event.mouse_button.y = xev.xbutton.y;

					// Update button state
					uint32_t button_mask = (1 << (xev.xbutton.button - 1));
					if (event.mouse_button.pressed) {
						g_ska.input_state.mouse_buttons |= button_mask;
					} else {
						g_ska.input_state.mouse_buttons &= ~button_mask;
					}

					ska_post_event(&event);
				}
				break;
			}

			case MotionNotify: {
				if (window->mouse_warped) {
					window->mouse_warped = false;
					break;
				}

				event.type = ska_event_mouse_motion;
				event.mouse_motion.window_id = window->id;
				event.mouse_motion.x = xev.xmotion.x;
				event.mouse_motion.y = xev.xmotion.y;
				event.mouse_motion.xrel = xev.xmotion.x - g_ska.input_state.mouse_x;
				event.mouse_motion.yrel = xev.xmotion.y - g_ska.input_state.mouse_y;

				g_ska.input_state.mouse_x = xev.xmotion.x;
				g_ska.input_state.mouse_y = xev.xmotion.y;
				g_ska.input_state.mouse_xrel = event.mouse_motion.xrel;
				g_ska.input_state.mouse_yrel = event.mouse_motion.yrel;

				ska_post_event(&event);
				break;
			}

			case EnterNotify:
				event.type = ska_event_window_mouse_enter;
				event.window.window_id = window->id;
				window->mouse_inside = true;
				ska_post_event(&event);
				break;

			case LeaveNotify:
				event.type = ska_event_window_mouse_leave;
				event.window.window_id = window->id;
				window->mouse_inside = false;
				ska_post_event(&event);
				break;

			case FocusIn:
				event.type = ska_event_window_focus_gained;
				event.window.window_id = window->id;
				window->has_focus = true;
				if (window->xic) {
					XSetICFocus(window->xic);
				}
				ska_post_event(&event);
				break;

			case FocusOut:
				event.type = ska_event_window_focus_lost;
				event.window.window_id = window->id;
				window->has_focus = false;
				if (window->xic) {
					XUnsetICFocus(window->xic);
				}
				ska_post_event(&event);
				break;

			case ConfigureNotify:
				if (xev.xconfigure.width != window->width || xev.xconfigure.height != window->height) {
					event.type = ska_event_window_resized;
					event.window.window_id = window->id;
					event.window.data1 = xev.xconfigure.width;
					event.window.data2 = xev.xconfigure.height;
					window->width = xev.xconfigure.width;
					window->height = xev.xconfigure.height;
					window->drawable_width = xev.xconfigure.width;
					window->drawable_height = xev.xconfigure.height;
					ska_post_event(&event);
				}
				{
					// ConfigureNotify gives position relative to parent (WM frame).
					// Translate to root coordinates for actual screen position.
					Window child;
					int32_t root_x, root_y;
					XTranslateCoordinates(g_ska.x_display, window->xwindow, g_ska.x_root, 0, 0, &root_x, &root_y, &child);

					if (root_x != window->x || root_y != window->y) {
						event.type = ska_event_window_moved;
						event.window.window_id = window->id;
						event.window.data1 = root_x;
						event.window.data2 = root_y;
						window->x = root_x;
						window->y = root_y;
						ska_post_event(&event);
					}
				}
				break;

			case MapNotify:
				if (!window->is_visible) {
					event.type = ska_event_window_shown;
					event.window.window_id = window->id;
					window->is_visible = true;
					ska_post_event(&event);
				}
				break;

			case UnmapNotify:
				if (window->is_visible) {
					event.type = ska_event_window_hidden;
					event.window.window_id = window->id;
					window->is_visible = false;
					ska_post_event(&event);
				}
				break;

			case ClientMessage:
				if (xev.xclient.message_type == g_ska.wm_protocols &&
					(Atom)xev.xclient.data.l[0] == g_ska.wm_delete_window) {
					event.type = ska_event_window_close;
					event.window.window_id = window->id;
					window->should_close = true;
					ska_post_event(&event);
				}
				break;

			case SelectionRequest: {
				// Handle clipboard data requests from other applications
				XSelectionRequestEvent* req = &xev.xselectionrequest;

				// If property is None, use the target as the property (some apps do this)
				Atom property = req->property;
				if (property == None) {
					property = req->target;
				}

				XEvent response;
				memset(&response, 0, sizeof(response));
				response.xselection.type = SelectionNotify;
				response.xselection.requestor = req->requestor;
				response.xselection.selection = req->selection;
				response.xselection.target = req->target;
				response.xselection.time = req->time;
				response.xselection.property = None;

				Atom clipboard_atom = XInternAtom(g_ska.x_display, "CLIPBOARD", False);
				Atom utf8_atom = XInternAtom(g_ska.x_display, "UTF8_STRING", False);
				Atom text_atom = XInternAtom(g_ska.x_display, "TEXT", False);
				Atom string_atom = XA_STRING;
				Atom targets_atom = XInternAtom(g_ska.x_display, "TARGETS", False);
				Atom text_plain_atom = XInternAtom(g_ska.x_display, "text/plain", False);
				Atom text_plain_utf8_atom = XInternAtom(g_ska.x_display, "text/plain;charset=utf-8", False);
				Atom property_atom = XInternAtom(g_ska.x_display, "SKA_CLIPBOARD_DATA", False);

				if (req->selection == clipboard_atom) {
					// Handle TARGETS request - tell requestor what formats we support
					if (req->target == targets_atom) {
						Atom supported_targets[] = {
							targets_atom,
							utf8_atom,
							text_atom,
							string_atom,
							text_plain_atom,
							text_plain_utf8_atom
						};
						XChangeProperty(
							g_ska.x_display, req->requestor, property,
							XA_ATOM, 32, PropModeReplace,
							(unsigned char*)supported_targets, 6
						);
						response.xselection.property = property;
					}
					// Handle UTF8_STRING, TEXT, STRING, or MIME type requests
					else if (req->target == utf8_atom || req->target == text_atom || req->target == string_atom ||
					         req->target == text_plain_atom || req->target == text_plain_utf8_atom) {
						// Check if we have clipboard data stored
						Atom actual_type;
						int32_t actual_format;
						unsigned long nitems, bytes_after;
						unsigned char* data = NULL;

						int32_t result = XGetWindowProperty(
							g_ska.x_display, window->xwindow, property_atom,
							0, 0x1FFFFFFF, False, AnyPropertyType,
							&actual_type, &actual_format, &nitems, &bytes_after, &data
						);

						if (result == Success && data && nitems > 0) {
							// We have data - send it to the requestor
							XChangeProperty(
								g_ska.x_display, req->requestor, property,
								req->target, 8, PropModeReplace,
								data, nitems
							);
							response.xselection.property = property;
							XFree(data);
						}
					}
				}

				XSendEvent(g_ska.x_display, req->requestor, False, 0, &response);
				XFlush(g_ska.x_display);
				break;
			}
		}
	}
}

/////////////////////////////////////////
// X11 specific subset of Vulkan header
/////////////////////////////////////////

typedef VkFlags VkXlibSurfaceCreateFlagsKHR;
typedef struct VkXlibSurfaceCreateInfoKHR {
	VkStructureType                sType;
	const void*                    pNext;
	VkXlibSurfaceCreateFlagsKHR    flags;
	Display*                       dpy;
	Window                         window;
} VkXlibSurfaceCreateInfoKHR;

typedef VkResult (VKAPI_PTR *PFN_vkCreateXlibSurfaceKHR)(VkInstance instance, const VkXlibSurfaceCreateInfoKHR* pCreateInfo, const /*VkAllocationCallbacks*/ void* pAllocator, VkSurfaceKHR* pSurface);

/////////////////////////////////////////

const char** ska_platform_vk_get_instance_extensions(uint32_t* out_count) {
	static const char* extensions[] = {
		"VK_KHR_surface",
		"VK_KHR_xlib_surface"
	};
	*out_count = 2;
	return extensions;
}

bool ska_platform_vk_create_surface(const ska_window_t* window, VkInstance instance, VkSurfaceKHR* out_surface) {
	void* module = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
	if (!module) module = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
	if (!module) {
		ska_set_error("Failed to load Vulkan .so");
		return false;
	}

	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(module, "vkGetInstanceProcAddr");
	if (!vkGetInstanceProcAddr) {
		ska_set_error("Failed to load vkGetInstanceProcAddr");
		return false;
	}

	PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateXlibSurfaceKHR");
	if (!vkCreateXlibSurfaceKHR) {
		ska_set_error("Failed to load vkCreateXlibSurfaceKHR");
		return false;
	}

	VkXlibSurfaceCreateInfoKHR create_info = {0};
	create_info.sType  = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	create_info.dpy    = g_ska.x_display;
	create_info.window = window->xwindow;

	VkResult result = vkCreateXlibSurfaceKHR(instance, &create_info, NULL, out_surface);
	if (result != VK_SUCCESS) {
		ska_set_error("Failed to create Vulkan Xlib surface: %d", result);
		return false;
	}

	return true;
}

// ========== Text Input Platform Functions ==========

void ska_platform_show_virtual_keyboard(bool visible, ska_text_input_type_ type) {
	// Linux X11 - desktop platform, no virtual keyboard
	(void)visible;
	(void)type;
}

// ========== Clipboard Platform Functions ==========

size_t ska_platform_clipboard_get_text(char* opt_out_buffer, size_t buffer_size) {
	if (!g_ska.x_display) {
		return 0;
	}

	Atom clipboard_atom = XInternAtom(g_ska.x_display, "CLIPBOARD", False);
	Atom utf8_atom = XInternAtom(g_ska.x_display, "UTF8_STRING", False);
	Atom property_atom = XInternAtom(g_ska.x_display, "XSEL_DATA", False);

	// Find a window to use for selection requests (use first available window)
	Window window = None;
	for (uint32_t i = 0; i < SKA_MAX_WINDOWS; i++) {
		if (g_ska.windows[i]) {
			window = g_ska.windows[i]->xwindow;
			break;
		}
	}

	if (window == None) {
		return 0;
	}

	// Request clipboard content
	XConvertSelection(g_ska.x_display, clipboard_atom, utf8_atom, property_atom, window, CurrentTime);
	XFlush(g_ska.x_display);

	// Wait for SelectionNotify event with timeout
	XEvent event;
	bool received = false;
	const int32_t timeout_ms = 500;
	const int32_t start_time = ska_time_get_elapsed_ms();

	while (ska_time_get_elapsed_ms() - start_time < timeout_ms) {
		if (XCheckTypedWindowEvent(g_ska.x_display, window, SelectionNotify, &event)) {
			received = true;
			break;
		}
		ska_time_sleep(1);
	}

	if (!received || event.xselection.property == None) {
		return 0;
	}

	// Get the property data
	Atom actual_type;
	int32_t actual_format;
	unsigned long nitems, bytes_after;
	unsigned char* data = NULL;

	int32_t result = XGetWindowProperty(
		g_ska.x_display, window, property_atom,
		0, 0x1FFFFFFF, False, AnyPropertyType,
		&actual_type, &actual_format, &nitems, &bytes_after, &data
	);

	if (result != Success || !data || nitems == 0) {
		if (data) XFree(data);
		XDeleteProperty(g_ska.x_display, window, property_atom);
		return 0;
	}

	// Calculate size including null terminator
	size_t text_size = nitems + 1;

	// If buffer is provided, copy the text
	if (opt_out_buffer && buffer_size > 0) {
		size_t copy_size = (text_size < buffer_size) ? text_size : buffer_size;
		memcpy(opt_out_buffer, data, copy_size - 1);
		opt_out_buffer[copy_size - 1] = '\0';
	}

	XFree(data);
	XDeleteProperty(g_ska.x_display, window, property_atom);

	return text_size;
}

bool ska_platform_clipboard_set_text(const char* text) {
	if (!g_ska.x_display || !text) {
		ska_set_error("ska_platform_clipboard_set_text: invalid display or text");
		return false;
	}

	Atom clipboard_atom = XInternAtom(g_ska.x_display, "CLIPBOARD", False);

	// Find a window to use as selection owner
	Window window = None;
	for (uint32_t i = 0; i < SKA_MAX_WINDOWS; i++) {
		if (g_ska.windows[i]) {
			window = g_ska.windows[i]->xwindow;
			break;
		}
	}

	if (window == None) {
		ska_set_error("ska_platform_clipboard_set_text: no window available");
		return false;
	}

	// Store the text in a window property
	Atom property_atom = XInternAtom(g_ska.x_display, "SKA_CLIPBOARD_DATA", False);
	Atom utf8_atom = XInternAtom(g_ska.x_display, "UTF8_STRING", False);

	XChangeProperty(
		g_ska.x_display, window, property_atom,
		utf8_atom, 8, PropModeReplace,
		(const unsigned char*)text, strlen(text)
	);

	// Take ownership of the clipboard
	XSetSelectionOwner(g_ska.x_display, clipboard_atom, window, CurrentTime);
	XFlush(g_ska.x_display);

	// Verify ownership
	Window owner = XGetSelectionOwner(g_ska.x_display, clipboard_atom);
	if (owner != window) {
		ska_set_error("ska_platform_clipboard_set_text: failed to acquire clipboard ownership");
		return false;
	}

	return true;
}

#endif // SKA_PLATFORM_LINUX
