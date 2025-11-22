# Renderer Example (requires Vulkan and sk_renderer)

include(FetchContent)

message(STATUS "Fetching sk_renderer...")

# Disable sk_renderer's example app to avoid SDL2 dependency
set(SKR_BUILD_EXAMPLES OFF CACHE BOOL "Disable sk_renderer examples" FORCE)
set(SKR_BUILD_SKSHADERC OFF CACHE BOOL "Disable shader compilation" FORCE)

FetchContent_Declare(
	sk_renderer
	GIT_REPOSITORY https://github.com/StereoKit/sk_renderer.git
	GIT_TAG        54b2b66d3f1230f1a73f95eabebb78dc653a02b9
	#GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(sk_renderer)
message(STATUS "sk_renderer available (examples disabled)")

# Build the renderer example
if(SKA_PLATFORM STREQUAL "ANDROID")
	# Android: build as shared library
	add_library(renderer_example SHARED renderer_example.c)
	add_apk(
		TARGET renderer_example
		PACKAGE_NAME net.stereokit.renderer_example
		APP_NAME "Renderer Example"
	)
else()
	# Desktop: build as executable
	add_executable(renderer_example renderer_example.c)
endif()

target_link_libraries(renderer_example PRIVATE sk_app sk_renderer)

# Link math library on Linux
if(UNIX AND NOT APPLE)
	target_link_libraries(renderer_example PRIVATE m)
endif()

# Static link pthread for MinGW cross-compilation
if(MINGW)
	target_link_options(renderer_example PRIVATE -static-libgcc -static)
endif()

message(STATUS "Building sk_renderer example")
