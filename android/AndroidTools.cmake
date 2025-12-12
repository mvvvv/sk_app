# Android Tools Detection for sk_app
# This module detects and validates the Android SDK, NDK, JDK and build tools.
# Include this once at the root CMakeLists.txt when building for Android.

if(ANDROID_TOOLS_INITIALIZED)
	return()
endif()
set(ANDROID_TOOLS_INITIALIZED TRUE)

###############################################################################
## Find our SDK locations and paths
###############################################################################

# Find the Android SDK
if(NOT DEFINED ANDROID_SDK_ROOT)
	if(DEFINED ENV{ANDROID_HOME})
		set(ANDROID_SDK_ROOT $ENV{ANDROID_HOME})
	elseif(DEFINED ENV{ANDROID_SDK_ROOT})
		set(ANDROID_SDK_ROOT $ENV{ANDROID_SDK_ROOT})
	elseif(DEFINED ANDROID_HOME)
		set(ANDROID_SDK_ROOT ANDROID_HOME)
	endif()
endif()
if (NOT EXISTS "${ANDROID_SDK_ROOT}/platform-tools")
	message(FATAL_ERROR "Android SDK not found. Set ANDROID_HOME or ANDROID_SDK_ROOT as an environment or cmake variable.")
endif()

# Find a build-tools folder in the Android SDK that matches our CMAKE_SYSTEM_VERSION
if(ANDROID_BUILD_TOOLS_VERSION AND EXISTS "${ANDROID_SDK_ROOT}/build-tools/${ANDROID_BUILD_TOOLS_VERSION}")
	set(ANDROID_BUILD_TOOLS_PATH "${ANDROID_SDK_ROOT}/build-tools/${ANDROID_BUILD_TOOLS_VERSION}")
else()
	file(GLOB BUILD_TOOLS_VERSIONS LIST_DIRECTORIES true "${ANDROID_SDK_ROOT}/build-tools/*")
	foreach(VERSION_DIR ${BUILD_TOOLS_VERSIONS})
		get_filename_component(VERSION_NAME ${VERSION_DIR} NAME)
		if(VERSION_NAME MATCHES "${CMAKE_SYSTEM_VERSION}\.")
			set(ANDROID_BUILD_TOOLS_PATH "${VERSION_DIR}")
			break()
		endif()
	endforeach()
endif()
if(NOT EXISTS "${ANDROID_BUILD_TOOLS_PATH}")
	message(STATUS "Can't find build-tools matching ANDROID_BUILD_TOOLS_VERSION ${ANDROID_BUILD_TOOLS_VERSION} or CMAKE_SYSTEM_VERSION ${CMAKE_SYSTEM_VERSION}")
endif()

# Find JDK bin folder
if(NOT DEFINED JAVA_HOME)
	if(DEFINED ENV{JAVA_HOME})
		set(JAVA_HOME $ENV{JAVA_HOME})
	endif()
endif()
if (DEFINED JAVA_HOME)
	cmake_path(APPEND JAVA_HOME_BIN ${JAVA_HOME} "bin")
endif()

message(STATUS "APK build var - ANDROID_SDK_ROOT         : ${ANDROID_SDK_ROOT}")
message(STATUS "APK build var - CMAKE_ANDROID_NDK        : ${CMAKE_ANDROID_NDK}")
message(STATUS "APK build var - ANDROID_BUILD_TOOLS_PATH : ${ANDROID_BUILD_TOOLS_PATH}")
message(STATUS "APK build var - JAVA_HOME_BIN            : ${JAVA_HOME_BIN}")
message(STATUS "APK build var - CMAKE_ANDROID_ARCH_ABI   : ${CMAKE_ANDROID_ARCH_ABI}")
message(STATUS "APK build var - CMAKE_SYSTEM_VERSION     : ${CMAKE_SYSTEM_VERSION}")

###############################################################################
## Get tools for building APKs
###############################################################################

set(AAPT2    "${ANDROID_BUILD_TOOLS_PATH}/aapt2" CACHE INTERNAL "Android AAPT2 tool")
set(AAPT     "${ANDROID_BUILD_TOOLS_PATH}/aapt" CACHE INTERNAL "Android AAPT tool")
set(ZIPALIGN "${ANDROID_BUILD_TOOLS_PATH}/zipalign" CACHE INTERNAL "Android zipalign tool")
set(APKSIGN  "${ANDROID_BUILD_TOOLS_PATH}/apksigner" CACHE INTERNAL "Android apksigner tool")
set(D8       "${ANDROID_BUILD_TOOLS_PATH}/d8" CACHE INTERNAL "Android D8 tool")
find_program(JAVAC   NAMES javac   REQUIRED PATHS ${JAVA_HOME_BIN})
find_program(KEYTOOL NAMES keytool REQUIRED PATHS ${JAVA_HOME_BIN})

###############################################################################
## Keystore for signing APKs (default debug keystore)
###############################################################################

# Set default keystore variables
set(DEFAULT_KEYSTORE       "${CMAKE_CURRENT_LIST_DIR}/debug.keystore")
set(DEFAULT_KEYSTORE_ALIAS "androiddebugkey")
set(DEFAULT_KEYSTORE_PASS  "android")
set(DEFAULT_KEY_ALIAS_PASS "android")

# Check if keystore variables are provided, otherwise use defaults
set(KEYSTORE       "${DEFAULT_KEYSTORE}"       CACHE STRING "Path to the keystore")
set(KEY_ALIAS      "${DEFAULT_KEYSTORE_ALIAS}" CACHE STRING "Alias for the key")
set(KEYSTORE_PASS  "${DEFAULT_KEYSTORE_PASS}"  CACHE STRING "Password for the keystore")
set(KEY_ALIAS_PASS "${DEFAULT_KEY_ALIAS_PASS}" CACHE STRING "Password for the key")

if(NOT EXISTS "${KEYSTORE}")
	message(STATUS "APK keystore not found, generating new keystore...")
	execute_process(COMMAND ${KEYTOOL}
		-genkeypair -v
		-keyalg RSA -keysize 2048 -validity 10000
		-keystore "${KEYSTORE}" -alias "${KEY_ALIAS}"
		-storepass "${KEYSTORE_PASS}" -keypass "${KEY_ALIAS_PASS}"
		-dname "CN=Android Debug,O=Android,C=US"
		RESULT_VARIABLE KEYTOOL_RESULT)
	if(NOT KEYTOOL_RESULT EQUAL "0")
		message(FATAL_ERROR "Failed to create keystore")
	endif()
endif()

message(STATUS "APK using keystore: ${KEYSTORE} with alias ${KEY_ALIAS}")

###############################################################################
## Version information (used by APKs)
###############################################################################

set(APK_VERSION_NAME "${CMAKE_PROJECT_VERSION}" CACHE INTERNAL "APK version name")
math(EXPR APK_VERSION_CODE "${PROJECT_VERSION_MAJOR} * 10000 + ${PROJECT_VERSION_MINOR} * 100 + ${PROJECT_VERSION_PATCH}")
set(APK_VERSION_CODE "${APK_VERSION_CODE}" CACHE INTERNAL "APK version code")
