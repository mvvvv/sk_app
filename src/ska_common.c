//
// sk_app - Common implementation

#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include "ska_internal.h"
#include <stdarg.h>
#include <time.h>

#ifdef SKA_PLATFORM_WIN32
#include <timeapi.h>
#pragma comment(lib, "winmm.lib")
#else
#include <unistd.h>
#endif

#ifdef SKA_PLATFORM_LINUX
#include <sys/time.h>
#include <unistd.h>
#endif

#ifdef SKA_PLATFORM_MACOS
#include <mach/mach_time.h>
#include <unistd.h>
#endif

// Global state
ska_state_t g_ska = {0};

// ============================================================================
// Error Handling
// ============================================================================

void ska_set_error(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsnprintf(g_ska.error_msg, sizeof(g_ska.error_msg), fmt, args);
	va_end(args);
	ska_log(ska_log_error, "%s", g_ska.error_msg);
}

SKA_API const char* ska_error_get(void) {
	return g_ska.error_msg[0] != '\0' ? g_ska.error_msg : NULL;
}

// ============================================================================
// Initialization
// ============================================================================

SKA_API bool ska_init(void) {
	if (g_ska.initialized) {
		ska_set_error("sk_app already initialized");
		return false;
	}

#ifdef SKA_PLATFORM_ANDROID
	// Preserve android_app pointer that was set in android_main()
	struct android_app* saved_android_app = g_ska.android_app;
#endif

	memset(&g_ska, 0, sizeof(g_ska));
	g_ska.start_time = ska_get_time_ms();
	g_ska.next_window_id = 1;

#ifdef SKA_PLATFORM_ANDROID
	// Restore android_app pointer
	g_ska.android_app = saved_android_app;
#endif

	ska_event_queue_init(&g_ska.event_queue);
	ska_input_state_init(&g_ska.input_state);

	if (!ska_platform_init()) {
		return false;
	}

	g_ska.initialized = true;
	ska_log(ska_log_info, "sk_app initialized");
	return true;
}

SKA_API void ska_shutdown(void) {
	if (!g_ska.initialized) {
		return;
	}

	// Destroy all windows
	for (uint32_t i = 0; i < SKA_MAX_WINDOWS; i++) {
		if (g_ska.windows[i]) {
			ska_window_destroy(g_ska.windows[i]);
		}
	}

	ska_platform_shutdown();

	g_ska.initialized = false;
	ska_log(ska_log_info, "sk_app shutdown");
}

// ============================================================================
// Window Management
// ============================================================================

ska_window_t* ska_window_alloc(void) {
	if (g_ska.window_count >= SKA_MAX_WINDOWS) {
		ska_set_error("Maximum number of windows (%d) reached", SKA_MAX_WINDOWS);
		return NULL;
	}

	ska_window_t* window = (ska_window_t*)calloc(1, sizeof(ska_window_t));
	if (!window) {
		ska_set_error("Failed to allocate window structure");
		return NULL;
	}

	window->id = g_ska.next_window_id++;
	window->is_visible = true;

	// Find free slot
	for (uint32_t i = 0; i < SKA_MAX_WINDOWS; i++) {
		if (!g_ska.windows[i]) {
			g_ska.windows[i] = window;
			g_ska.window_count++;
			return window;
		}
	}

	free(window);
	ska_set_error("Internal error: no free window slot");
	return NULL;
}

void ska_window_free(ska_window_t* ref_window) {
	if (!ref_window) return;

	// Remove from window array
	for (uint32_t i = 0; i < SKA_MAX_WINDOWS; i++) {
		if (g_ska.windows[i] == ref_window) {
			g_ska.windows[i] = NULL;
			g_ska.window_count--;
			break;
		}
	}

	if (ref_window->title) {
		free(ref_window->title);
	}

	free(ref_window);
}

SKA_API ska_window_t* ska_window_create(
	const char* title,
	int32_t x, int32_t y,
	int32_t width, int32_t height,
	uint32_t flags
) {
	if (!g_ska.initialized) {
		ska_set_error("sk_app not initialized");
		return NULL;
	}

	if (!title) title = "sk_app window";
	if (width <= 0) width = 640;
	if (height <= 0) height = 480;

	ska_window_t* window = ska_window_alloc();
	if (!window) {
		return NULL;
	}

	window->flags = flags;
	window->width = width;
	window->height = height;

	if (x == SKA_WINDOWPOS_UNDEFINED) {
		x = 100; // Default position
	} else if (x == SKA_WINDOWPOS_CENTERED) {
		x = -1; // Platform will center
	}

	if (y == SKA_WINDOWPOS_UNDEFINED) {
		y = 100;
	} else if (y == SKA_WINDOWPOS_CENTERED) {
		y = -1;
	}

	window->x = x;
	window->y = y;

	if (!ska_platform_window_create(window, title, x, y, width, height, flags)) {
		ska_window_free(window);
		return NULL;
	}

	if (!(flags & ska_window_hidden)) {
		ska_platform_window_show(window);
	}

	return window;
}

SKA_API void ska_window_destroy(ska_window_t* ref_window) {
	if (!ref_window) return;

	ska_platform_window_destroy(ref_window);
	ska_window_free(ref_window);
}

SKA_API ska_window_id_t ska_window_get_id(const ska_window_t* window) {
	return window ? window->id : 0;
}

SKA_API ska_window_t* ska_window_from_id(ska_window_id_t id) {
	for (uint32_t i = 0; i < SKA_MAX_WINDOWS; i++) {
		if (g_ska.windows[i] && g_ska.windows[i]->id == id) {
			return g_ska.windows[i];
		}
	}
	return NULL;
}

SKA_API void ska_window_set_title(ska_window_t* ref_window, const char* title) {
	if (!ref_window || !title) return;
	ska_platform_window_set_title(ref_window, title);
}

SKA_API const char* ska_window_get_title(const ska_window_t* window) {
	return window ? window->title : NULL;
}

SKA_API void ska_window_set_position(ska_window_t* ref_window, int32_t x, int32_t y) {
	if (!ref_window) return;
	ska_platform_window_set_position(ref_window, x, y);
}

SKA_API void ska_window_get_position(const ska_window_t* window, int32_t* opt_out_x, int32_t* opt_out_y) {
	if (!window) return;
	if (opt_out_x) *opt_out_x = window->x;
	if (opt_out_y) *opt_out_y = window->y;
}

SKA_API void ska_window_set_size(ska_window_t* ref_window, int32_t width, int32_t height) {
	if (!ref_window) return;
	ska_platform_window_set_size(ref_window, width, height);
}

SKA_API void ska_window_get_size(const ska_window_t* window, int32_t* opt_out_width, int32_t* opt_out_height) {
	if (!window) return;
	if (opt_out_width) *opt_out_width = window->width;
	if (opt_out_height) *opt_out_height = window->height;
}

SKA_API void ska_window_get_drawable_size(ska_window_t* ref_window, int32_t* opt_out_width, int32_t* opt_out_height) {
	if (!ref_window) return;
	ska_platform_window_get_drawable_size(ref_window, opt_out_width, opt_out_height);
	if (opt_out_width) *opt_out_width = ref_window->drawable_width;
	if (opt_out_height) *opt_out_height = ref_window->drawable_height;
}

SKA_API void ska_window_show(ska_window_t* ref_window) {
	if (!ref_window) return;
	ska_platform_window_show(ref_window);
}

SKA_API void ska_window_hide(ska_window_t* ref_window) {
	if (!ref_window) return;
	ska_platform_window_hide(ref_window);
}

SKA_API void ska_window_maximize(ska_window_t* ref_window) {
	if (!ref_window) return;
	ska_platform_window_maximize(ref_window);
}

SKA_API void ska_window_minimize(ska_window_t* ref_window) {
	if (!ref_window) return;
	ska_platform_window_minimize(ref_window);
}

SKA_API void ska_window_restore(ska_window_t* ref_window) {
	if (!ref_window) return;
	ska_platform_window_restore(ref_window);
}

SKA_API void ska_window_raise(ska_window_t* ref_window) {
	if (!ref_window) return;
	ska_platform_window_raise(ref_window);
}

SKA_API uint32_t ska_window_get_flags(const ska_window_t* window) {
	return window ? window->flags : 0;
}

SKA_API void* ska_window_get_native_handle(const ska_window_t* window) {
	if (!window) return NULL;

#ifdef SKA_PLATFORM_WIN32
	return (void*)window->hwnd;
#elif defined(SKA_PLATFORM_LINUX)
	return (void*)(uintptr_t)window->xwindow;
#elif defined(SKA_PLATFORM_MACOS)
	return window->ns_window;
#elif defined(SKA_PLATFORM_ANDROID)
	return window->native_window;
#else
	return NULL;
#endif
}

// ============================================================================
// Event System
// ============================================================================

void ska_post_event(const ska_event_t* event) {
	if (!ska_event_queue_push(&g_ska.event_queue, event)) {
		ska_log(ska_log_warn, "Event queue full, dropping event type %d", event->type);
	}
}

SKA_API bool ska_event_poll(ska_event_t* out_event) {
	if (!g_ska.initialized || !out_event) {
		return false;
	}

	// Process platform events first
	ska_platform_pump_events();

	bool has_event = ska_event_queue_pop(&g_ska.event_queue, out_event);

	// Feed text input events to the text queue
	if (has_event && out_event->type == ska_event_text_input) {
		ska_text_queue_push_utf8(&g_ska.input_state.text_queue, out_event->text.text);
	}

	return has_event;
}

SKA_API bool ska_event_wait(ska_event_t* out_event) {
	return ska_event_wait_timeout(out_event, -1);
}

SKA_API bool ska_event_wait_timeout(ska_event_t* out_event, int32_t timeout_ms) {
	if (!g_ska.initialized || !out_event) {
		return false;
	}

	uint64_t start = ska_time_get_elapsed_ms();

	while (true) {
		if (ska_event_poll(out_event)) {
			return true;
		}

		if (timeout_ms == 0) {
			return false;
		}

		if (timeout_ms > 0) {
			uint64_t elapsed = ska_time_get_elapsed_ms() - start;
			if (elapsed >= (uint64_t)timeout_ms) {
				return false;
			}
		}

		ska_time_sleep(1);
	}
}

// ============================================================================
// Input State
// ============================================================================

SKA_API const uint8_t* ska_keyboard_get_state(int32_t* opt_out_num_keys) {
	if (opt_out_num_keys) {
		*opt_out_num_keys = ska_scancode_count;
	}
	return g_ska.input_state.keyboard;
}

SKA_API uint16_t ska_keyboard_get_modifiers(void) {
	return g_ska.input_state.key_modifiers;
}

SKA_API uint32_t ska_mouse_get_state(int32_t* opt_out_x, int32_t* opt_out_y) {
	if (opt_out_x) *opt_out_x = g_ska.input_state.mouse_x;
	if (opt_out_y) *opt_out_y = g_ska.input_state.mouse_y;
	return g_ska.input_state.mouse_buttons;
}

SKA_API uint32_t ska_mouse_get_global_state(int32_t* opt_out_x, int32_t* opt_out_y) {
	// Platform-specific implementation would query OS directly
	return ska_mouse_get_state(opt_out_x, opt_out_y);
}

SKA_API void ska_mouse_warp(ska_window_t* ref_window, int32_t x, int32_t y) {
	if (!ref_window) return;
	ska_platform_warp_mouse(ref_window, x, y);
}

SKA_API void ska_cursor_show(bool show) {
	ska_platform_show_cursor(show);
	g_ska.input_state.cursor_visible = show;
}

SKA_API bool ska_mouse_set_relative_mode(bool enabled) {
	if (ska_platform_set_relative_mouse_mode(enabled)) {
		g_ska.input_state.relative_mouse_mode = enabled;
		return true;
	}
	return false;
}

SKA_API bool ska_mouse_get_relative_mode(void) {
	return g_ska.input_state.relative_mouse_mode;
}

// ============================================================================
// Vulkan Support
// ============================================================================

SKA_API const char** ska_vk_get_instance_extensions(uint32_t* out_count) {
	if (!out_count) return NULL;
	return ska_platform_vk_get_instance_extensions(out_count);
}

SKA_API bool ska_vk_create_surface(const ska_window_t* window, void* instance, void* out_surface) {
	if (!window || !instance || !out_surface) {
		ska_set_error("Invalid parameters for Vulkan surface creation");
		return false;
	}
	return ska_platform_vk_create_surface(window, (VkInstance)instance, (VkSurfaceKHR*)out_surface);
}

// ============================================================================
// Utilities
// ============================================================================

uint64_t ska_get_time_ms(void) {
#ifdef SKA_PLATFORM_WIN32
	return GetTickCount64();
#elif defined(SKA_PLATFORM_MACOS)
	static mach_timebase_info_data_t timebase = {0};
	if (timebase.denom == 0) {
		mach_timebase_info(&timebase);
	}
	return (mach_absolute_time() * timebase.numer) / (timebase.denom * 1000000);
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#endif
}

SKA_API uint64_t ska_time_get_elapsed_ms(void) {
	return ska_get_time_ms() - g_ska.start_time;
}

SKA_API void ska_time_sleep(uint32_t ms) {
#ifdef SKA_PLATFORM_WIN32
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}

// ============================================================================
// Clipboard Support
// ============================================================================

SKA_API size_t ska_clipboard_get_text(char* opt_out_buffer, size_t buffer_size) {
	return ska_platform_clipboard_get_text(opt_out_buffer, buffer_size);
}

SKA_API bool ska_clipboard_set_text(const char* text) {
	if (!text) {
		ska_set_error("ska_clipboard_set_text: text cannot be NULL");
		return false;
	}
	return ska_platform_clipboard_set_text(text);
}

// ============================================================================
// Logging
// ============================================================================

SKA_API void ska_log(ska_log_ level, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

#ifdef SKA_PLATFORM_ANDROID
	int android_level;
	switch (level) {
		case ska_log_info:  android_level = ANDROID_LOG_INFO;  break;
		case ska_log_warn:  android_level = ANDROID_LOG_WARN;  break;
		case ska_log_error: android_level = ANDROID_LOG_ERROR; break;
		default:            android_level = ANDROID_LOG_INFO;  break;
	}
	__android_log_vprint(android_level, "sk_app", fmt, args);
#else
	FILE* output = (level == ska_log_error) ? stderr : stdout;
	const char* prefix;
	switch (level) {
		case ska_log_info:  prefix = "[INFO] ";  break;
		case ska_log_warn:  prefix = "[WARN] ";  break;
		case ska_log_error: prefix = "[ERROR] "; break;
		default:            prefix = "[LOG] ";   break;
	}
	fprintf(output, "%s", prefix);
	vfprintf(output, fmt, args);
	fprintf(output, "\n");
#endif

	va_end(args);
}

// ============================================================================
// Platform-Specific Exports
// ============================================================================

#ifdef SKA_PLATFORM_WIN32
SKA_API void* ska_win32_get_hinstance(void) {
	return g_ska.hinstance;
}
#endif

#ifdef SKA_PLATFORM_LINUX
SKA_API void* ska_linux_get_x11_display(void) {
	return g_ska.x_display;
}

SKA_API void* ska_linux_get_wayland_display(void) {
	// Not implemented yet
	return NULL;
}
#endif

#ifdef SKA_PLATFORM_ANDROID
SKA_API void ska_android_set_app(void* app) {
	g_ska.android_app = (struct android_app*)app;
}
#endif
