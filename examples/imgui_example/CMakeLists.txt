# Dear ImGui Example (requires Vulkan and sk_renderer)

# Enable C++ for this example (Dear ImGui is C++)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

message(STATUS "Fetching sk_renderer for ImGui example...")

# Disable sk_renderer's example app and shader compiler to avoid build complexity
set(SKR_BUILD_EXAMPLES OFF CACHE BOOL "Disable sk_renderer examples" FORCE)
set(SKR_BUILD_SKSHADERC OFF CACHE BOOL "Disable shader compilation" FORCE)

FetchContent_Declare(
	sk_renderer
	GIT_REPOSITORY https://github.com/StereoKit/sk_renderer.git
	GIT_TAG        54b2b66d3f1230f1a73f95eabebb78dc653a02b9
	#GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(sk_renderer)
message(STATUS "sk_renderer available")

# Dear ImGui C++ library
FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG        v1.91.6
	GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)
message(STATUS "Dear ImGui fetched")

# cimgui C bindings for ImGui
FetchContent_Declare(
	cimgui
	GIT_REPOSITORY https://github.com/cimgui/cimgui.git
	GIT_TAG        1.91.6
	GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(cimgui)
message(STATUS "cimgui fetched")

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
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_SOURCE_DIR}/imgui_demo.cpp
	${cimgui_SOURCE_DIR}/cimgui.cpp
)

target_link_libraries(imgui_example PRIVATE sk_app sk_renderer)

# Include directories
target_include_directories(imgui_example PRIVATE
	${imgui_SOURCE_DIR}
	${cimgui_SOURCE_DIR}
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
