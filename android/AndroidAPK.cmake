# Android APK Build Function for sk_app
# Provides add_apk() function to build APK files from native libraries

# add_apk: Create an APK build target from a shared library target
#
# Usage:
#   add_apk(<target_name>
#       PACKAGE_NAME <com.example.package>
#       [APP_NAME <display_name>]
#       [MIN_SDK <version>]
#       [TARGET_SDK <version>]
#       [MANIFEST <path/to/AndroidManifest.xml>]
#       [RESOURCES <path/to/resources>]
#       [LIB_NAME <native_lib_name>]
#   )
#
# Creates targets:
#   <target_name>-apk     - Build the APK
#   <target_name>-install - Install APK to device via adb
#   <target_name>-run     - Install and launch the app
function(add_apk APK_TARGET)
	# Parse remaining arguments
	cmake_parse_arguments(APK
		""  # Options (boolean flags)
		"PACKAGE_NAME;APP_NAME;MIN_SDK;TARGET_SDK;MANIFEST;RESOURCES;LIB_NAME"  # Single-value args
		""  # Multi-value args
		${ARGN}
	)

	# Validate required arguments
	if(NOT APK_TARGET)
		message(FATAL_ERROR "add_apk: target name is required as first argument")
	endif()
	if(NOT APK_PACKAGE_NAME)
		message(FATAL_ERROR "add_apk: PACKAGE_NAME argument is required")
	endif()
	if(NOT TARGET ${APK_TARGET})
		message(FATAL_ERROR "add_apk: TARGET '${APK_TARGET}' is not a valid CMake target")
	endif()

	# Verify target is a shared library
	get_target_property(TARGET_TYPE ${APK_TARGET} TYPE)
	if(NOT ${TARGET_TYPE} STREQUAL "SHARED_LIBRARY")
		message(FATAL_ERROR "add_apk: TARGET '${APK_TARGET}' must be a SHARED_LIBRARY (found ${TARGET_TYPE})")
	endif()

	# Set defaults
	if(NOT APK_MIN_SDK)
		set(APK_MIN_SDK 32)
	endif()
	if(NOT APK_TARGET_SDK)
		set(APK_TARGET_SDK 33)
	endif()
	if(NOT APK_MANIFEST)
		set(APK_MANIFEST "${CMAKE_SOURCE_DIR}/android/AndroidManifest.xml")
	endif()
	if(NOT APK_RESOURCES)
		set(APK_RESOURCES "${CMAKE_SOURCE_DIR}/android/resources")
	endif()
	if(NOT APK_LIB_NAME)
		set(APK_LIB_NAME "${APK_TARGET}")
	endif()
	if(NOT APK_APP_NAME)
		set(APK_APP_NAME "${APK_TARGET}")
	endif()

	# Validate paths exist
	if(NOT EXISTS "${APK_MANIFEST}")
		message(FATAL_ERROR "add_apk: MANIFEST file not found: ${APK_MANIFEST}")
	endif()
	if(NOT EXISTS "${APK_RESOURCES}")
		message(FATAL_ERROR "add_apk: RESOURCES directory not found: ${APK_RESOURCES}")
	endif()

	message(STATUS "Configuring APK: ${APK_TARGET}")
	message(STATUS "  Package: ${APK_PACKAGE_NAME}")
	message(STATUS "  Min SDK: ${APK_MIN_SDK}, Target SDK: ${APK_TARGET_SDK}")

	###########################################################################
	## Add Android glue code to target
	###########################################################################

	target_link_libraries(${APK_TARGET} PRIVATE android log)
	target_include_directories(${APK_TARGET} PUBLIC ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue)
	target_sources(${APK_TARGET} PRIVATE
		${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

	###########################################################################
	## Get list of shared libraries to pack
	###########################################################################

	get_target_property(PROJECT_LIBRARIES ${APK_TARGET} LINK_LIBRARIES)
	set(APK_SRC_LIBRARIES $<TARGET_FILE:${APK_TARGET}>)
	set(APK_COPY_LIBRARIES lib/${CMAKE_ANDROID_ARCH_ABI}/$<TARGET_FILE_NAME:${APK_TARGET}>)
	foreach(CURR ${PROJECT_LIBRARIES})
		if (TARGET ${CURR})
			get_target_property(CURR_TARGET_TYPE ${CURR} TYPE)
			if(${CURR_TARGET_TYPE} STREQUAL "SHARED_LIBRARY")
				list(APPEND APK_SRC_LIBRARIES $<TARGET_FILE:${CURR}>)
				list(APPEND APK_COPY_LIBRARIES lib/${CMAKE_ANDROID_ARCH_ABI}/$<TARGET_FILE_NAME:${CURR}>)
			endif()
		endif()
	endforeach()

	###########################################################################
	## Set up build directories and paths
	###########################################################################

	set(APK_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/apk_${APK_TARGET}")
	set(APK_BASE      ${APK_BUILD_DIR}/${APK_TARGET}.1.base.apk)
	set(APK_UNALIGNED ${APK_BUILD_DIR}/${APK_TARGET}.2.unaligned.apk)
	set(APK_UNSIGNED  ${APK_BUILD_DIR}/${APK_TARGET}.3.unsigned.apk)
	set(OUTPUT_APK    ${CMAKE_CURRENT_BINARY_DIR}/${APK_TARGET}.apk)

	# Make folders
	file(MAKE_DIRECTORY
		"${APK_BUILD_DIR}/obj"
		"${APK_BUILD_DIR}/resources/values"
		"${APK_BUILD_DIR}/lib/${CMAKE_ANDROID_ARCH_ABI}")

	# Set manifest variables for substitution
	set(ANDROID_PACKAGE_NAME ${APK_PACKAGE_NAME})
	set(ANDROID_MIN_SDK_VERSION ${APK_MIN_SDK})
	set(ANDROID_TARGET_SDK_VERSION ${APK_TARGET_SDK})
	set(APK_LIB_NAME ${APK_LIB_NAME})

	# Configure AndroidManifest.xml
	configure_file(
		${APK_MANIFEST}
		${APK_BUILD_DIR}/obj/AndroidManifest.xml
		@ONLY)

	# Generate custom strings.xml with the app name
	file(WRITE ${APK_BUILD_DIR}/resources/values/strings.xml
"<?xml version=\"1.0\" encoding=\"utf-8\"?>
<resources>
	<string name=\"app_name\">${APK_APP_NAME}</string>
</resources>
")

	###########################################################################
	## Build Java boilerplate (minimal for NativeActivity)
	###########################################################################

	file(WRITE ${APK_BUILD_DIR}/src/android/Empty.java "public class Empty {}")
	add_custom_command(
		DEPENDS ${APK_BUILD_DIR}/src/android/Empty.java
		OUTPUT  ${APK_BUILD_DIR}/obj/classes.dex
		COMMAND ${JAVAC} -d ${APK_BUILD_DIR}/obj -sourcepath src ${APK_BUILD_DIR}/src/android/Empty.java
		COMMAND ${D8} --release ${APK_BUILD_DIR}/obj/Empty.class --output ${APK_BUILD_DIR}/obj
		COMMENT "Building Java boilerplate for ${APK_TARGET}" )

	###########################################################################
	## Compile Android resources
	###########################################################################

	# Compile shared resources (icons, etc.)
	add_custom_command(
		OUTPUT  ${APK_BUILD_DIR}/obj/apk_resources_shared.zip
		DEPENDS ${APK_RESOURCES}
		COMMAND ${AAPT2} compile
			--dir ${APK_RESOURCES}
			-o ${APK_BUILD_DIR}/obj/apk_resources_shared.zip
		COMMENT "Compiling shared resources for ${APK_TARGET}" )

	# Compile per-APK resources (custom app name)
	add_custom_command(
		OUTPUT  ${APK_BUILD_DIR}/obj/apk_resources_custom.zip
		DEPENDS ${APK_BUILD_DIR}/resources/values/strings.xml
		COMMAND ${AAPT2} compile
			--dir ${APK_BUILD_DIR}/resources
			-o ${APK_BUILD_DIR}/obj/apk_resources_custom.zip
		COMMENT "Compiling custom resources for ${APK_TARGET}" )

	###########################################################################
	## Assemble base APK (resources + manifest)
	###########################################################################

	add_custom_command(
		DEPENDS
			${APK_BUILD_DIR}/obj/classes.dex
			${APK_BUILD_DIR}/obj/apk_resources_shared.zip
			${APK_BUILD_DIR}/obj/apk_resources_custom.zip
			${APK_BUILD_DIR}/obj/AndroidManifest.xml
		OUTPUT
			${APK_BASE}
		COMMAND ${CMAKE_COMMAND} -E rm -f ${APK_BASE}
		COMMAND ${AAPT2} link
			-o ${APK_BASE}
			--manifest ${APK_BUILD_DIR}/obj/AndroidManifest.xml
			-I ${ANDROID_SDK_ROOT}/platforms/android-${APK_MIN_SDK}/android.jar
			${APK_BUILD_DIR}/obj/apk_resources_shared.zip
			${APK_BUILD_DIR}/obj/apk_resources_custom.zip
		COMMAND cd ${APK_BUILD_DIR}/obj && ${AAPT} add ${APK_BASE} classes.dex
		COMMENT "Building base APK for ${APK_TARGET}")

	###########################################################################
	## Assemble final APK (add native libraries, align, sign)
	###########################################################################

	add_custom_command(
		DEPENDS
			${APK_TARGET}
			${APK_BASE}
		OUTPUT
			${OUTPUT_APK}
		COMMAND ${CMAKE_COMMAND} -E rm -f
			${APK_UNALIGNED}
			${APK_UNSIGNED}
			${OUTPUT_APK}
		COMMAND ${CMAKE_COMMAND} -E copy
			${APK_SRC_LIBRARIES}
			${APK_BUILD_DIR}/lib/${CMAKE_ANDROID_ARCH_ABI}/
		COMMAND ${CMAKE_COMMAND} -E copy ${APK_BASE} ${APK_UNALIGNED}
		COMMAND cd ${APK_BUILD_DIR} && ${AAPT} add ${APK_UNALIGNED} ${APK_COPY_LIBRARIES}
		COMMAND ${ZIPALIGN} -p 4 ${APK_UNALIGNED} ${APK_UNSIGNED}
		COMMAND ${APKSIGN} sign --ks ${KEYSTORE} --ks-key-alias ${KEY_ALIAS} --ks-pass pass:${KEYSTORE_PASS} --key-pass pass:${KEY_ALIAS_PASS} --out ${OUTPUT_APK} ${APK_UNSIGNED}
		COMMENT "Building final APK: ${APK_TARGET}.apk")

	###########################################################################
	## Create CMake targets
	###########################################################################

	# APK build target (part of ALL so it builds by default)
	add_custom_target(${APK_TARGET}-apk ALL
		DEPENDS
			${APK_TARGET}
			${OUTPUT_APK}
		COMMENT "Building Android APK: ${APK_TARGET}.apk")

	# Install target (adb install)
	add_custom_target(${APK_TARGET}-install
		DEPENDS ${APK_TARGET}-apk
		COMMAND ${ANDROID_SDK_ROOT}/platform-tools/adb install -r ${OUTPUT_APK}
		COMMENT "Installing ${APK_TARGET}.apk to device")

	# Run target (install + launch)
	add_custom_target(${APK_TARGET}-run
		DEPENDS ${APK_TARGET}-apk
		COMMAND ${ANDROID_SDK_ROOT}/platform-tools/adb install -r ${OUTPUT_APK}
		COMMAND ${ANDROID_SDK_ROOT}/platform-tools/adb shell am start -n ${APK_PACKAGE_NAME}/android.app.NativeActivity
		COMMENT "Installing and running ${APK_TARGET}.apk")

	# Set target properties for introspection
	set_target_properties(${APK_TARGET}-apk PROPERTIES
		APK_OUTPUT_PATH "${OUTPUT_APK}"
		APK_PACKAGE_NAME "${APK_PACKAGE_NAME}"
	)

endfunction()
