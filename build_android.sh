#!/bin/bash
# Build script for Android APK

set -e  # Exit on error

# Check for Android SDK
if [ -z "$ANDROID_HOME" ] && [ -z "$ANDROID_SDK_ROOT" ]; then
	echo "Error: ANDROID_HOME or ANDROID_SDK_ROOT must be set"
	exit 1
fi

# Set Android SDK root
ANDROID_SDK=${ANDROID_HOME:-$ANDROID_SDK_ROOT}

# Check for Android NDK
if [ -z "$ANDROID_NDK" ]; then
	# Try to find NDK in SDK
	if [ -d "$ANDROID_SDK/ndk" ]; then
		# Get the latest NDK version
		ANDROID_NDK=$(ls -d $ANDROID_SDK/ndk/* | sort -V | tail -1)
		echo "Found NDK: $ANDROID_NDK"
	else
		echo "Error: ANDROID_NDK not set and not found in SDK"
		exit 1
	fi
fi

# Build configuration
BUILD_DIR="build-android"
ABI="${ANDROID_ABI:-arm64-v8a}"
API_LEVEL="${ANDROID_API_LEVEL:-24}"

echo "========================================="
echo "Building sk_app for Android"
echo "========================================="
echo "NDK:       $ANDROID_NDK"
echo "SDK:       $ANDROID_SDK"
echo "ABI:       $ABI"
echo "API Level: $API_LEVEL"
echo "========================================="

# Configure
cmake -B $BUILD_DIR \
	-DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
	-DANDROID_ABI=$ABI \
	-DANDROID_PLATFORM=android-$API_LEVEL \
	-DCMAKE_BUILD_TYPE=Release \
	-DANDROID_STL=c++_shared

# Build
cmake --build $BUILD_DIR -j$(nproc)

# Build APK
echo "========================================="
echo "Building APK..."
echo "========================================="
cmake --build $BUILD_DIR --target simple_window-apk

# Show result
if [ -f "$BUILD_DIR/examples/simple_window/simple_window.apk" ]; then
	echo "========================================="
	echo "SUCCESS! APK built:"
	echo "$BUILD_DIR/examples/simple_window/simple_window.apk"
	ls -lh "$BUILD_DIR/examples/simple_window/simple_window.apk"
	echo "========================================="
	echo ""
	echo "To install and run:"
	echo "  adb install -r $BUILD_DIR/examples/simple_window/simple_window.apk"
	echo "  adb shell am start -n com.example.ska.simple_window/android.app.NativeActivity"
	echo ""
	echo "Or use the convenience target:"
	echo "  cmake --build $BUILD_DIR --target simple_window-run"
else
	echo "Error: APK not found!"
	exit 1
fi
