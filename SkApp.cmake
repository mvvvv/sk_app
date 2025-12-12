# sk_app CMake Support
# Provides add_skapp() for building sk_app-based applications on all platforms
#
# Usage:
#   add_skapp(<target_name>
#       [PACKAGE_NAME <com.example.app>]  # Required on Android, ignored elsewhere
#       [APP_NAME "My App"]               # Optional, defaults to target name
#       [ASSETS <path>]                   # Assets directory to bundle with the app
#       [MIN_SDK <version>]               # Android only: minimum SDK version
#       [TARGET_SDK <version>]            # Android only: target SDK version
#       [MANIFEST <path>]                 # Android only: custom manifest (defaults to sk_app's)
#       [RESOURCES <path>]                # Android only: custom resources directory
#       [LIB_NAME <name>]                 # Android only: native library name
#   )
#   target_sources(<target_name> PRIVATE main.c)
#
# After calling add_skapp(), the target has a SKAPP_ASSETS_DIR property containing
# the path where assets will be packaged from. You can output compiled shaders or
# other generated files there and they'll be included in the final app.
#
# On Android: Creates shared library + APK with SkAppActivity, assets packed in APK
# On other platforms: Creates executable, assets copied to assets/ folder next to it

function(add_skapp SKA_TARGET)
	cmake_parse_arguments(SKA "" "PACKAGE_NAME;APP_NAME;ASSETS;MIN_SDK;TARGET_SDK;MANIFEST;RESOURCES;LIB_NAME" "" ${ARGN})

	if(NOT SKA_TARGET)
		message(FATAL_ERROR "add_skapp: target name is required as first argument")
	endif()
	if(NOT SKA_APP_NAME)
		set(SKA_APP_NAME "${SKA_TARGET}")
	endif()

	# Create a staging directory for assets in the build tree
	set(SKA_ASSETS_STAGING_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets_${SKA_TARGET}")
	file(MAKE_DIRECTORY "${SKA_ASSETS_STAGING_DIR}")

	# Stamp file to track when assets were last synced (for APK rebuild triggering)
	set(SKA_ASSETS_STAMP "${CMAKE_CURRENT_BINARY_DIR}/${SKA_TARGET}_assets.stamp")

	if(NOT ANDROID)
		# Desktop platforms: just create an executable
		add_executable(${SKA_TARGET})

		# Store the assets staging directory as a target property
		set_target_properties(${SKA_TARGET} PROPERTIES
			SKAPP_ASSETS_DIR "${SKA_ASSETS_STAGING_DIR}"
		)

		# Build the asset sync commands
		set(ASSET_SYNC_COMMANDS "")
		if(SKA_ASSETS)
			list(APPEND ASSET_SYNC_COMMANDS
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${SKA_ASSETS} ${SKA_ASSETS_STAGING_DIR}
			)
		endif()
		list(APPEND ASSET_SYNC_COMMANDS
			COMMAND ${CMAKE_COMMAND} -E touch ${SKA_ASSETS_STAMP}
		)

		# Create an assets target that always runs and syncs source assets
		add_custom_target(${SKA_TARGET}-assets
			${ASSET_SYNC_COMMANDS}
			COMMENT "Syncing assets for ${SKA_TARGET}"
		)

		# Main target depends on assets being synced
		add_dependencies(${SKA_TARGET} ${SKA_TARGET}-assets)

		# Copy staging dir to output directory after build
		add_custom_command(TARGET ${SKA_TARGET} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
				${SKA_ASSETS_STAGING_DIR}
				$<TARGET_FILE_DIR:${SKA_TARGET}>/assets
			COMMENT "Packaging assets for ${SKA_TARGET}"
		)
	else()
		# Android: create shared library + APK with SkAppActivity
		if(NOT SKA_PACKAGE_NAME)
			message(FATAL_ERROR "add_skapp: PACKAGE_NAME is required for Android builds")
		endif()

		# 1. Create shared library target
		add_library(${SKA_TARGET} SHARED)

		# Store the assets staging directory as a target property
		set_target_properties(${SKA_TARGET} PROPERTIES
			SKAPP_ASSETS_DIR "${SKA_ASSETS_STAGING_DIR}"
		)

		# 2. Default to sk_app's manifest if not specified
		if(NOT SKA_MANIFEST)
			set(SKA_MANIFEST "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/android/AndroidManifest_SkApp.xml")
		endif()

		# 3. Build add_apk arguments - always use staging dir for assets
		set(APK_ARGS
			PACKAGE_NAME ${SKA_PACKAGE_NAME}
			APP_NAME "${SKA_APP_NAME}"
			MANIFEST ${SKA_MANIFEST}
			ASSETS ${SKA_ASSETS_STAGING_DIR}
			ASSETS_STAMP ${SKA_ASSETS_STAMP}
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

		# 4. Build the asset sync commands
		set(ASSET_SYNC_COMMANDS "")
		if(SKA_ASSETS)
			list(APPEND ASSET_SYNC_COMMANDS
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${SKA_ASSETS} ${SKA_ASSETS_STAGING_DIR}
			)
		endif()
		list(APPEND ASSET_SYNC_COMMANDS
			COMMAND ${CMAKE_COMMAND} -E touch ${SKA_ASSETS_STAMP}
		)

		# Create assets target that always runs and syncs source assets
		add_custom_target(${SKA_TARGET}-assets
			${ASSET_SYNC_COMMANDS}
			COMMENT "Syncing assets for ${SKA_TARGET}"
		)

		add_apk(${SKA_TARGET} ${APK_ARGS})

		# Make APK depend on assets being synced
		add_dependencies(${SKA_TARGET}-apk ${SKA_TARGET}-assets)

		# 5. Add sk_app Java code (SkAppActivity)
		apk_add_java_sources(${SKA_TARGET} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/android/java"
			"net/stereokit/sk_app/SkAppActivity.java"
		)

		message(STATUS "sk_app Android target configured: ${SKA_TARGET}")
		message(STATUS "  Package: ${SKA_PACKAGE_NAME}")
		message(STATUS "  Activity: net.stereokit.sk_app.SkAppActivity")
	endif()

	# Link sk_app library
	target_link_libraries(${SKA_TARGET} PRIVATE sk_app)

	# Export the staging dir path to parent scope for convenience
	set(${SKA_TARGET}_ASSETS_DIR "${SKA_ASSETS_STAGING_DIR}" PARENT_SCOPE)
endfunction()
