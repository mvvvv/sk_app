// Dear ImGui sk_app Backend
// Platform backend for sk_app (window/input handling)
// C-compatible header for integration with both C and C++ code

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ska_window_t;
struct ska_event_t;

// Initialization and lifecycle
bool ImGui_ImplSkApp_Init         (struct ska_window_t* window);
void ImGui_ImplSkApp_Shutdown     (void);
void ImGui_ImplSkApp_NewFrame     (void);

// Event processing - call this for each event from ska_event_poll()
// Returns true if ImGui wants to capture this event
bool ImGui_ImplSkApp_ProcessEvent (const struct ska_event_t* event);

#ifdef __cplusplus
}
#endif
