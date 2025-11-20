# sk_app

A small cross-platform library for windows / vulkan surfaces / input / OS functionality. Targets Win32, Linux, Android, and MacOS.

## Building

From the project root:

### Linux

```sh
# To configure
cmake -B build

# To build
cmake --build build -j8

# To run
./build/examples/simple_window
```

### Windows (Cross-compile with MinGW)

```sh
# To configure a Windows build
cmake -B build-win -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++

# To build
cmake --build build-win -j8

# To run with Wine
wine ./build-win/examples/simple_window.exe
```

### Android

```sh
# Prerequisites:
# - ANDROID_HOME or ANDROID_SDK_ROOT environment variable set
# - ANDROID_NDK environment variable (optional - auto-detected from SDK)
# - Java JDK installed for signing APKs

# Quick build (uses convenience script)
./build_android.sh

# Manual build
cmake -B build-android \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-32 \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build-android -j8

# Build the APK
cmake --build build-android --target simple_window-apk

# Install and run on device
adb install -r build-android/examples/simple_window.apk
adb shell am start -n com.example.ska.simple_window/android.app.NativeActivity

# Or use the convenience target
cmake --build build-android --target simple_window-run
```