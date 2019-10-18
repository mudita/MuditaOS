#!/bin/bash -e

BUILD_DIR="build"
ASSETS_DIR="assets"
SYS_DIR="sys"
DB_DIR="db"

function check_submodules() {
    local gitlist=$(cat .gitmodules | grep path | cut -d ' ' -f 3)
    for el in $gitlist; do
        if [[ ! -f "./${el}/.git" ]]; then
            echo "Please init (all) submodules"
            echo "not found: ./${el}/.git break!"
            echo "please run:"
            echo "git submodule update --init --recursive"
            exit 1
        fi
    done
}

# check for submodules
check_submodules

#if there is no build directory create one
if [ ! -d "$BUILD_DIR" ]; then
	mkdir "$BUILD_DIR"
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
		BT="DBT_STACK_SEL=\"BlueKitchen\""
	fi
	if [ "$1" = "assets" ]; then
		echo "Copying assets folder"
		cd "$BUILD_DIR"
		#if there is no sys directory create one
		if [ ! -d "$SYS_DIR" ]; then
			mkdir "$SYS_DIR"
		fi
		cd "$SYS_DIR"
		#if there is no assets directory create one
		if [ ! -d "$ASSETS_DIR" ]; then
			mkdir "$ASSETS_DIR"
		fi	
		cd "$ASSETS_DIR"
		rm -rf *
		cd ../..
		cp -R ../image/assets/* sys/assets
		exit 1
	fi
	if [ "$1" = "rt1051" ]; then
		echo "Building for RT1051."
		TARGET="Target_RT1051.cmake"
		#BT="DBT_STACK_SEL=\"BlueKitchen\""
		BT="DBT_STACK_SEL=\"Bluetopia\""
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


#enter build directory, erase content and make /sys/assets directory
cd "$BUILD_DIR"
rm -rf *
mkdir -p "sys/$ASSETS_DIR"
mkdir -p "sys/$DB_DIR"

#if module-gui directory is present try to copy assets to build directory
MODULE_GUI_DIR="../module-gui"
if [ -d "$MODULE_GUI_DIR" ]; then
	
	echo "Found module-gui, copying assets."
#	cp -R "$MODULE_GUI_DIR/$ASSETS_DIR" "sys"
	cp -R ../image/* sys
#	cp -R "../image/Luts.bin" "sys"
	cp "../module-apps/application-viewer/viewerStates.txt" "sys"
	#language profiles
#	cp -R "../image/assets" "sys"
fi

if [ "$BUILD_TESTS" = "true" ]; then
	cmake -DCMAKE_EXPORT_COMPILE_COMMANDS="True" -DCMAKE_BUILD_TYPE="$TYPE" -DBUILD_UNIT_TESTS="" -DCMAKE_TOOLCHAIN_FILE=../"$TARGET" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ../
else
	cmake -DCMAKE_EXPORT_COMPILE_COMMANDS="True" -DCMAKE_BUILD_TYPE="$TYPE" -DCMAKE_TOOLCHAIN_FILE=../"$TARGET" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ${BT} ../
fi
