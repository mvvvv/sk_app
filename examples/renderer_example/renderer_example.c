//
// sk_app + sk_renderer - Vulkan Rendering Example
//
// Demonstrates integration with sk_renderer for Vulkan-based rendering.

#include <sk_app.h>
#include <sk_renderer.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int32_t main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	printf("sk_app + sk_renderer Example\n");
	printf("============================\n\n");

	// Initialize sk_app
	if (!ska_init()) {
		fprintf(stderr, "Failed to initialize sk_app: %s\n", ska_error_get());
		return 1;
	}

	printf("[INIT] sk_app initialized\n");

	// Create window
	ska_window_t* window = ska_window_create(
		"sk_app + sk_renderer",
		SKA_WINDOWPOS_CENTERED,
		SKA_WINDOWPOS_CENTERED,
		1280, 720,
		ska_window_resizable
	);

	if (!window) {
		fprintf(stderr, "Failed to create window: %s\n", ska_error_get());
		ska_shutdown();
		return 1;
	}

	printf("[WINDOW] Window created\n");

	// Get required Vulkan instance extensions
	uint32_t     ext_count  = 0;
	const char** extensions = ska_vk_get_instance_extensions(&ext_count);
	if (!extensions) {
		fprintf(stderr, "Failed to get Vulkan extensions\n");
		ska_window_destroy(window);
		ska_shutdown();
		return 1;
	}
	
	// Initialize sk_renderer
	skr_settings_t settings = {
		.app_name                 = "sk_app_renderer_example",
		.app_version              = 1,
		.enable_validation        = true,
		.required_extensions      = extensions,
		.required_extension_count = ext_count,
	};

	if (!skr_init(settings)) {
		skr_log(skr_log_critical, "Failed to initialize sk_renderer!");
		ska_window_destroy(window);
		ska_shutdown();
		return 1;
	}

	printf("[RENDERER] sk_renderer initialized\n");

	// Create Vulkan surface
	VkSurfaceKHR vk_surface = VK_NULL_HANDLE;
	VkAllocationCallbacks* allocator = NULL;
	if (!ska_vk_create_surface(window, skr_get_vk_instance(), &vk_surface)) {
		fprintf(stderr, "Failed to create Vulkan surface\n");
		skr_shutdown();
		ska_window_destroy(window);
		ska_shutdown();
		return 1;
	}

	printf("[VULKAN] Surface created\n");

	// Create sk_renderer surface
	skr_surface_t surface = {0};
	if (skr_surface_create(vk_surface, &surface) != skr_err_success || surface.surface == VK_NULL_HANDLE) {
		skr_log(skr_log_critical, "Failed to create sk_renderer surface!");
		vkDestroySurfaceKHR(skr_get_vk_instance(), vk_surface, allocator);
		skr_shutdown();
		ska_window_destroy(window);
		ska_shutdown();
		return 1;
	}

	printf("[RENDERER] Surface created\n");
	printf("\n[CONTROLS] ESC to exit\n\n");

	// Main rendering loop
	uint32_t frame   = 0;
	bool     running = true;
	while (running) {
		ska_event_t event;

		// Process events
		while (ska_event_poll(&event)) {
			switch (event.type) {
				case ska_event_quit:
				case ska_event_window_close:
					printf("[EVENT] Quit requested\n");
					running = false;
					break;

				case ska_event_window_resized:
					printf("[EVENT] Window resized to %dx%d\n", event.window.data1, event.window.data2);
					skr_surface_resize(&surface);
					break;

				case ska_event_key_down:
					if (event.keyboard.scancode == ska_scancode_escape) {
						running = false;
					}
					break;

				default:
					break;
			}
		}

		skr_renderer_frame_begin();

		// Get next swapchain image
		skr_tex_t*   render_target  = NULL;
		skr_acquire_ acquire_result = skr_surface_next_tex(&surface, &render_target);

		if (acquire_result == skr_acquire_success && render_target) {
			// Begin rendering
			skr_vec4_t clear_color = {0.1f, 0.1f, 0.2f, 1.0f};
			skr_renderer_begin_pass(
				render_target,    // color target
				NULL,            // depth buffer
				NULL,            // resolve target
				skr_clear_all,   // clear flags
				clear_color,     // clear color
				1.0f,            // clear depth
				0                // clear stencil
			);

			// Set viewport
			skr_vec2i_t size = skr_surface_get_size(&surface);
			skr_renderer_set_viewport((skr_rect_t ){0, 0, (float)size.x, (float)size.y});
			skr_renderer_set_scissor ((skr_recti_t){0, 0, size.x, size.y});

			// End pass (we're just clearing for now)
			skr_renderer_end_pass();

			// End frame with surface synchronization
			skr_surface_t* surfaces[] = {&surface};
			skr_renderer_frame_end(surfaces, 1);

			// Present
			skr_surface_present(&surface);

			// Progress indicator
			frame++;
			if (frame % 60 == 0) {
				printf("[RENDER] Frame %u (res: %dx%d)\n", frame, size.x, size.y);
			}
		} else {
			// Failed to acquire swapchain image
			skr_renderer_frame_end(NULL, 0);

			if (acquire_result == skr_acquire_needs_resize) {
				skr_surface_resize(&surface);
			} else if (acquire_result != skr_acquire_success) {
				ska_time_sleep(16);
			}
		}
	}

	// Cleanup
	printf("\n[CLEANUP] Shutting down...\n");

	vkDeviceWaitIdle(skr_get_vk_device());
	skr_surface_destroy(&surface);
	skr_shutdown();

	printf("[CLEANUP] sk_renderer shutdown complete\n");

	ska_window_destroy(window);
	ska_shutdown();

	printf("[CLEANUP] sk_app shutdown complete\n");
	printf("\nTotal frames rendered: %u\n", frame);

	return 0;
}
