cmake_minimum_required(VERSION 3.15)
project(sk_app VERSION 1.0.0 LANGUAGES C)

# C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Options
option(SKA_BUILD_SHARED   "Build shared library"       OFF)
option(SKA_BUILD_EXAMPLES "Build example applications" ON)

# Release build optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	add_compile_options(
		-fno-exceptions
		-fno-unwind-tables
		-fno-asynchronous-unwind-tables
		-fdata-sections
		-ffunction-sections
	)

	add_link_options(
		-Wl,--gc-sections
		-Wl,--strip-all
	)
endif()

###############################################################################
# Create sk_app library target
###############################################################################

# Create library (shared or static)
if(SKA_BUILD_SHARED)
	add_library(sk_app SHARED)
	target_compile_definitions(sk_app
		PRIVATE SKA_BUILD_SHARED SKA_EXPORT
		INTERFACE SKA_BUILD_SHARED
	)
else()
	add_library(sk_app STATIC)
endif()

# Add common sources
target_sources(sk_app PRIVATE
	src/ska_common.c
	src/ska_event.c
	src/ska_input.c
	src/ska_text.c
	src/ska_file.c
)

# Include directories
target_include_directories(sk_app
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Compiler warnings
if(MSVC)
	target_compile_options(sk_app PRIVATE /W4 /WX-)
else()
	target_compile_options(sk_app PRIVATE
		-Wall -Wextra -Wpedantic
		-Wno-unused-parameter
		-Wno-missing-field-initializers
	)
endif()

###############################################################################
# Platform-specific configuration
###############################################################################

if(WIN32)
	set(SKA_PLATFORM "WIN32")
	target_compile_definitions(sk_app PRIVATE SKA_PLATFORM_WIN32)

	# Windows-specific sources
	target_sources(sk_app PRIVATE src/ska_win32.c)

	# Static link MinGW runtime for cross-compilation
	if(MINGW)
		target_link_options(sk_app PRIVATE -static-libgcc -static-libstdc++ -static)
	endif()

elseif(ANDROID)
	set(SKA_PLATFORM "ANDROID")
	target_compile_definitions(sk_app PRIVATE SKA_PLATFORM_ANDROID)

	# Android-specific sources
	target_sources(sk_app PRIVATE
		src/ska_android.c
		${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
	)

	# Android includes and libraries
	target_include_directories(sk_app PRIVATE
		${CMAKE_ANDROID_NDK}/sources/android/native_app_glue
	)
	target_link_libraries(sk_app PRIVATE android log)

elseif(APPLE)
	set(SKA_PLATFORM "MACOS")
	target_compile_definitions(sk_app PRIVATE SKA_PLATFORM_MACOS)

	# macOS-specific sources
	target_sources(sk_app PRIVATE src/ska_macos.m)

	# macOS framework dependencies
	find_library(COCOA_LIBRARY Cocoa REQUIRED)
	find_library(IOKIT_LIBRARY IOKit REQUIRED)
	find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
	target_link_libraries(sk_app PRIVATE
		${COCOA_LIBRARY}
		${IOKIT_LIBRARY}
		${COREVIDEO_LIBRARY}
	)

elseif(UNIX)
	set(SKA_PLATFORM "LINUX")
	target_compile_definitions(sk_app PRIVATE SKA_PLATFORM_LINUX)

	# Linux-specific sources
	target_sources(sk_app PRIVATE src/ska_linux_x11.c)

	# X11 dependencies
	find_package(X11 REQUIRED)
	target_link_libraries(sk_app PRIVATE
		X11::X11
		X11::Xrandr
		X11::Xcursor
		X11::Xi
	)

else()
	message(FATAL_ERROR "Unsupported platform")
endif()

message(STATUS "Building for platform: ${SKA_PLATFORM}")

###############################################################################
# Install rules
###############################################################################

include(GNUInstallDirs)

set(SK_APP_INSTALL_TARGETS sk_app)

install(TARGETS ${SK_APP_INSTALL_TARGETS}
	EXPORT sk_app-targets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


install(FILES include/sk_app.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT sk_app-targets
	FILE sk_app-config.cmake
	NAMESPACE ska::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sk_app
)

###############################################################################
# Examples
###############################################################################

if(SKA_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

###############################################################################
# Configuration summary
###############################################################################

message(STATUS "")
message(STATUS "sk_app configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Platform: ${SKA_PLATFORM}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Shared library: ${SKA_BUILD_SHARED}")
message(STATUS "  Build examples: ${SKA_BUILD_EXAMPLES}")
message(STATUS "")
