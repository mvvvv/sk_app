//
// sk_app - Input state implementation

#include "ska_internal.h"

void ska_input_state_init(ska_input_state_t* state) {
	memset(state, 0, sizeof(*state));
	state->cursor_visible = true;
	ska_text_queue_init(&state->text_queue);
	state->text_input_type = ska_text_input_type_text;
}

void ska_input_state_reset(ska_input_state_t* state) {
	memset(state->keyboard, 0, sizeof(state->keyboard));
	state->key_modifiers = 0;
	state->mouse_buttons = 0;
	state->mouse_xrel = 0;
	state->mouse_yrel = 0;
}
