mkdir -p build-android-armv7
pushd build-android-armv7
cmake -DCMAKE_TOOLCHAIN_FILE=../android.toolchain.cmake -DANDROID_ABI="armeabi-v7a with NEON" -DANDROID_NATIVE_API_LEVEL=android-21 -DANDROID_FORCE_ARM_BUILD=OFF -DANDROID_STL_FORCE_FEATURES=OFF ..
make
popd
