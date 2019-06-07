#!/bin/sh

BUILD_DIR="build"
ASSETS_DIR="assets"

#if there is no build directory create one
if [ ! -d "$BUILD_DIR" ]; then
	mkdir "$BUILD_DIR"
fi

#enter build directory, erase content and make /sys/assets directory
cd "$BUILD_DIR"
rm -rf *
mkdir -p "sys/$ASSETS_DIR"

#if module-gui directory is present try to copy assets to build directory
MODULE_GUI_DIR="../module-gui"
if [ -d "$MODULE_GUI_DIR" ]; then
	
	echo "Found module-gui, copying assets."
	cp -R "$MODULE_GUI_DIR/$ASSETS_DIR" "sys"
	cp -R "../image/Luts.bin" "sys"
fi

#first parameter specifies platform, by default it is Linux
TARGET="Target_Linux.cmake"
TARGET_SET="false"
BUILD_TESTS="false"
TYPE="Debug"

if [ "$1" != "" ]; then
	if [ "$1" = "linux" ]; then
		echo "Building for Linux."
		TARGET="Target_Linux.cmake"
		TARGET_SET="true"
	fi
	if [ "$1" = "rt1051" ]; then
		echo "Building for RT1051."
		TARGET="Target_RT1051.cmake"
	fi
	if [ "$1" = "release" ]; then
		echo "Building for RT1051."
		TARGET="Target_RT1051.cmake"
		TYPE="RelWithDebInfo" 
	fi
else
	echo "No platform specified, using target Linux."
fi

#second flag is processed only if platform flag was set correctly
if [ "$TARGET_SET" = "true" ]; then
	if [ "$2" != "" ]; then
		if [ "$2" = "test" ]; then
			echo "Building with tests."
			BUILD_TESTS="true"
		fi
	else
		echo "Building without tests."
	fi
fi

if [ "$BUILD_TESTS" = "true" ]; then
	cmake -DCMAKE_BUILD_TYPE="$TYPE" -DBUILD_UNIT_TESTS="" -DCMAKE_TOOLCHAIN_FILE=../"$TARGET" ../
else
	cmake -DCMAKE_BUILD_TYPE="$TYPE" -DCMAKE_TOOLCHAIN_FILE=../"$TARGET" ../
fi
