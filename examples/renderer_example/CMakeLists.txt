# Renderer Example (requires Vulkan and sk_renderer)

include(FetchContent)

###############################################################################
# App definition
###############################################################################

# Build the renderer example
add_skapp(renderer_example
	PACKAGE_NAME net.stereokit.renderer_example
	APP_NAME "Renderer Example"
	ASSETS ${CMAKE_CURRENT_SOURCE_DIR}/assets
)
target_sources(renderer_example PRIVATE
	renderer_example.c
)

if(UNIX AND NOT APPLE) # Link math library on Linux
	target_link_libraries(renderer_example PRIVATE m)
endif()
if(MINGW) # Static link pthread for MinGW cross-compilation
	target_link_options(renderer_example PRIVATE -static-libgcc -static)
endif()


###############################################################################
# sk_renderer dependency
###############################################################################

message(STATUS "Fetching sk_renderer...")

# Disable sk_renderer's example app to avoid SDL2 dependency
set(SKR_BUILD_EXAMPLES OFF CACHE BOOL "Disable sk_renderer examples" FORCE)
set(SKR_BUILD_SKSHADERC OFF CACHE BOOL "Disable shader compilation" FORCE)
FetchContent_Declare(
	sk_renderer
	GIT_REPOSITORY https://github.com/StereoKit/sk_renderer.git
	GIT_TAG        main
	#GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(sk_renderer)
target_link_libraries(renderer_example PRIVATE sk_renderer)

# Include scene_util and float_math from sk_renderer's example tools
target_include_directories(renderer_example PRIVATE
	${sk_renderer_SOURCE_DIR}/example/tools
)

message(STATUS "Building sk_renderer example")
