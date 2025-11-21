/*
 * sk_app - macOS Cocoa platform backend
 */

#include "ska_internal.h"

#ifdef SKA_PLATFORM_MACOS

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>  /* For key codes */

/* Scancode translation table (Carbon key codes to ska_scancode_) */
static ska_scancode_ ska_macos_scancode_table[256];

static void ska_init_scancode_table(void) {
	/* Initialize all to unknown */
	for (int i = 0; i < 256; i++) {
		ska_macos_scancode_table[i] = SKA_SCANCODE_UNKNOWN;
	}

	/* Letters */
	ska_macos_scancode_table[kVK_ANSI_A] = SKA_SCANCODE_A;
	ska_macos_scancode_table[kVK_ANSI_B] = SKA_SCANCODE_B;
	ska_macos_scancode_table[kVK_ANSI_C] = SKA_SCANCODE_C;
	ska_macos_scancode_table[kVK_ANSI_D] = SKA_SCANCODE_D;
	ska_macos_scancode_table[kVK_ANSI_E] = SKA_SCANCODE_E;
	ska_macos_scancode_table[kVK_ANSI_F] = SKA_SCANCODE_F;
	ska_macos_scancode_table[kVK_ANSI_G] = SKA_SCANCODE_G;
	ska_macos_scancode_table[kVK_ANSI_H] = SKA_SCANCODE_H;
	ska_macos_scancode_table[kVK_ANSI_I] = SKA_SCANCODE_I;
	ska_macos_scancode_table[kVK_ANSI_J] = SKA_SCANCODE_J;
	ska_macos_scancode_table[kVK_ANSI_K] = SKA_SCANCODE_K;
	ska_macos_scancode_table[kVK_ANSI_L] = SKA_SCANCODE_L;
	ska_macos_scancode_table[kVK_ANSI_M] = SKA_SCANCODE_M;
	ska_macos_scancode_table[kVK_ANSI_N] = SKA_SCANCODE_N;
	ska_macos_scancode_table[kVK_ANSI_O] = SKA_SCANCODE_O;
	ska_macos_scancode_table[kVK_ANSI_P] = SKA_SCANCODE_P;
	ska_macos_scancode_table[kVK_ANSI_Q] = SKA_SCANCODE_Q;
	ska_macos_scancode_table[kVK_ANSI_R] = SKA_SCANCODE_R;
	ska_macos_scancode_table[kVK_ANSI_S] = SKA_SCANCODE_S;
	ska_macos_scancode_table[kVK_ANSI_T] = SKA_SCANCODE_T;
	ska_macos_scancode_table[kVK_ANSI_U] = SKA_SCANCODE_U;
	ska_macos_scancode_table[kVK_ANSI_V] = SKA_SCANCODE_V;
	ska_macos_scancode_table[kVK_ANSI_W] = SKA_SCANCODE_W;
	ska_macos_scancode_table[kVK_ANSI_X] = SKA_SCANCODE_X;
	ska_macos_scancode_table[kVK_ANSI_Y] = SKA_SCANCODE_Y;
	ska_macos_scancode_table[kVK_ANSI_Z] = SKA_SCANCODE_Z;

	/* Numbers */
	ska_macos_scancode_table[kVK_ANSI_0] = SKA_SCANCODE_0;
	ska_macos_scancode_table[kVK_ANSI_1] = SKA_SCANCODE_1;
	ska_macos_scancode_table[kVK_ANSI_2] = SKA_SCANCODE_2;
	ska_macos_scancode_table[kVK_ANSI_3] = SKA_SCANCODE_3;
	ska_macos_scancode_table[kVK_ANSI_4] = SKA_SCANCODE_4;
	ska_macos_scancode_table[kVK_ANSI_5] = SKA_SCANCODE_5;
	ska_macos_scancode_table[kVK_ANSI_6] = SKA_SCANCODE_6;
	ska_macos_scancode_table[kVK_ANSI_7] = SKA_SCANCODE_7;
	ska_macos_scancode_table[kVK_ANSI_8] = SKA_SCANCODE_8;
	ska_macos_scancode_table[kVK_ANSI_9] = SKA_SCANCODE_9;

	/* Function keys */
	ska_macos_scancode_table[kVK_Return] = SKA_SCANCODE_RETURN;
	ska_macos_scancode_table[kVK_Escape] = SKA_SCANCODE_ESCAPE;
	ska_macos_scancode_table[kVK_Delete] = SKA_SCANCODE_BACKSPACE;
	ska_macos_scancode_table[kVK_Tab] = SKA_SCANCODE_TAB;
	ska_macos_scancode_table[kVK_Space] = SKA_SCANCODE_SPACE;

	/* Symbols */
	ska_macos_scancode_table[kVK_ANSI_Minus] = SKA_SCANCODE_MINUS;
	ska_macos_scancode_table[kVK_ANSI_Equal] = SKA_SCANCODE_EQUALS;
	ska_macos_scancode_table[kVK_ANSI_LeftBracket] = SKA_SCANCODE_LEFTBRACKET;
	ska_macos_scancode_table[kVK_ANSI_RightBracket] = SKA_SCANCODE_RIGHTBRACKET;
	ska_macos_scancode_table[kVK_ANSI_Backslash] = SKA_SCANCODE_BACKSLASH;
	ska_macos_scancode_table[kVK_ANSI_Semicolon] = SKA_SCANCODE_SEMICOLON;
	ska_macos_scancode_table[kVK_ANSI_Quote] = SKA_SCANCODE_APOSTROPHE;
	ska_macos_scancode_table[kVK_ANSI_Grave] = SKA_SCANCODE_GRAVE;
	ska_macos_scancode_table[kVK_ANSI_Comma] = SKA_SCANCODE_COMMA;
	ska_macos_scancode_table[kVK_ANSI_Period] = SKA_SCANCODE_PERIOD;
	ska_macos_scancode_table[kVK_ANSI_Slash] = SKA_SCANCODE_SLASH;

	ska_macos_scancode_table[kVK_CapsLock] = SKA_SCANCODE_CAPSLOCK;

	/* F keys */
	ska_macos_scancode_table[kVK_F1] = SKA_SCANCODE_F1;
	ska_macos_scancode_table[kVK_F2] = SKA_SCANCODE_F2;
	ska_macos_scancode_table[kVK_F3] = SKA_SCANCODE_F3;
	ska_macos_scancode_table[kVK_F4] = SKA_SCANCODE_F4;
	ska_macos_scancode_table[kVK_F5] = SKA_SCANCODE_F5;
	ska_macos_scancode_table[kVK_F6] = SKA_SCANCODE_F6;
	ska_macos_scancode_table[kVK_F7] = SKA_SCANCODE_F7;
	ska_macos_scancode_table[kVK_F8] = SKA_SCANCODE_F8;
	ska_macos_scancode_table[kVK_F9] = SKA_SCANCODE_F9;
	ska_macos_scancode_table[kVK_F10] = SKA_SCANCODE_F10;
	ska_macos_scancode_table[kVK_F11] = SKA_SCANCODE_F11;
	ska_macos_scancode_table[kVK_F12] = SKA_SCANCODE_F12;

	/* Navigation */
	ska_macos_scancode_table[kVK_Home] = SKA_SCANCODE_HOME;
	ska_macos_scancode_table[kVK_PageUp] = SKA_SCANCODE_PAGEUP;
	ska_macos_scancode_table[kVK_ForwardDelete] = SKA_SCANCODE_DELETE;
	ska_macos_scancode_table[kVK_End] = SKA_SCANCODE_END;
	ska_macos_scancode_table[kVK_PageDown] = SKA_SCANCODE_PAGEDOWN;
	ska_macos_scancode_table[kVK_RightArrow] = SKA_SCANCODE_RIGHT;
	ska_macos_scancode_table[kVK_LeftArrow] = SKA_SCANCODE_LEFT;
	ska_macos_scancode_table[kVK_DownArrow] = SKA_SCANCODE_DOWN;
	ska_macos_scancode_table[kVK_UpArrow] = SKA_SCANCODE_UP;

	/* Modifiers */
	ska_macos_scancode_table[kVK_Control] = SKA_SCANCODE_LCTRL;
	ska_macos_scancode_table[kVK_Shift] = SKA_SCANCODE_LSHIFT;
	ska_macos_scancode_table[kVK_Option] = SKA_SCANCODE_LALT;
	ska_macos_scancode_table[kVK_Command] = SKA_SCANCODE_LGUI;
	ska_macos_scancode_table[kVK_RightControl] = SKA_SCANCODE_RCTRL;
	ska_macos_scancode_table[kVK_RightShift] = SKA_SCANCODE_RSHIFT;
	ska_macos_scancode_table[kVK_RightOption] = SKA_SCANCODE_RALT;
	/* Note: macOS doesn't distinguish right Command in Carbon */
}

static ska_window_t* ska_find_window_by_nswindow(NSWindow* nswindow) {
	for (uint32_t i = 0; i < SKA_MAX_WINDOWS; i++) {
		if (g_ska.windows[i] && g_ska.windows[i]->ns_window == nswindow) {
			return g_ska.windows[i];
		}
	}
	return NULL;
}

static uint16_t ska_macos_get_modifiers(NSEventModifierFlags flags) {
	uint16_t mods = 0;
	if (flags & NSEventModifierFlagShift) mods |= SKA_KEYMOD_SHIFT;
	if (flags & NSEventModifierFlagControl) mods |= SKA_KEYMOD_CTRL;
	if (flags & NSEventModifierFlagOption) mods |= SKA_KEYMOD_ALT;
	if (flags & NSEventModifierFlagCommand) mods |= SKA_KEYMOD_GUI;
	return mods;
}

/* Window delegate for event handling */
@interface SKAWindowDelegate : NSObject <NSWindowDelegate>
@property (assign) ska_window_t* window;
@end

@implementation SKAWindowDelegate

- (void)windowDidResize:(NSNotification*)notification {
	if (!self.window) return;

	NSWindow* nswindow = (NSWindow*)self.window->ns_window;
	NSRect rect = [nswindow contentRectForFrameRect:[nswindow frame]];

	int32_t width = (int32_t)rect.size.width;
	int32_t height = (int32_t)rect.size.height;

	if (width != self.window->width || height != self.window->height) {
		ska_event_t event = {0};
		event.type = SKA_EVENT_WINDOW_RESIZED;
		event.timestamp = (uint32_t)ska_get_ticks();
		event.window.window_id = self.window->id;
		event.window.data1 = width;
		event.window.data2 = height;
		self.window->width = width;
		self.window->height = height;

		/* Update drawable size for Retina displays */
		NSView* view = (NSView*)self.window->ns_view;
		NSRect backing_rect = [view convertRectToBacking:rect];
		self.window->drawable_width = (int32_t)backing_rect.size.width;
		self.window->drawable_height = (int32_t)backing_rect.size.height;

		ska_post_event(&event);
	}
}

- (void)windowDidMove:(NSNotification*)notification {
	if (!self.window) return;

	NSWindow* nswindow = (NSWindow*)self.window->ns_window;
	NSRect frame = [nswindow frame];

	/* macOS origin is bottom-left, convert to top-left */
	NSScreen* screen = [NSScreen mainScreen];
	int32_t x = (int32_t)frame.origin.x;
	int32_t y = (int32_t)(screen.frame.size.height - frame.origin.y - frame.size.height);

	if (x != self.window->x || y != self.window->y) {
		ska_event_t event = {0};
		event.type = SKA_EVENT_WINDOW_MOVED;
		event.timestamp = (uint32_t)ska_get_ticks();
		event.window.window_id = self.window->id;
		event.window.data1 = x;
		event.window.data2 = y;
		self.window->x = x;
		self.window->y = y;
		ska_post_event(&event);
	}
}

- (void)windowDidBecomeKey:(NSNotification*)notification {
	if (!self.window) return;

	ska_event_t event = {0};
	event.type = SKA_EVENT_WINDOW_FOCUS_GAINED;
	event.timestamp = (uint32_t)ska_get_ticks();
	event.window.window_id = self.window->id;
	self.window->has_focus = true;
	ska_post_event(&event);
}

- (void)windowDidResignKey:(NSNotification*)notification {
	if (!self.window) return;

	ska_event_t event = {0};
	event.type = SKA_EVENT_WINDOW_FOCUS_LOST;
	event.timestamp = (uint32_t)ska_get_ticks();
	event.window.window_id = self.window->id;
	self.window->has_focus = false;
	ska_post_event(&event);
}

- (BOOL)windowShouldClose:(NSWindow*)sender {
	if (!self.window) return YES;

	ska_event_t event = {0};
	event.type = SKA_EVENT_WINDOW_CLOSE;
	event.timestamp = (uint32_t)ska_get_ticks();
	event.window.window_id = self.window->id;
	self.window->should_close = true;
	ska_post_event(&event);

	return NO;  /* We'll close it manually */
}

- (void)windowDidMiniaturize:(NSNotification*)notification {
	if (!self.window) return;

	ska_event_t event = {0};
	event.type = SKA_EVENT_WINDOW_MINIMIZED;
	event.timestamp = (uint32_t)ska_get_ticks();
	event.window.window_id = self.window->id;
	ska_post_event(&event);
}

- (void)windowDidDeminiaturize:(NSNotification*)notification {
	if (!self.window) return;

	ska_event_t event = {0};
	event.type = SKA_EVENT_WINDOW_RESTORED;
	event.timestamp = (uint32_t)ska_get_ticks();
	event.window.window_id = self.window->id;
	ska_post_event(&event);
}

@end

/* Application delegate for app lifecycle */
@interface SKAAppDelegate : NSObject <NSApplicationDelegate>
@end

@implementation SKAAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
	g_ska.app_activated = true;
	[NSApp stop:nil];  /* Exit the initial run loop */
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
	return NO;  /* Don't auto-quit */
}

@end

bool ska_platform_init(void) {
	@autoreleasepool {
		/* Get or create NSApplication */
		g_ska.ns_app = [NSApplication sharedApplication];
		[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

		/* Create app delegate */
		g_ska.ns_delegate = [[SKAAppDelegate alloc] init];
		[NSApp setDelegate:(SKAAppDelegate*)g_ska.ns_delegate];

		/* Activate the app */
		[NSApp finishLaunching];
		[NSApp activateIgnoringOtherApps:YES];

		/* Initialize scancode table */
		ska_init_scancode_table();

		return true;
	}
}

void ska_platform_shutdown(void) {
	@autoreleasepool {
		if (g_ska.ns_delegate) {
			[NSApp setDelegate:nil];
			g_ska.ns_delegate = nil;
		}
	}
}

bool ska_platform_window_create(
	ska_window_t* window,
	const char* title,
	int32_t x, int32_t y,
	int32_t w, int32_t h,
	uint32_t flags
) {
	@autoreleasepool {
		/* Determine window style */
		NSWindowStyleMask style = NSWindowStyleMaskTitled;

		if (flags & SKA_WINDOW_BORDERLESS) {
			style = NSWindowStyleMaskBorderless;
		} else {
			style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
			if (flags & SKA_WINDOW_RESIZABLE) {
				style |= NSWindowStyleMaskResizable;
			}
		}

		/* Convert coordinates (macOS uses bottom-left origin) */
		NSScreen* screen = [NSScreen mainScreen];
		NSRect content_rect;

		if (x == -1 || y == -1) {
			/* Center window */
			content_rect = NSMakeRect(0, 0, w, h);
		} else {
			/* Convert from top-left to bottom-left origin */
			CGFloat cocoa_y = screen.frame.size.height - y - h;
			content_rect = NSMakeRect(x, cocoa_y, w, h);
		}

		/* Create window */
		NSWindow* nswindow = [[NSWindow alloc]
			initWithContentRect:content_rect
			styleMask:style
			backing:NSBackingStoreBuffered
			defer:NO];

		if (!nswindow) {
			ska_set_error("Failed to create NSWindow");
			return false;
		}

		window->ns_window = nswindow;

		/* Set title */
		NSString* ns_title = [NSString stringWithUTF8String:title];
		[nswindow setTitle:ns_title];
		window->title = strdup(title);

		/* Create and set delegate */
		SKAWindowDelegate* delegate = [[SKAWindowDelegate alloc] init];
		delegate.window = window;
		[nswindow setDelegate:delegate];
		window->ns_view = [nswindow contentView];

		/* Set window properties */
		[nswindow setAcceptsMouseMovedEvents:YES];
		[nswindow setReleasedWhenClosed:NO];

		if (x == -1 || y == -1) {
			[nswindow center];
		}

		/* Get actual position and size */
		NSRect frame = [nswindow frame];
		NSRect client = [nswindow contentRectForFrameRect:frame];

		window->width = (int32_t)client.size.width;
		window->height = (int32_t)client.size.height;
		window->x = (int32_t)frame.origin.x;
		window->y = (int32_t)(screen.frame.size.height - frame.origin.y - frame.size.height);

		/* Get drawable size (for Retina displays) */
		NSView* view = (NSView*)window->ns_view;
		NSRect backing_rect = [view convertRectToBacking:client];
		window->drawable_width = (int32_t)backing_rect.size.width;
		window->drawable_height = (int32_t)backing_rect.size.height;

		/* Apply initial state */
		if (flags & SKA_WINDOW_MAXIMIZED) {
			[nswindow zoom:nil];
		}

		if (!(flags & SKA_WINDOW_HIDDEN)) {
			[nswindow makeKeyAndOrderFront:nil];
			window->is_visible = true;
		}

		return true;
	}
}

void ska_platform_window_destroy(ska_window_t* window) {
	@autoreleasepool {
		if (window->ns_window) {
			NSWindow* nswindow = (NSWindow*)window->ns_window;
			[nswindow setDelegate:nil];
			[nswindow close];
			window->ns_window = nil;
			window->ns_view = nil;
		}
	}
}

void ska_platform_window_set_title(ska_window_t* window, const char* title) {
	@autoreleasepool {
		NSWindow* nswindow = (NSWindow*)window->ns_window;
		NSString* ns_title = [NSString stringWithUTF8String:title];
		[nswindow setTitle:ns_title];

		if (window->title) {
			free(window->title);
		}
		window->title = strdup(title);
	}
}

void ska_platform_window_set_position(ska_window_t* window, int32_t x, int32_t y) {
	@autoreleasepool {
		NSWindow* nswindow = (NSWindow*)window->ns_window;
		NSScreen* screen = [NSScreen mainScreen];
		NSRect frame = [nswindow frame];

		/* Convert from top-left to bottom-left origin */
		CGFloat cocoa_y = screen.frame.size.height - y - frame.size.height;
		[nswindow setFrameOrigin:NSMakePoint(x, cocoa_y)];
	}
}

void ska_platform_window_set_size(ska_window_t* window, int32_t w, int32_t h) {
	@autoreleasepool {
		NSWindow* nswindow = (NSWindow*)window->ns_window;
		NSRect frame = [nswindow frame];
		NSRect content_rect = NSMakeRect(frame.origin.x, frame.origin.y, w, h);
		NSRect new_frame = [nswindow frameRectForContentRect:content_rect];
		[nswindow setFrame:new_frame display:YES];
	}
}

void ska_platform_window_show(ska_window_t* window) {
	@autoreleasepool {
		NSWindow* nswindow = (NSWindow*)window->ns_window;
		[nswindow makeKeyAndOrderFront:nil];
		window->is_visible = true;
	}
}

void ska_platform_window_hide(ska_window_t* window) {
	@autoreleasepool {
		NSWindow* nswindow = (NSWindow*)window->ns_window;
		[nswindow orderOut:nil];
		window->is_visible = false;
	}
}

void ska_platform_window_maximize(ska_window_t* window) {
	@autoreleasepool {
		NSWindow* nswindow = (NSWindow*)window->ns_window;
		if (![nswindow isZoomed]) {
			[nswindow zoom:nil];
		}
	}
}

void ska_platform_window_minimize(ska_window_t* window) {
	@autoreleasepool {
		NSWindow* nswindow = (NSWindow*)window->ns_window;
		[nswindow miniaturize:nil];
	}
}

void ska_platform_window_restore(ska_window_t* window) {
	@autoreleasepool {
		NSWindow* nswindow = (NSWindow*)window->ns_window;
		if ([nswindow isMiniaturized]) {
			[nswindow deminiaturize:nil];
		} else if ([nswindow isZoomed]) {
			[nswindow zoom:nil];
		}
	}
}

void ska_platform_window_raise(ska_window_t* window) {
	@autoreleasepool {
		NSWindow* nswindow = (NSWindow*)window->ns_window;
		[nswindow makeKeyAndOrderFront:nil];
	}
}

void ska_platform_window_get_drawable_size(ska_window_t* window, int32_t* opt_out_width, int32_t* opt_out_height) {
	@autoreleasepool {
		/* Already computed during resize, just return cached values */
		/* Retina displays have drawable size != window size */
		(void)opt_out_width;
		(void)opt_out_height;
	}
}

void ska_platform_warp_mouse(ska_window_t* window, int32_t x, int32_t y) {
	@autoreleasepool {
		NSWindow* nswindow = (NSWindow*)window->ns_window;
		NSView* view = (NSView*)window->ns_view;

		NSPoint point = NSMakePoint(x, window->height - y);  /* Flip Y */
		point = [view convertPoint:point toView:nil];
		point = [nswindow convertPointToScreen:point];

		/* Convert to global screen coordinates */
		CGPoint cgpoint = CGPointMake(point.x, [[NSScreen mainScreen] frame].size.height - point.y);
		CGWarpMouseCursorPosition(cgpoint);
	}
}

void ska_platform_set_cursor(ska_system_cursor_ cursor) {
	NSCursor* ns_cursor = nil;

	switch (cursor) {
		case ska_system_cursor_arrow:      ns_cursor = [NSCursor arrowCursor]; break;
		case ska_system_cursor_ibeam:      ns_cursor = [NSCursor IBeamCursor]; break;
		case ska_system_cursor_crosshair:  ns_cursor = [NSCursor crosshairCursor]; break;
		case ska_system_cursor_hand:       ns_cursor = [NSCursor pointingHandCursor]; break;
		case ska_system_cursor_sizewe:     ns_cursor = [NSCursor resizeLeftRightCursor]; break;
		case ska_system_cursor_sizens:     ns_cursor = [NSCursor resizeUpDownCursor]; break;
		case ska_system_cursor_no:         ns_cursor = [NSCursor operationNotAllowedCursor]; break;
		case ska_system_cursor_wait:
		case ska_system_cursor_waitarrow:
		case ska_system_cursor_sizenwse:
		case ska_system_cursor_sizenesw:
		case ska_system_cursor_sizeall:
			ns_cursor = [NSCursor arrowCursor];
			break;
		default:
			ns_cursor = [NSCursor arrowCursor];
			break;
	}

	[ns_cursor set];
}

void ska_platform_show_cursor(bool show) {
	if (show) {
		[NSCursor unhide];
		CGAssociateMouseAndMouseCursorPosition(true);
	} else {
		[NSCursor hide];
	}
}

bool ska_platform_set_relative_mouse_mode(bool enabled) {
	CGAssociateMouseAndMouseCursorPosition(enabled ? false : true);
	if (enabled) {
		[NSCursor hide];
	} else {
		[NSCursor unhide];
	}
	return true;
}

void ska_platform_pump_events(void) {
	@autoreleasepool {
		while (true) {
			NSEvent* nsevent = [NSApp nextEventMatchingMask:NSEventMaskAny
									untilDate:[NSDate distantPast]
									inMode:NSDefaultRunLoopMode
									dequeue:YES];

			if (!nsevent) {
				break;
			}

			ska_window_t* window = NULL;
			if (nsevent.window) {
				window = ska_find_window_by_nswindow(nsevent.window);
			}

			ska_event_t event = {0};
			event.timestamp = (uint32_t)ska_get_ticks();

			switch (nsevent.type) {
				case NSEventTypeKeyDown:
				case NSEventTypeKeyUp: {
					if (window) {
						bool pressed = (nsevent.type == NSEventTypeKeyDown);
						unsigned short keycode = nsevent.keyCode;

						event.type = pressed ? SKA_EVENT_KEY_DOWN : SKA_EVENT_KEY_UP;
						event.keyboard.window_id = window->id;
						event.keyboard.pressed = pressed;
						event.keyboard.repeat = nsevent.isARepeat;
						event.keyboard.scancode = ska_macos_scancode_table[keycode];
						event.keyboard.modifiers = ska_macos_get_modifiers(nsevent.modifierFlags);

						/* Update input state */
						if (event.keyboard.scancode != SKA_SCANCODE_UNKNOWN) {
							g_ska.input_state.keyboard[event.keyboard.scancode] = pressed ? 1 : 0;
						}
						g_ska.input_state.key_modifiers = event.keyboard.modifiers;

						ska_post_event(&event);

						/* Handle text input */
						if (pressed && nsevent.characters.length > 0) {
							const char* utf8 = [nsevent.characters UTF8String];
							if (utf8 && strlen(utf8) > 0) {
								event.type = SKA_EVENT_TEXT_INPUT;
								event.text.window_id = window->id;
								strncpy(event.text.text, utf8, sizeof(event.text.text) - 1);
								ska_post_event(&event);
							}
						}
					}
					break;
				}

				case NSEventTypeMouseMoved:
				case NSEventTypeLeftMouseDragged:
				case NSEventTypeRightMouseDragged:
				case NSEventTypeOtherMouseDragged: {
					if (window) {
						NSPoint location = nsevent.locationInWindow;
						int32_t x = (int32_t)location.x;
						int32_t y = (int32_t)(window->height - location.y);  /* Flip Y */

						event.type = SKA_EVENT_MOUSE_MOTION;
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
					break;
				}

				case NSEventTypeLeftMouseDown:
				case NSEventTypeRightMouseDown:
				case NSEventTypeOtherMouseDown:
				case NSEventTypeLeftMouseUp:
				case NSEventTypeRightMouseUp:
				case NSEventTypeOtherMouseUp: {
					if (window) {
						bool pressed = (nsevent.type == NSEventTypeLeftMouseDown ||
									   nsevent.type == NSEventTypeRightMouseDown ||
									   nsevent.type == NSEventTypeOtherMouseDown);

						ska_mouse_button_ button;
						if (nsevent.type == NSEventTypeLeftMouseDown || nsevent.type == NSEventTypeLeftMouseUp) {
							button = SKA_MOUSE_BUTTON_LEFT;
						} else if (nsevent.type == NSEventTypeRightMouseDown || nsevent.type == NSEventTypeRightMouseUp) {
							button = SKA_MOUSE_BUTTON_RIGHT;
						} else {
							// OtherMouse can be middle, X1, or X2
							NSInteger buttonNumber = nsevent.buttonNumber;
							if (buttonNumber == 2) {
								button = SKA_MOUSE_BUTTON_MIDDLE;
							} else if (buttonNumber == 3) {
								button = ska_mouse_button_x1;
							} else if (buttonNumber == 4) {
								button = ska_mouse_button_x2;
							} else {
								button = SKA_MOUSE_BUTTON_MIDDLE; // Fallback for unknown buttons
							}
						}

						NSPoint location = nsevent.locationInWindow;
						int32_t x = (int32_t)location.x;
						int32_t y = (int32_t)(window->height - location.y);

						event.type = pressed ? SKA_EVENT_MOUSE_BUTTON_DOWN : SKA_EVENT_MOUSE_BUTTON_UP;
						event.mouse_button.window_id = window->id;
						event.mouse_button.button = button;
						event.mouse_button.pressed = pressed;
						event.mouse_button.clicks = (uint8_t)nsevent.clickCount;
						event.mouse_button.x = x;
						event.mouse_button.y = y;

						/* Update button state */
						uint32_t button_mask = (1 << (button - 1));
						if (pressed) {
							g_ska.input_state.mouse_buttons |= button_mask;
						} else {
							g_ska.input_state.mouse_buttons &= ~button_mask;
						}

						ska_post_event(&event);
					}
					break;
				}

				case NSEventTypeScrollWheel: {
					if (window) {
						CGFloat deltaX = nsevent.scrollingDeltaX;
						CGFloat deltaY = nsevent.scrollingDeltaY;

						event.type = SKA_EVENT_MOUSE_WHEEL;
						event.mouse_wheel.window_id = window->id;
						event.mouse_wheel.x = (int32_t)deltaX;
						event.mouse_wheel.y = (int32_t)deltaY;
						event.mouse_wheel.precise_x = (float)deltaX;
						event.mouse_wheel.precise_y = (float)deltaY;

						ska_post_event(&event);
					}
					break;
				}

				default:
					break;
			}

			[NSApp sendEvent:nsevent];
		}
	}
}

/////////////////////////////////////////
// macOS specific subset of Vulkan header
/////////////////////////////////////////

#import <QuartzCore/CAMetalLayer.h>

typedef VkFlags VkMetalSurfaceCreateFlagsEXT;
typedef struct VkMetalSurfaceCreateInfoEXT {
	VkStructureType                 sType;
	const void*                     pNext;
	VkMetalSurfaceCreateFlagsEXT    flags;
	const CAMetalLayer*             pLayer;
} VkMetalSurfaceCreateInfoEXT;

typedef VkResult (VKAPI_PTR *PFN_vkCreateMetalSurfaceEXT)(VkInstance instance, const VkMetalSurfaceCreateInfoEXT* pCreateInfo, const /*VkAllocationCallbacks*/ void* pAllocator, VkSurfaceKHR* pSurface);

/////////////////////////////////////////

const char** ska_platform_vk_get_instance_extensions(uint32_t* out_count) {
	static const char* extensions[] = {
		"VK_KHR_surface",
		"VK_EXT_metal_surface"
	};
	*out_count = 2;
	return extensions;
}

bool ska_platform_vk_create_surface(const ska_window_t* window, VkInstance instance, VkSurfaceKHR* out_surface) {
	@autoreleasepool {
		void* module = dlopen("@executable_path/../Frameworks/libMoltenVK.dylib", RTLD_NOW | RTLD_LOCAL);
		if (!module)
			module = dlopen("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
		if (!module)
			module = dlopen("libvulkan.1.dylib", RTLD_NOW | RTLD_LOCAL);
		if (!module && getenv("DYLD_FALLBACK_LIBRARY_PATH") == NULL)
			module = dlopen("/usr/local/lib/libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
		if (!module)
			module = dlopen("libMoltenVK.dylib", RTLD_NOW | RTLD_LOCAL);
		if (!module)
			module = dlopen("vulkan.framework/vulkan", RTLD_NOW | RTLD_LOCAL);
		if (!module)
			module = dlopen("MoltenVK.framework/MoltenVK", RTLD_NOW | RTLD_LOCAL);
		if (!module) {
			ska_set_error("Failed to load Vulkan dylib");
			return false;
		}

		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(module, "vkGetInstanceProcAddr");
		if (!vkGetInstanceProcAddr) {
			ska_set_error("Failed to load vkGetInstanceProcAddr");
			return false;
		}

		PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT)vkGetInstanceProcAddr(instance, "vkCreateMetalSurfaceEXT");
		if (!vkCreateMetalSurfaceEXT) {
			ska_set_error("Failed to load vkCreateMetalSurfaceEXT");
			return false;
		}

		NSView* nsview = (__bridge NSView*)window->ns_view;
		if (!nsview) {
			ska_set_error("Window view not available");
			return false;
		}

		/* Create a CAMetalLayer and set it as the view's layer */
		CAMetalLayer* metal_layer = [CAMetalLayer layer];
		[nsview setWantsLayer:YES];
		[nsview setLayer:metal_layer];

		VkMetalSurfaceCreateInfoEXT create_info = {0};
		create_info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
		create_info.pLayer = metal_layer;

		VkResult result = vkCreateMetalSurfaceEXT(instance, &create_info, NULL, out_surface);
		if (result != VK_SUCCESS) {
			ska_set_error("Failed to create Vulkan Metal surface: %d", result);
			return false;
		}

		return true;
	}
}

/* ========== Text Input Platform Functions ========== */

void ska_platform_show_virtual_keyboard(bool visible, ska_text_input_type_ type) {
	/* macOS - desktop platform, no virtual keyboard */
	(void)visible;
	(void)type;
}

/* ========== Clipboard Platform Functions ========== */

size_t ska_platform_clipboard_get_text(char* opt_out_buffer, size_t buffer_size) {
	@autoreleasepool {
		NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
		NSString* text = [pasteboard stringForType:NSPasteboardTypeString];

		if (!text) {
			return 0;
		}

		const char* utf8_text = [text UTF8String];
		if (!utf8_text) {
			return 0;
		}

		/* Calculate size including null terminator */
		size_t text_size = strlen(utf8_text) + 1;

		/* If buffer is provided, copy the text */
		if (opt_out_buffer && buffer_size > 0) {
			size_t copy_size = (text_size < buffer_size) ? text_size : buffer_size;
			memcpy(opt_out_buffer, utf8_text, copy_size - 1);
			opt_out_buffer[copy_size - 1] = '\0';
		}

		return text_size;
	}
}

bool ska_platform_clipboard_set_text(const char* text) {
	if (!text) {
		ska_set_error("ska_platform_clipboard_set_text: text cannot be NULL");
		return false;
	}

	@autoreleasepool {
		NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
		NSString* nstext = [NSString stringWithUTF8String:text];

		if (!nstext) {
			ska_set_error("ska_platform_clipboard_set_text: UTF-8 conversion failed");
			return false;
		}

		[pasteboard clearContents];
		BOOL success = [pasteboard setString:nstext forType:NSPasteboardTypeString];

		if (!success) {
			ska_set_error("ska_platform_clipboard_set_text: setString failed");
			return false;
		}

		return true;
	}
}

#endif /* SKA_PLATFORM_MACOS */
