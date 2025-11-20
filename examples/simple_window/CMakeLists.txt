# Simple Window Example

if(SKA_PLATFORM STREQUAL "ANDROID")
	# On Android, build as shared library
	add_library(simple_window SHARED simple_window.c)
	target_link_libraries(simple_window PRIVATE sk_app)

	# Set up Android APK build
	set(APK_TARGET_NAME "simple_window")
	set(APK_NAME "simple_window")
	set(APK_LIB_NAME "simple_window")
	set(ANDROID_PACKAGE_NAME "com.example.ska.simple_window")
	set(ANDROID_MIN_SDK_VERSION 32)
	set(ANDROID_TARGET_SDK_VERSION 33)
	include(${CMAKE_SOURCE_DIR}/android/AndroidBuild.cmake)
else()
	# On desktop, build as executable
	add_executable(simple_window simple_window.c)
	target_link_libraries(simple_window PRIVATE sk_app)
endif()
