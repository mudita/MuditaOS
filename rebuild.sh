#!/bin/sh

DIRECTORY=build

if [ ! -d "$DIRECTORY" ]; then
  mkdir build
fi

cd build
rm -rf *

#first parameter specifies platform, by default it is Linux
TARGET="Target_Linux.cmake"
TARGET_SET="false"
BUILD_TESTS="false"

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
	cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_UNIT_TESTS="" -DCMAKE_TOOLCHAIN_FILE=../"$TARGET" ../
else
	cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=../"$TARGET" ../
fi
