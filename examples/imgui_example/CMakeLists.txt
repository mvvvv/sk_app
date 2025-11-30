# Dear ImGui Example (requires Vulkan and sk_renderer)

# Enable C++ for this example (Dear ImGui is C++)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)


# Disable sk_renderer's example app and shader compiler to avoid build complexity
set(SKR_BUILD_EXAMPLES OFF CACHE BOOL "Disable sk_renderer examples" FORCE)
set(SKR_BUILD_SKSHADERC OFF CACHE BOOL "Disable shader compilation" FORCE)
message(STATUS "Fetching sk_renderer...")
FetchContent_Declare(
	sk_renderer
	GIT_REPOSITORY https://github.com/StereoKit/sk_renderer.git
	GIT_TAG        8ad5f21f5ca76e474139aaf169859587acd803a0
	#GIT_SHALLOW   TRUE
)
FetchContent_MakeAvailable(sk_renderer)

# cimgui C bindings for ImGui
message(STATUS "Fetching Dear ImGui...")
FetchContent_Declare(
	cimgui
	GIT_REPOSITORY https://github.com/cimgui/cimgui.git
	GIT_TAG        1.91.6
	GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(cimgui)

# Build the imgui example
if(SKA_PLATFORM STREQUAL "ANDROID")
	# Android: build as shared library
	add_library(imgui_example SHARED main.c)
	add_apk(
		TARGET imgui_example
		PACKAGE_NAME net.stereokit.imgui_example
		APP_NAME "ImGui Example"
	)
else()
	# Desktop: build as executable
	add_executable(imgui_example main.c)
endif()

# Link to sk_renderer backend implementation (C++ source)
target_sources(imgui_example PRIVATE
	${sk_renderer_SOURCE_DIR}/example/imgui_backend/imgui_impl_sk_renderer.cpp
	imgui_impl_sk_app.cpp
	${cimgui_SOURCE_DIR}/imgui/imgui.cpp
	${cimgui_SOURCE_DIR}/imgui/imgui_draw.cpp
	${cimgui_SOURCE_DIR}/imgui/imgui_tables.cpp
	${cimgui_SOURCE_DIR}/imgui/imgui_widgets.cpp
	${cimgui_SOURCE_DIR}/imgui/imgui_demo.cpp
	${cimgui_SOURCE_DIR}/cimgui.cpp
)

target_link_libraries(imgui_example PRIVATE sk_app sk_renderer)

# Include directories
target_include_directories(imgui_example PRIVATE
	${cimgui_SOURCE_DIR}
	${cimgui_SOURCE_DIR}/imgui
	${sk_renderer_SOURCE_DIR}/example/imgui_backend
)

# Link math library on Linux
if(UNIX AND NOT APPLE)
	target_link_libraries(imgui_example PRIVATE m)
endif()

# Static link MinGW runtime for cross-compilation
if(MINGW)
	target_link_options(imgui_example PRIVATE -static-libgcc -static-libstdc++ -static)
endif()

message(STATUS "Building Dear ImGui example with sk_app backend")
