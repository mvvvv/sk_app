# sk_app CMake Support
# Provides add_skapp() for building sk_app-based applications on all platforms
#
# Usage:
#   add_skapp(<target_name>
#       [PACKAGE_NAME <com.example.app>]  # Required on Android, ignored elsewhere
#       [APP_NAME "My App"]               # Optional, defaults to target name
#       [MIN_SDK <version>]               # Android only: minimum SDK version
#       [TARGET_SDK <version>]            # Android only: target SDK version
#       [MANIFEST <path>]                 # Android only: custom manifest (defaults to sk_app's)
#       [RESOURCES <path>]                # Android only: custom resources directory
#       [LIB_NAME <name>]                 # Android only: native library name
#   )
#   target_sources(<target_name> PRIVATE main.c)
#
# On Android: Creates shared library + APK with SkAppActivity
# On other platforms: Creates executable (Android options ignored)

function(add_skapp SKA_TARGET)
	cmake_parse_arguments(SKA "" "PACKAGE_NAME;APP_NAME;MIN_SDK;TARGET_SDK;MANIFEST;RESOURCES;LIB_NAME" "" ${ARGN})

	if(NOT SKA_TARGET)
		message(FATAL_ERROR "add_skapp: target name is required as first argument")
	endif()
	if(NOT SKA_APP_NAME)
		set(SKA_APP_NAME "${SKA_TARGET}")
	endif()

	if(NOT ANDROID)
		# Desktop platforms: just create an executable
		add_executable(${SKA_TARGET})
	else()
		# Android: create shared library + APK with SkAppActivity
		if(NOT SKA_PACKAGE_NAME)
			message(FATAL_ERROR "add_skapp: PACKAGE_NAME is required for Android builds")
		endif()

		# 1. Create shared library target
		add_library(${SKA_TARGET} SHARED)

		# 2. Default to sk_app's manifest if not specified
		if(NOT SKA_MANIFEST)
			set(SKA_MANIFEST "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/android/AndroidManifest_SkApp.xml")
		endif()

		# 3. Build add_apk arguments
		set(APK_ARGS
			PACKAGE_NAME ${SKA_PACKAGE_NAME}
			APP_NAME "${SKA_APP_NAME}"
			MANIFEST ${SKA_MANIFEST}
		)
		if(SKA_MIN_SDK)
			list(APPEND APK_ARGS MIN_SDK ${SKA_MIN_SDK})
		endif()
		if(SKA_TARGET_SDK)
			list(APPEND APK_ARGS TARGET_SDK ${SKA_TARGET_SDK})
		endif()
		if(SKA_RESOURCES)
			list(APPEND APK_ARGS RESOURCES ${SKA_RESOURCES})
		endif()
		if(SKA_LIB_NAME)
			list(APPEND APK_ARGS LIB_NAME ${SKA_LIB_NAME})
		endif()

		add_apk(${SKA_TARGET} ${APK_ARGS})

		# 4. Add sk_app Java code (SkAppActivity)
		apk_add_java_sources(${SKA_TARGET} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/android/java"
			"net/stereokit/sk_app/SkAppActivity.java"
		)

		message(STATUS "sk_app Android target configured: ${SKA_TARGET}")
		message(STATUS "  Package: ${SKA_PACKAGE_NAME}")
		message(STATUS "  Activity: net.stereokit.sk_app.SkAppActivity")
	endif()

	# Link sk_app library
	target_link_libraries(${SKA_TARGET} PRIVATE sk_app)
endfunction()
