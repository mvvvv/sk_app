//
// sk_app - Android platform backend

#include "ska_internal.h"

#ifdef SKA_PLATFORM_ANDROID

#include <android_native_app_glue.h>
#include <android/native_window.h>
#include <android/input.h>
#include <android/keycodes.h>
#include <dlfcn.h>
#include <unistd.h>

// Scancode translation table (Android key codes to ska_scancode_)
static ska_scancode_ ska_android_scancode_table[256];

static void ska_init_scancode_table(void) {
	// Initialize all to unknown
	for (int32_t i = 0; i < 256; i++) {
		ska_android_scancode_table[i] = ska_scancode_unknown;
	}

	// Letters
	ska_android_scancode_table[AKEYCODE_A] = ska_scancode_a;
	ska_android_scancode_table[AKEYCODE_B] = ska_scancode_b;
	ska_android_scancode_table[AKEYCODE_C] = ska_scancode_c;
	ska_android_scancode_table[AKEYCODE_D] = ska_scancode_d;
	ska_android_scancode_table[AKEYCODE_E] = ska_scancode_e;
	ska_android_scancode_table[AKEYCODE_F] = ska_scancode_f;
	ska_android_scancode_table[AKEYCODE_G] = ska_scancode_g;
	ska_android_scancode_table[AKEYCODE_H] = ska_scancode_h;
	ska_android_scancode_table[AKEYCODE_I] = ska_scancode_i;
	ska_android_scancode_table[AKEYCODE_J] = ska_scancode_j;
	ska_android_scancode_table[AKEYCODE_K] = ska_scancode_k;
	ska_android_scancode_table[AKEYCODE_L] = ska_scancode_l;
	ska_android_scancode_table[AKEYCODE_M] = ska_scancode_m;
	ska_android_scancode_table[AKEYCODE_N] = ska_scancode_n;
	ska_android_scancode_table[AKEYCODE_O] = ska_scancode_o;
	ska_android_scancode_table[AKEYCODE_P] = ska_scancode_p;
	ska_android_scancode_table[AKEYCODE_Q] = ska_scancode_q;
	ska_android_scancode_table[AKEYCODE_R] = ska_scancode_r;
	ska_android_scancode_table[AKEYCODE_S] = ska_scancode_s;
	ska_android_scancode_table[AKEYCODE_T] = ska_scancode_t;
	ska_android_scancode_table[AKEYCODE_U] = ska_scancode_u;
	ska_android_scancode_table[AKEYCODE_V] = ska_scancode_v;
	ska_android_scancode_table[AKEYCODE_W] = ska_scancode_w;
	ska_android_scancode_table[AKEYCODE_X] = ska_scancode_x;
	ska_android_scancode_table[AKEYCODE_Y] = ska_scancode_y;
	ska_android_scancode_table[AKEYCODE_Z] = ska_scancode_z;

	// Numbers
	ska_android_scancode_table[AKEYCODE_0] = ska_scancode_0;
	ska_android_scancode_table[AKEYCODE_1] = ska_scancode_1;
	ska_android_scancode_table[AKEYCODE_2] = ska_scancode_2;
	ska_android_scancode_table[AKEYCODE_3] = ska_scancode_3;
	ska_android_scancode_table[AKEYCODE_4] = ska_scancode_4;
	ska_android_scancode_table[AKEYCODE_5] = ska_scancode_5;
	ska_android_scancode_table[AKEYCODE_6] = ska_scancode_6;
	ska_android_scancode_table[AKEYCODE_7] = ska_scancode_7;
	ska_android_scancode_table[AKEYCODE_8] = ska_scancode_8;
	ska_android_scancode_table[AKEYCODE_9] = ska_scancode_9;

	// Function keys
	ska_android_scancode_table[AKEYCODE_ENTER] = ska_scancode_return;
	ska_android_scancode_table[AKEYCODE_ESCAPE] = ska_scancode_escape;
	ska_android_scancode_table[AKEYCODE_BACK] = ska_scancode_escape;  // Back button = Escape
	ska_android_scancode_table[AKEYCODE_DEL] = ska_scancode_backspace;
	ska_android_scancode_table[AKEYCODE_TAB] = ska_scancode_tab;
	ska_android_scancode_table[AKEYCODE_SPACE] = ska_scancode_space;

	// Symbols
	ska_android_scancode_table[AKEYCODE_MINUS] = ska_scancode_minus;
	ska_android_scancode_table[AKEYCODE_EQUALS] = ska_scancode_equals;
	ska_android_scancode_table[AKEYCODE_LEFT_BRACKET] = ska_scancode_leftbracket;
	ska_android_scancode_table[AKEYCODE_RIGHT_BRACKET] = ska_scancode_rightbracket;
	ska_android_scancode_table[AKEYCODE_BACKSLASH] = ska_scancode_backslash;
	ska_android_scancode_table[AKEYCODE_SEMICOLON] = ska_scancode_semicolon;
	ska_android_scancode_table[AKEYCODE_APOSTROPHE] = ska_scancode_apostrophe;
	ska_android_scancode_table[AKEYCODE_GRAVE] = ska_scancode_grave;
	ska_android_scancode_table[AKEYCODE_COMMA] = ska_scancode_comma;
	ska_android_scancode_table[AKEYCODE_PERIOD] = ska_scancode_period;
	ska_android_scancode_table[AKEYCODE_SLASH] = ska_scancode_slash;

	ska_android_scancode_table[AKEYCODE_CAPS_LOCK] = ska_scancode_capslock;

	// F keys
	ska_android_scancode_table[AKEYCODE_F1] = ska_scancode_f1;
	ska_android_scancode_table[AKEYCODE_F2] = ska_scancode_f2;
	ska_android_scancode_table[AKEYCODE_F3] = ska_scancode_f3;
	ska_android_scancode_table[AKEYCODE_F4] = ska_scancode_f4;
	ska_android_scancode_table[AKEYCODE_F5] = ska_scancode_f5;
	ska_android_scancode_table[AKEYCODE_F6] = ska_scancode_f6;
	ska_android_scancode_table[AKEYCODE_F7] = ska_scancode_f7;
	ska_android_scancode_table[AKEYCODE_F8] = ska_scancode_f8;
	ska_android_scancode_table[AKEYCODE_F9] = ska_scancode_f9;
	ska_android_scancode_table[AKEYCODE_F10] = ska_scancode_f10;
	ska_android_scancode_table[AKEYCODE_F11] = ska_scancode_f11;
	ska_android_scancode_table[AKEYCODE_F12] = ska_scancode_f12;

	// Navigation
	ska_android_scancode_table[AKEYCODE_MOVE_HOME] = ska_scancode_home;
	ska_android_scancode_table[AKEYCODE_PAGE_UP] = ska_scancode_pageup;
	ska_android_scancode_table[AKEYCODE_FORWARD_DEL] = ska_scancode_delete;
	ska_android_scancode_table[AKEYCODE_MOVE_END] = ska_scancode_end;
	ska_android_scancode_table[AKEYCODE_PAGE_DOWN] = ska_scancode_pagedown;
	ska_android_scancode_table[AKEYCODE_DPAD_RIGHT] = ska_scancode_right;
	ska_android_scancode_table[AKEYCODE_DPAD_LEFT] = ska_scancode_left;
	ska_android_scancode_table[AKEYCODE_DPAD_DOWN] = ska_scancode_down;
	ska_android_scancode_table[AKEYCODE_DPAD_UP] = ska_scancode_up;

	// Modifiers
	ska_android_scancode_table[AKEYCODE_CTRL_LEFT] = ska_scancode_lctrl;
	ska_android_scancode_table[AKEYCODE_SHIFT_LEFT] = ska_scancode_lshift;
	ska_android_scancode_table[AKEYCODE_ALT_LEFT] = ska_scancode_lalt;
	ska_android_scancode_table[AKEYCODE_META_LEFT] = ska_scancode_lgui;
	ska_android_scancode_table[AKEYCODE_CTRL_RIGHT] = ska_scancode_rctrl;
	ska_android_scancode_table[AKEYCODE_SHIFT_RIGHT] = ska_scancode_rshift;
	ska_android_scancode_table[AKEYCODE_ALT_RIGHT] = ska_scancode_ralt;
	ska_android_scancode_table[AKEYCODE_META_RIGHT] = ska_scancode_rgui;
}

static uint16_t ska_android_get_modifiers(int32_t meta_state) {
	uint16_t mods = 0;
	if (meta_state & AMETA_SHIFT_ON) mods |= ska_keymod_shift;
	if (meta_state & AMETA_CTRL_ON) mods |= ska_keymod_ctrl;
	if (meta_state & AMETA_ALT_ON) mods |= ska_keymod_alt;
	if (meta_state & AMETA_META_ON) mods |= ska_keymod_gui;
	return mods;
}

// Command handler for app lifecycle events
static void ska_android_handle_cmd(struct android_app* app, int32_t cmd) {
	ska_event_t event = {0};
	event.timestamp = (uint32_t)ska_time_get_elapsed_ms();

	switch (cmd) {
		case APP_CMD_INIT_WINDOW:
			// Window has been created
			if (app->window != NULL && g_ska.window_count > 0) {
				ska_window_t* window = g_ska.windows[0];
				if (window) {
					window->native_window = app->window;

					int32_t width = ANativeWindow_getWidth(app->window);
					int32_t height = ANativeWindow_getHeight(app->window);

					window->width = width;
					window->height = height;
					window->drawable_width = width;
					window->drawable_height = height;

					event.type = ska_event_window_shown;
					event.window.window_id = window->id;
					window->is_visible = true;
					ska_post_event(&event);

					ska_log(ska_log_info, "Android window created: %dx%d", width, height);
				}
			}
			break;

		case APP_CMD_TERM_WINDOW:
			// Window is being destroyed
			if (g_ska.window_count > 0) {
				ska_window_t* window = g_ska.windows[0];
				if (window) {
					event.type = ska_event_window_hidden;
					event.window.window_id = window->id;
					window->is_visible = false;
					ska_post_event(&event);

					window->native_window = NULL;
				}
			}
			break;

		case APP_CMD_WINDOW_RESIZED:
		case APP_CMD_CONFIG_CHANGED:
			// Window has been resized
			if (app->window != NULL && g_ska.window_count > 0) {
				ska_window_t* window = g_ska.windows[0];
				if (window) {
					int32_t width = ANativeWindow_getWidth(app->window);
					int32_t height = ANativeWindow_getHeight(app->window);

					if (width != window->width || height != window->height) {
						event.type = ska_event_window_resized;
						event.window.window_id = window->id;
						event.window.data1 = width;
						event.window.data2 = height;
						window->width = width;
						window->height = height;
						window->drawable_width = width;
						window->drawable_height = height;
						ska_post_event(&event);

						ska_log(ska_log_info, "Android window resized: %dx%d", width, height);
					}
				}
			}
			break;

		case APP_CMD_GAINED_FOCUS:
			g_ska.app_has_focus = true;
			if (g_ska.window_count > 0) {
				ska_window_t* window = g_ska.windows[0];
				if (window) {
					event.type = ska_event_window_focus_gained;
					event.window.window_id = window->id;
					window->has_focus = true;
					ska_post_event(&event);
				}
			}
			break;

		case APP_CMD_LOST_FOCUS:
			g_ska.app_has_focus = false;
			if (g_ska.window_count > 0) {
				ska_window_t* window = g_ska.windows[0];
				if (window) {
					event.type = ska_event_window_focus_lost;
					event.window.window_id = window->id;
					window->has_focus = false;
					ska_post_event(&event);
				}
			}
			break;

		case APP_CMD_PAUSE:
			event.type = ska_event_app_background;
			g_ska.app_is_visible = false;
			ska_post_event(&event);
			ska_log(ska_log_info, "App paused");
			break;

		case APP_CMD_RESUME:
			event.type = ska_event_app_foreground;
			g_ska.app_is_visible = true;
			ska_post_event(&event);
			ska_log(ska_log_info, "App resumed");
			break;

		case APP_CMD_LOW_MEMORY:
			event.type = ska_event_app_lowmemory;
			ska_post_event(&event);
			ska_log(ska_log_warn, "Low memory warning");
			break;

		case APP_CMD_DESTROY:
			event.type = ska_event_quit;
			ska_post_event(&event);
			ska_log(ska_log_info, "App destroy requested");
			break;
	}
}

// Input event handler
static int32_t ska_android_handle_input(struct android_app* app, AInputEvent* input_event) {
	if (g_ska.window_count == 0) {
		return 0;
	}

	ska_window_t* window = g_ska.windows[0];
	if (!window) {
		return 0;
	}

	ska_event_t event = {0};
	event.timestamp = (uint32_t)ska_time_get_elapsed_ms();

	int32_t event_type = AInputEvent_getType(input_event);

	if (event_type == AINPUT_EVENT_TYPE_KEY) {
		// Keyboard event
		int32_t action = AKeyEvent_getAction(input_event);
		int32_t keycode = AKeyEvent_getKeyCode(input_event);
		int32_t meta_state = AKeyEvent_getMetaState(input_event);

		bool pressed = (action == AKEY_EVENT_ACTION_DOWN);
		bool repeat = (action == AKEY_EVENT_ACTION_MULTIPLE);

		event.type = pressed ? ska_event_key_down : ska_event_key_up;
		event.keyboard.window_id = window->id;
		event.keyboard.pressed = pressed;
		event.keyboard.repeat = repeat;
		event.keyboard.scancode = ska_android_scancode_table[keycode];
		event.keyboard.modifiers = ska_android_get_modifiers(meta_state);

		// Update input state
		if (event.keyboard.scancode != ska_scancode_unknown) {
			g_ska.input_state.keyboard[event.keyboard.scancode] = pressed ? 1 : 0;
		}
		g_ska.input_state.key_modifiers = event.keyboard.modifiers;

		ska_post_event(&event);
		return 1;

	} else if (event_type == AINPUT_EVENT_TYPE_MOTION) {
		// Touch/Mouse event
		int32_t action = AMotionEvent_getAction(input_event);
		int32_t action_masked = action & AMOTION_EVENT_ACTION_MASK;

		float x = AMotionEvent_getX(input_event, 0);
		float y = AMotionEvent_getY(input_event, 0);

		switch (action_masked) {
			case AMOTION_EVENT_ACTION_DOWN:
			case AMOTION_EVENT_ACTION_POINTER_DOWN:
				// Touch down = Left mouse button down
				event.type = ska_event_mouse_button_down;
				event.mouse_button.window_id = window->id;
				event.mouse_button.button = ska_mouse_button_left;
				event.mouse_button.pressed = true;
				event.mouse_button.clicks = 1;
				event.mouse_button.x = (int32_t)x;
				event.mouse_button.y = (int32_t)y;

				g_ska.input_state.mouse_buttons |= (1 << (ska_mouse_button_left - 1));
				g_ska.input_state.mouse_x = (int32_t)x;
				g_ska.input_state.mouse_y = (int32_t)y;

				ska_post_event(&event);
				return 1;

			case AMOTION_EVENT_ACTION_UP:
			case AMOTION_EVENT_ACTION_POINTER_UP:
				// Touch up = Left mouse button up
				event.type = ska_event_mouse_button_up;
				event.mouse_button.window_id = window->id;
				event.mouse_button.button = ska_mouse_button_left;
				event.mouse_button.pressed = false;
				event.mouse_button.clicks = 1;
				event.mouse_button.x = (int32_t)x;
				event.mouse_button.y = (int32_t)y;

				g_ska.input_state.mouse_buttons &= ~(1 << (ska_mouse_button_left - 1));
				g_ska.input_state.mouse_x = (int32_t)x;
				g_ska.input_state.mouse_y = (int32_t)y;

				ska_post_event(&event);
				return 1;

			case AMOTION_EVENT_ACTION_MOVE:
				// Touch move = Mouse motion
				event.type = ska_event_mouse_motion;
				event.mouse_motion.window_id = window->id;
				event.mouse_motion.x = (int32_t)x;
				event.mouse_motion.y = (int32_t)y;
				event.mouse_motion.xrel = (int32_t)x - g_ska.input_state.mouse_x;
				event.mouse_motion.yrel = (int32_t)y - g_ska.input_state.mouse_y;

				g_ska.input_state.mouse_x = (int32_t)x;
				g_ska.input_state.mouse_y = (int32_t)y;
				g_ska.input_state.mouse_xrel = event.mouse_motion.xrel;
				g_ska.input_state.mouse_yrel = event.mouse_motion.yrel;

				ska_post_event(&event);
				return 1;
		}
	}

	return 0;
}

bool ska_platform_init(void) {
	if (!g_ska.android_app) {
		ska_set_error("android_app not set - call ska_android_set_app() before ska_init()");
		return false;
	}

	// Set up callbacks
	g_ska.android_app->onAppCmd = ska_android_handle_cmd;
	g_ska.android_app->onInputEvent = ska_android_handle_input;

	// Initialize scancode table
	ska_init_scancode_table();

	ska_log(ska_log_info, "Android platform initialized");

	return true;
}

void ska_platform_shutdown(void) {
	if (g_ska.android_app) {
		g_ska.android_app->onAppCmd = NULL;
		g_ska.android_app->onInputEvent = NULL;
	}

	ska_log(ska_log_info, "Android platform shutdown");
}

bool ska_platform_window_create(
	ska_window_t* window,
	const char* title,
	int32_t x, int32_t y,
	int32_t w, int32_t h,
	uint32_t flags
) {
	(void)x; (void)y; (void)w; (void)h; (void)flags;

	// On Android, we don't create windows - the system provides one
	// We'll use the ANativeWindow when it becomes available

	window->title = strdup(title ? title : "sk_app");

	// Window dimensions will be set when we get APP_CMD_INIT_WINDOW
	window->native_window = NULL;
	window->is_visible = false;

	// Wait for window to be created by the system
	ska_log(ska_log_info, "Android window stub created, waiting for native window");

	return true;
}

void ska_platform_window_destroy(ska_window_t* window) {
	// On Android, we don't destroy the window - the system manages it
	// Just clear our reference
	window->native_window = NULL;
}

void ska_platform_window_set_title(ska_window_t* window, const char* title) {
	// Android doesn't support changing window title at runtime
	if (window->title) {
		free(window->title);
	}
	window->title = strdup(title);
}

void ska_platform_window_set_position(ska_window_t* window, int32_t x, int32_t y) {
	// Android windows are always fullscreen, position cannot be changed
	(void)window; (void)x; (void)y;
}

void ska_platform_window_set_size(ska_window_t* window, int32_t w, int32_t h) {
	// Android window size is managed by the system
	(void)window; (void)w; (void)h;
}

void ska_platform_window_show(ska_window_t* window) {
	// Android windows are always visible when active
	window->is_visible = true;
}

void ska_platform_window_hide(ska_window_t* window) {
	// Cannot hide Android windows
	window->is_visible = false;
}

void ska_platform_window_maximize(ska_window_t* window) {
	// Android windows are always maximized
	(void)window;
}

void ska_platform_window_minimize(ska_window_t* window) {
	// Cannot minimize Android windows programmatically
	(void)window;
}

void ska_platform_window_restore(ska_window_t* window) {
	// No-op on Android
	(void)window;
}

void ska_platform_window_raise(ska_window_t* window) {
	// Android windows are always on top
	(void)window;
}

void ska_platform_window_get_drawable_size(ska_window_t* window, int32_t* opt_out_width, int32_t* opt_out_height) {
	// Drawable size equals window size on Android
	(void)window;
	(void)opt_out_width;
	(void)opt_out_height;
}

void ska_platform_warp_mouse(ska_window_t* window, int32_t x, int32_t y) {
	// Cannot warp cursor on touchscreen
	(void)window; (void)x; (void)y;
}

void ska_platform_set_cursor(ska_system_cursor_ cursor) {
	// No cursor on touchscreen
	(void)cursor;
}

void ska_platform_show_cursor(bool show) {
	// No cursor on touchscreen
	(void)show;
}

bool ska_platform_set_relative_mouse_mode(bool enabled) {
	// Not applicable on touchscreen
	(void)enabled;
	return false;
}

void ska_platform_pump_events(void) {
	if (!g_ska.android_app) {
		return;
	}

	// Poll all pending events
	int32_t events;
	struct android_poll_source* source;

	// Non-blocking poll
	while (ALooper_pollOnce(0, NULL, &events, (void**)&source) >= 0) {
		if (source != NULL) {
			source->process(g_ska.android_app, source);
		}

		// Check if we're exiting
		if (g_ska.android_app->destroyRequested) {
			ska_event_t event = {0};
			event.type = ska_event_quit;
			event.timestamp = (uint32_t)ska_time_get_elapsed_ms();
			ska_post_event(&event);
			return;
		}
	}
}

/////////////////////////////////////////
// Android specific subset of Vulkan header
/////////////////////////////////////////

typedef VkFlags VkAndroidSurfaceCreateFlagsKHR;
typedef struct VkAndroidSurfaceCreateInfoKHR {
	VkStructureType                   sType;
	const void*                       pNext;
	VkAndroidSurfaceCreateFlagsKHR    flags;
	ANativeWindow*                    window;
} VkAndroidSurfaceCreateInfoKHR;

typedef VkResult (VKAPI_PTR *PFN_vkCreateAndroidSurfaceKHR)(VkInstance instance, const VkAndroidSurfaceCreateInfoKHR* pCreateInfo, const /*VkAllocationCallbacks*/ void* pAllocator, VkSurfaceKHR* pSurface);

/////////////////////////////////////////

const char** ska_platform_vk_get_instance_extensions(uint32_t* out_count) {
	static const char* extensions[] = {
		"VK_KHR_surface",
		"VK_KHR_android_surface"
	};
	*out_count = 2;
	return extensions;
}

bool ska_platform_vk_create_surface(const ska_window_t* window, VkInstance instance, VkSurfaceKHR* out_surface) {
	if (!window->native_window) {
		ska_set_error("Native window not available");
		return false;
	}

	void* module = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
	if (!module) {
		ska_set_error("Failed to load Vulkan .so");
		return false;
	}

	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(module, "vkGetInstanceProcAddr");
	if (!vkGetInstanceProcAddr) {
		ska_set_error("Failed to load vkGetInstanceProcAddr");
		return false;
	}

	PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR = (PFN_vkCreateAndroidSurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateAndroidSurfaceKHR");
	if (!vkCreateAndroidSurfaceKHR) {
		ska_set_error("Failed to load vkCreateAndroidSurfaceKHR");
		return false;
	}

	VkAndroidSurfaceCreateInfoKHR create_info = {0};
	create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	create_info.window = window->native_window;

	VkResult result = vkCreateAndroidSurfaceKHR(instance, &create_info, NULL, out_surface);
	if (result != VK_SUCCESS) {
		ska_set_error("Failed to create Vulkan Android surface: %d", result);
		return false;
	}

	return true;
}

// ========== Text Input Platform Functions ==========

void ska_platform_show_virtual_keyboard(bool visible, ska_text_input_type_ type) {
	// Android - JNI implementation to show/hide soft keyboard
	if (!g_ska.android_app) {
		return;
	}

	JNIEnv* env;
	JavaVM* jvm = g_ska.android_app->activity->vm;
	(*jvm)->AttachCurrentThread(jvm, &env, NULL);

	if (visible) {
		// Show soft keyboard using InputMethodManager
		jclass activity_class = (*env)->FindClass(env, "android/app/NativeActivity");
		jmethodID get_system_service = (*env)->GetMethodID(env, activity_class,
			"getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

		jstring service_name = (*env)->NewStringUTF(env, "input_method");
		jobject imm = (*env)->CallObjectMethod(env, g_ska.android_app->activity->clazz,
			get_system_service, service_name);

		if (imm) {
			jclass imm_class = (*env)->FindClass(env, "android/view/inputmethod/InputMethodManager");
			jmethodID show_soft_input = (*env)->GetMethodID(env, imm_class,
				"showSoftInput", "(Landroid/view/View;I)Z");

			// Get the window's decor view
			jmethodID get_window = (*env)->GetMethodID(env, activity_class,
				"getWindow", "()Landroid/view/Window;");
			jobject window = (*env)->CallObjectMethod(env, g_ska.android_app->activity->clazz,
				get_window);

			if (window) {
				jclass window_class = (*env)->FindClass(env, "android/view/Window");
				jmethodID get_decor_view = (*env)->GetMethodID(env, window_class,
					"getDecorView", "()Landroid/view/View;");
				jobject decor_view = (*env)->CallObjectMethod(env, window, get_decor_view);

				if (decor_view) {
					(*env)->CallBooleanMethod(env, imm, show_soft_input, decor_view, 0);
					(*env)->DeleteLocalRef(env, decor_view);
				}
				(*env)->DeleteLocalRef(env, window);
			}
			(*env)->DeleteLocalRef(env, imm);
		}
		(*env)->DeleteLocalRef(env, service_name);
	} else {
		// Hide soft keyboard
		jclass activity_class = (*env)->FindClass(env, "android/app/NativeActivity");
		jmethodID get_system_service = (*env)->GetMethodID(env, activity_class,
			"getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

		jstring service_name = (*env)->NewStringUTF(env, "input_method");
		jobject imm = (*env)->CallObjectMethod(env, g_ska.android_app->activity->clazz,
			get_system_service, service_name);

		if (imm) {
			jclass imm_class = (*env)->FindClass(env, "android/view/inputmethod/InputMethodManager");
			jmethodID hide_soft_input = (*env)->GetMethodID(env, imm_class,
				"hideSoftInputFromWindow", "(Landroid/os/IBinder;I)Z");

			// Get window token
			jmethodID get_window = (*env)->GetMethodID(env, activity_class,
				"getWindow", "()Landroid/view/Window;");
			jobject window = (*env)->CallObjectMethod(env, g_ska.android_app->activity->clazz,
				get_window);

			if (window) {
				jclass window_class = (*env)->FindClass(env, "android/view/Window");
				jmethodID get_decor_view = (*env)->GetMethodID(env, window_class,
					"getDecorView", "()Landroid/view/View;");
				jobject decor_view = (*env)->CallObjectMethod(env, window, get_decor_view);

				if (decor_view) {
					jclass view_class = (*env)->FindClass(env, "android/view/View");
					jmethodID get_window_token = (*env)->GetMethodID(env, view_class,
						"getWindowToken", "()Landroid/os/IBinder;");
					jobject token = (*env)->CallObjectMethod(env, decor_view, get_window_token);

					if (token) {
						(*env)->CallBooleanMethod(env, imm, hide_soft_input, token, 0);
						(*env)->DeleteLocalRef(env, token);
					}
					(*env)->DeleteLocalRef(env, decor_view);
				}
				(*env)->DeleteLocalRef(env, window);
			}
			(*env)->DeleteLocalRef(env, imm);
		}
		(*env)->DeleteLocalRef(env, service_name);
	}

	(*jvm)->DetachCurrentThread(jvm);
	(void)type; // TODO: Use type to set input mode
}

// ========== Android Entry Point ==========

//
// Forward declaration of user's main function.
// Users write a standard main(int argc, char** argv) just like on desktop.
extern int32_t main(int argc, char** argv);

//
// Android native activity entry point.
// This is provided by the library - users do NOT define this.
//
// The library automatically:
// 1. Sets up the android_app pointer
// 2. Waits for the window to be ready
// 3. Calls the user's main() function in a separate thread
// 4. Manages the Android event loop

#include <pthread.h>

typedef struct {
	struct android_app* app;
	bool user_main_finished;
	int32_t user_main_result;
} android_main_state_t;

static android_main_state_t g_android_main_state = {0};

// Thread function that runs the user's main()
static void* ska_android_user_main_thread(void* arg) {
	(void)arg;

	// Call user's main function with empty args
	char* argv[] = {"sk_app", NULL};
	g_android_main_state.user_main_result = main(1, argv);
	g_android_main_state.user_main_finished = true;

	// Request app destruction when main() returns
	ANativeActivity_finish(g_android_main_state.app->activity);

	return NULL;
}

// ========== Clipboard Platform Functions ==========

size_t ska_platform_clipboard_get_text(char* opt_out_buffer, size_t buffer_size) {
	if (!g_ska.android_app) {
		return 0;
	}

	JNIEnv* env;
	JavaVM* jvm = g_ska.android_app->activity->vm;
	(*jvm)->AttachCurrentThread(jvm, &env, NULL);

	// Get ClipboardManager
	jclass activity_class = (*env)->FindClass(env, "android/app/NativeActivity");
	jmethodID get_system_service = (*env)->GetMethodID(env, activity_class,
		"getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

	jstring service_name = (*env)->NewStringUTF(env, "clipboard");
	jobject clipboard_manager = (*env)->CallObjectMethod(env, g_ska.android_app->activity->clazz,
		get_system_service, service_name);

	if (!clipboard_manager) {
		(*jvm)->DetachCurrentThread(jvm);
		return 0;
	}

	// Check if clipboard has text
	jclass clipboard_class = (*env)->FindClass(env, "android/content/ClipboardManager");
	jmethodID has_primary_clip = (*env)->GetMethodID(env, clipboard_class,
		"hasPrimaryClip", "()Z");
	jboolean has_clip = (*env)->CallBooleanMethod(env, clipboard_manager, has_primary_clip);

	if (!has_clip) {
		(*jvm)->DetachCurrentThread(jvm);
		return 0;
	}

	// Get primary clip
	jmethodID get_primary_clip = (*env)->GetMethodID(env, clipboard_class,
		"getPrimaryClip", "()Landroid/content/ClipData;");
	jobject clip_data = (*env)->CallObjectMethod(env, clipboard_manager, get_primary_clip);

	if (!clip_data) {
		(*jvm)->DetachCurrentThread(jvm);
		return 0;
	}

	// Get first item
	jclass clip_data_class = (*env)->FindClass(env, "android/content/ClipData");
	jmethodID get_item_at = (*env)->GetMethodID(env, clip_data_class,
		"getItemAt", "(I)Landroid/content/ClipData$Item;");
	jobject clip_item = (*env)->CallObjectMethod(env, clip_data, get_item_at, 0);

	if (!clip_item) {
		(*jvm)->DetachCurrentThread(jvm);
		return 0;
	}

	// Get text from item
	jclass clip_item_class = (*env)->FindClass(env, "android/content/ClipData$Item");
	jmethodID get_text = (*env)->GetMethodID(env, clip_item_class,
		"getText", "()Ljava/lang/CharSequence;");
	jobject char_sequence = (*env)->CallObjectMethod(env, clip_item, get_text);

	if (!char_sequence) {
		(*jvm)->DetachCurrentThread(jvm);
		return 0;
	}

	// Convert CharSequence to String
	jclass char_sequence_class = (*env)->FindClass(env, "java/lang/CharSequence");
	jmethodID to_string = (*env)->GetMethodID(env, char_sequence_class,
		"toString", "()Ljava/lang/String;");
	jstring text_string = (jstring)(*env)->CallObjectMethod(env, char_sequence, to_string);

	if (!text_string) {
		(*jvm)->DetachCurrentThread(jvm);
		return 0;
	}

	// Convert to UTF-8
	const char* utf8_text = (*env)->GetStringUTFChars(env, text_string, NULL);
	if (!utf8_text) {
		(*jvm)->DetachCurrentThread(jvm);
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

	(*env)->ReleaseStringUTFChars(env, text_string, utf8_text);
	(*jvm)->DetachCurrentThread(jvm);

	return text_size;
}

bool ska_platform_clipboard_set_text(const char* text) {
	if (!g_ska.android_app || !text) {
		ska_set_error("ska_platform_clipboard_set_text: invalid app or text");
		return false;
	}

	JNIEnv* env;
	JavaVM* jvm = g_ska.android_app->activity->vm;
	(*jvm)->AttachCurrentThread(jvm, &env, NULL);

	// Get ClipboardManager
	jclass activity_class = (*env)->FindClass(env, "android/app/NativeActivity");
	jmethodID get_system_service = (*env)->GetMethodID(env, activity_class,
		"getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

	jstring service_name = (*env)->NewStringUTF(env, "clipboard");
	jobject clipboard_manager = (*env)->CallObjectMethod(env, g_ska.android_app->activity->clazz,
		get_system_service, service_name);

	if (!clipboard_manager) {
		(*jvm)->DetachCurrentThread(jvm);
		ska_set_error("ska_platform_clipboard_set_text: failed to get clipboard manager");
		return false;
	}

	// Create ClipData
	jclass clip_data_class = (*env)->FindClass(env, "android/content/ClipData");
	jmethodID new_plain_text = (*env)->GetStaticMethodID(env, clip_data_class,
		"newPlainText", "(Ljava/lang/CharSequence;Ljava/lang/CharSequence;)Landroid/content/ClipData;");

	jstring label = (*env)->NewStringUTF(env, "text");
	jstring text_string = (*env)->NewStringUTF(env, text);

	jobject clip_data = (*env)->CallStaticObjectMethod(env, clip_data_class,
		new_plain_text, label, text_string);

	if (!clip_data) {
		(*jvm)->DetachCurrentThread(jvm);
		ska_set_error("ska_platform_clipboard_set_text: failed to create clip data");
		return false;
	}

	// Set primary clip
	jclass clipboard_class = (*env)->FindClass(env, "android/content/ClipboardManager");
	jmethodID set_primary_clip = (*env)->GetMethodID(env, clipboard_class,
		"setPrimaryClip", "(Landroid/content/ClipData;)V");

	(*env)->CallVoidMethod(env, clipboard_manager, set_primary_clip, clip_data);

	(*jvm)->DetachCurrentThread(jvm);
	return true;
}

void android_main(struct android_app* app) {
	// Set the android_app pointer in the global state
	g_ska.android_app = app;
	g_android_main_state.app = app;
	g_android_main_state.user_main_finished = false;
	g_android_main_state.user_main_result = 0;

	pthread_t user_thread;
	bool user_thread_started = false;

	// Main event loop
	while (1) {
		int32_t events;
		struct android_poll_source* source;

		// Poll for events
		int32_t timeout = user_thread_started ? 0 : -1;
		while (ALooper_pollOnce(timeout, NULL, &events, (void**)&source) >= 0) {
			// Process this event
			if (source != NULL) {
				source->process(app, source);
			}

			// Start user's main() when window is ready
			if (app->window != NULL && !user_thread_started) {
				pthread_create(&user_thread, NULL, ska_android_user_main_thread, NULL);
				user_thread_started = true;
			}

			// Check if we are exiting
			if (app->destroyRequested != 0) {
				if (user_thread_started && !g_android_main_state.user_main_finished) {
					// Wait for user thread to finish
					pthread_join(user_thread, NULL);
				}
				return;
			}
		}

		// If user's main finished, exit
		if (g_android_main_state.user_main_finished) {
			pthread_join(user_thread, NULL);
			return;
		}
	}
}

#endif // SKA_PLATFORM_ANDROID
