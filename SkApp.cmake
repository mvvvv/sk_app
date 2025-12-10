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
			set(SKA_MANIFEST "${CMAKE_SOURCE_DIR}/android/AndroidManifest_SkApp.xml")
			if(NOT EXISTS "${SKA_MANIFEST}")
				set(SKA_MANIFEST "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/android/AndroidManifest_SkApp.xml")
			endif()
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

		# 4. Add sk_app Java code
		_skapp_add_java(${SKA_TARGET})

		message(STATUS "sk_app Android target configured: ${SKA_TARGET}")
		message(STATUS "  Package: ${SKA_PACKAGE_NAME}")
		message(STATUS "  Activity: net.stereokit.sk_app.SkAppActivity")
	endif()

	# Link sk_app library
	target_link_libraries(${SKA_TARGET} PRIVATE sk_app)
endfunction()

###############################################################################
# Internal: Android Java compilation
###############################################################################

function(_skapp_add_java SJ_TARGET)
	if(NOT SJ_TARGET)
		message(FATAL_ERROR "_skapp_add_java: target name is required")
	endif()
	if(NOT TARGET ${SJ_TARGET}-apk)
		message(FATAL_ERROR "_skapp_add_java: ${SJ_TARGET}-apk target not found")
	endif()

	set(APK_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/apk_${SJ_TARGET}")

	# Find sk_app Java source directory
	set(SK_APP_JAVA_DIR "${CMAKE_SOURCE_DIR}/android/java")
	if(NOT EXISTS "${SK_APP_JAVA_DIR}")
		set(SK_APP_JAVA_DIR "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/android/java")
	endif()

	# Create source directory structure
	file(MAKE_DIRECTORY "${APK_BUILD_DIR}/src/net/stereokit/sk_app")

	# List of Java files to compile
	set(SKAPP_JAVA_SOURCES
		"net/stereokit/sk_app/SkAppActivity.java"
	)

	# Copy Java sources to build directory
	set(SKAPP_JAVA_OUTPUTS "")
	foreach(JAVA_SRC ${SKAPP_JAVA_SOURCES})
		configure_file(
			"${SK_APP_JAVA_DIR}/${JAVA_SRC}"
			"${APK_BUILD_DIR}/src/${JAVA_SRC}"
			COPYONLY
		)
		string(REPLACE ".java" ".class" JAVA_CLASS "${JAVA_SRC}")
		list(APPEND SKAPP_JAVA_OUTPUTS "${APK_BUILD_DIR}/obj/${JAVA_CLASS}")
	endforeach()

	# Find Android SDK jar for compilation
	set(ANDROID_JAR "${ANDROID_SDK_ROOT}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/android.jar")
	if(NOT EXISTS "${ANDROID_JAR}")
		set(ANDROID_JAR "${ANDROID_SDK_ROOT}/platforms/android-32/android.jar")
	endif()

	# Compile all sk_app Java files
	set(JAVA_SRC_FILES "")
	foreach(JAVA_SRC ${SKAPP_JAVA_SOURCES})
		list(APPEND JAVA_SRC_FILES "${APK_BUILD_DIR}/src/${JAVA_SRC}")
	endforeach()

	add_custom_command(
		OUTPUT ${SKAPP_JAVA_OUTPUTS}
		DEPENDS ${JAVA_SRC_FILES}
		COMMAND ${JAVAC}
			-source 1.8 -target 1.8
			-bootclasspath ${ANDROID_JAR}
			-d ${APK_BUILD_DIR}/obj
			-sourcepath ${APK_BUILD_DIR}/src
			${JAVA_SRC_FILES}
		COMMENT "Compiling sk_app Java code for ${SJ_TARGET}"
		VERBATIM
	)

	# Merge sk_app classes with the APK's classes.dex
	add_custom_command(
		OUTPUT ${APK_BUILD_DIR}/obj/skapp_merged.dex
		DEPENDS
			${SKAPP_JAVA_OUTPUTS}
			${APK_BUILD_DIR}/obj/classes.dex
		COMMAND ${CMAKE_COMMAND} -E make_directory ${APK_BUILD_DIR}/obj/skapp_merge_tmp
		COMMAND ${D8} --release
			--output ${APK_BUILD_DIR}/obj/skapp_merge_tmp
			${APK_BUILD_DIR}/obj/classes.dex
			${SKAPP_JAVA_OUTPUTS}
		COMMAND ${CMAKE_COMMAND} -E copy
			${APK_BUILD_DIR}/obj/skapp_merge_tmp/classes.dex
			${APK_BUILD_DIR}/obj/skapp_merged.dex
		COMMAND ${CMAKE_COMMAND} -E rm -rf ${APK_BUILD_DIR}/obj/skapp_merge_tmp
		COMMENT "Merging sk_app Java into classes.dex for ${SJ_TARGET}"
		VERBATIM
	)

	# Swap in the merged DEX
	add_custom_command(
		OUTPUT ${APK_BUILD_DIR}/obj/skapp_java.stamp
		DEPENDS ${APK_BUILD_DIR}/obj/skapp_merged.dex
		COMMAND ${CMAKE_COMMAND} -E copy
			${APK_BUILD_DIR}/obj/classes.dex
			${APK_BUILD_DIR}/obj/classes_original.dex
		COMMAND ${CMAKE_COMMAND} -E copy
			${APK_BUILD_DIR}/obj/skapp_merged.dex
			${APK_BUILD_DIR}/obj/classes.dex
		COMMAND ${CMAKE_COMMAND} -E touch ${APK_BUILD_DIR}/obj/skapp_java.stamp
		COMMENT "Installing sk_app Java for ${SJ_TARGET}"
		VERBATIM
	)

	# Create target for sk_app Java compilation
	add_custom_target(${SJ_TARGET}-skapp-java
		DEPENDS ${APK_BUILD_DIR}/obj/skapp_java.stamp
	)

	# Make APK depend on sk_app Java
	add_dependencies(${SJ_TARGET}-apk ${SJ_TARGET}-skapp-java)

	# Set properties for downstream use
	set_target_properties(${SJ_TARGET}-apk PROPERTIES
		SKAPP_JAVA_ENABLED TRUE
	)
endfunction()
