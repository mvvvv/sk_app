//
// sk_app - Text input queue implementation

#include "ska_internal.h"

// ============================================================================
// UTF-8 to UTF-32 Conversion
// ============================================================================

static uint32_t ska_utf8_decode(const char** utf8_ptr) {
	const unsigned char* utf8 = (const unsigned char*)*utf8_ptr;
	uint32_t codepoint = 0;
	int32_t bytes = 0;

	if (utf8[0] == 0) {
		return 0;
	} else if ((utf8[0] & 0x80) == 0x00) {
		// 1-byte sequence: 0xxxxxxx
		codepoint = utf8[0];
		bytes = 1;
	} else if ((utf8[0] & 0xE0) == 0xC0) {
		// 2-byte sequence: 110xxxxx 10xxxxxx
		codepoint = ((utf8[0] & 0x1F) << 6) |
					(utf8[1] & 0x3F);
		bytes = 2;
	} else if ((utf8[0] & 0xF0) == 0xE0) {
		// 3-byte sequence: 1110xxxx 10xxxxxx 10xxxxxx
		codepoint = ((utf8[0] & 0x0F) << 12) |
					((utf8[1] & 0x3F) << 6) |
					(utf8[2] & 0x3F);
		bytes = 3;
	} else if ((utf8[0] & 0xF8) == 0xF0) {
		// 4-byte sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		codepoint = ((utf8[0] & 0x07) << 18) |
					((utf8[1] & 0x3F) << 12) |
					((utf8[2] & 0x3F) << 6) |
					(utf8[3] & 0x3F);
		bytes = 4;
	} else {
		// Invalid UTF-8
		bytes = 1;
		codepoint = 0xFFFD;  // Replacement character
	}

	*utf8_ptr += bytes;
	return codepoint;
}

// ============================================================================
// Text Queue Implementation
// ============================================================================

void ska_text_queue_init(ska_text_queue_t* queue) {
	memset(queue, 0, sizeof(*queue));
}

void ska_text_queue_push_utf8(ska_text_queue_t* queue, const char* utf8) {
	if (!utf8) return;

	const char* ptr = utf8;
	while (*ptr) {
		uint32_t codepoint = ska_utf8_decode(&ptr);
		if (codepoint != 0) {
			// Inline ska_text_queue_push
			if (queue->count >= SKA_TEXT_QUEUE_SIZE) {
				ska_log(ska_log_warn, "Text queue full, dropping codepoint U+%04X", codepoint);
				break;
			}

			queue->codepoints[queue->write_pos] = codepoint;
			queue->write_pos = (queue->write_pos + 1) % SKA_TEXT_QUEUE_SIZE;
			queue->count++;
		}
	}
}

// ============================================================================
// Public Text Input API
// ============================================================================

SKA_API bool ska_text_has_input(void) {
	ska_text_queue_t* queue = &g_ska.input_state.text_queue;
	return queue->count != 0;
}

SKA_API uint32_t ska_text_consume(void) {
	ska_text_queue_t* queue = &g_ska.input_state.text_queue;
	if (queue->count == 0) {
		return 0;
	}

	uint32_t codepoint = queue->codepoints[queue->read_pos];
	queue->read_pos = (queue->read_pos + 1) % SKA_TEXT_QUEUE_SIZE;
	queue->count--;
	return codepoint;
}

SKA_API uint32_t ska_text_peek(void) {
	ska_text_queue_t* queue = &g_ska.input_state.text_queue;
	if (queue->count == 0) {
		return 0;
	}

	return queue->codepoints[queue->read_pos];
}

SKA_API void ska_text_reset(void) {
	ska_text_queue_t* queue = &g_ska.input_state.text_queue;
	queue->read_pos = 0;
	queue->write_pos = 0;
	queue->count = 0;
}

SKA_API void ska_virtual_keyboard_show(bool visible, ska_text_input_type_ type) {
#if defined(SKA_PLATFORM_ANDROID)
	if (visible) {
		g_ska.input_state.text_input_type = type;
	}
	ska_platform_show_virtual_keyboard(visible, type);
	g_ska.input_state.virtual_keyboard_visible = visible;
#else
	(void)visible;
	(void)type;
	// No-op on desktop
#endif
}

SKA_API bool ska_virtual_keyboard_is_visible(void) {
#if defined(SKA_PLATFORM_ANDROID)
	return g_ska.input_state.virtual_keyboard_visible;
#else
	return false;
#endif
}
