//
// sk_app - File I/O utilities

#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#include "ska_internal.h"

#ifdef SKA_PLATFORM_WIN32
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#define access _access
#define F_OK 0
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

// ============================================================================
// File I/O Implementation
// ============================================================================

SKA_API bool ska_file_read(const char* filename, void** out_data, size_t* out_size) {
	if (!filename) {
		ska_set_error("ska_file_read: NULL filename");
		return false;
	}

	if (!out_data) {
		ska_set_error("ska_file_read: NULL out_data");
		return false;
	}

	FILE* file = fopen(filename, "rb");
	if (!file) {
		ska_set_error("ska_file_read: Failed to open '%s'", filename);
		return false;
	}

	// Get file size
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (file_size < 0) {
		ska_set_error("ska_file_read: Failed to get file size for '%s'", filename);
		fclose(file);
		return false;
	}

	// Allocate buffer
	void* data = malloc((size_t)file_size);
	if (!data) {
		ska_set_error("ska_file_read: Failed to allocate %ld bytes", file_size);
		fclose(file);
		return false;
	}

	// Read file
	size_t bytes_read = fread(data, 1, (size_t)file_size, file);
	fclose(file);

	if (bytes_read != (size_t)file_size) {
		ska_set_error("ska_file_read: Read %zu bytes, expected %ld", bytes_read, file_size);
		free(data);
		return false;
	}

	*out_data = data;
	if (out_size) {
		*out_size = (size_t)file_size;
	}

	return true;
}

SKA_API bool ska_file_read_text(const char* filename, char** out_text) {
	if (!filename) {
		ska_set_error("ska_file_read_text: NULL filename");
		return false;
	}

	if (!out_text) {
		ska_set_error("ska_file_read_text: NULL out_text");
		return false;
	}

	size_t file_size = 0;
	void* data = NULL;
	if (!ska_file_read(filename, &data, &file_size)) {
		return false;
	}

	// Allocate +1 for null terminator
	char* text = (char*)realloc(data, file_size + 1);
	if (!text) {
		ska_set_error("ska_file_read_text: Failed to allocate null terminator");
		free(data);
		return false;
	}

	text[file_size] = '\0';
	*out_text = text;
	return true;
}

SKA_API bool ska_file_write(const char* filename, const void* data, size_t size) {
	if (!filename) {
		ska_set_error("ska_file_write: NULL filename");
		return false;
	}

	if (!data && size > 0) {
		ska_set_error("ska_file_write: NULL data with non-zero size");
		return false;
	}

	FILE* file = fopen(filename, "wb");
	if (!file) {
		ska_set_error("ska_file_write: Failed to open '%s' for writing", filename);
		return false;
	}

	if (size > 0) {
		size_t bytes_written = fwrite(data, 1, size, file);
		fclose(file);

		if (bytes_written != size) {
			ska_set_error("ska_file_write: Wrote %zu bytes, expected %zu", bytes_written, size);
			return false;
		}
	} else {
		fclose(file);
	}

	return true;
}

SKA_API bool ska_file_write_text(const char* filename, const char* text) {
	if (!text) {
		ska_set_error("ska_file_write_text: NULL text");
		return false;
	}

	return ska_file_write(filename, text, strlen(text));
}

SKA_API void ska_file_free_data(void* data) {
	free(data);
}

SKA_API bool ska_file_exists(const char* filename) {
	if (!filename) {
		return false;
	}

#ifdef SKA_PLATFORM_WIN32
	// Windows: use _access
	return _access(filename, F_OK) == 0;
#else
	// POSIX: use access
	return access(filename, F_OK) == 0;
#endif
}

SKA_API size_t ska_file_size(const char* filename) {
	if (!filename) {
		return 0;
	}

#ifdef SKA_PLATFORM_WIN32
	// Windows: use _stat
	struct _stat st;
	if (_stat(filename, &st) != 0) {
		return 0;
	}
	return (size_t)st.st_size;
#else
	// POSIX: use stat
	struct stat st;
	if (stat(filename, &st) != 0) {
		return 0;
	}
	return (size_t)st.st_size;
#endif
}

// ============================================================================
// Asset I/O Implementation (non-Android)
// ============================================================================

#ifndef SKA_PLATFORM_ANDROID

SKA_API bool ska_asset_read(const char* asset_name, void** out_data, size_t* out_size) {
	if (!asset_name) {
		ska_set_error("ska_asset_read: NULL asset_name");
		return false;
	}

	if (!out_data) {
		ska_set_error("ska_asset_read: NULL out_data");
		return false;
	}

	// Build path: try "assets/" first, then "Assets/"
	size_t name_len = strlen(asset_name);
	char* path = (char*)malloc(8 + name_len + 1);  // "assets/" or "Assets/" + name + null
	if (!path) {
		ska_set_error("ska_asset_read: Failed to allocate path buffer");
		return false;
	}

	// Try "assets/" first
	snprintf(path, 8 + name_len + 1, "assets/%s", asset_name);
	if (ska_file_exists(path)) {
		bool result = ska_file_read(path, out_data, out_size);
		free(path);
		return result;
	}

	// Try "Assets/" (common on some platforms)
	snprintf(path, 8 + name_len + 1, "Assets/%s", asset_name);
	if (ska_file_exists(path)) {
		bool result = ska_file_read(path, out_data, out_size);
		free(path);
		return result;
	}

	ska_set_error("ska_asset_read: Asset '%s' not found in assets/ or Assets/", asset_name);
	free(path);
	return false;
}

SKA_API bool ska_asset_read_text(const char* asset_name, char** out_text) {
	if (!asset_name) {
		ska_set_error("ska_asset_read_text: NULL asset_name");
		return false;
	}

	if (!out_text) {
		ska_set_error("ska_asset_read_text: NULL out_text");
		return false;
	}

	size_t file_size = 0;
	void* data = NULL;
	if (!ska_asset_read(asset_name, &data, &file_size)) {
		return false;
	}

	// Allocate +1 for null terminator
	char* text = (char*)realloc(data, file_size + 1);
	if (!text) {
		ska_set_error("ska_asset_read_text: Failed to allocate null terminator");
		free(data);
		return false;
	}

	text[file_size] = '\0';
	*out_text = text;
	return true;
}

#endif // !SKA_PLATFORM_ANDROID
