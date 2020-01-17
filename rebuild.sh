#!/bin/bash -e

BUILD_DIR="build"
ASSETS_DIR="assets"
SYS_DIR="sys"
DB_DIR="db"

#check if we're building in CI
if [ "${HOME}" = "/home/runner" ]; then
	export PATH="${HOME}/cmake-3.15.5-Linux-x86_64/bin:$PATH"
fi

# add pre commit hook
if [[ ! -f '.git/hooks/pre-commit' ]]; then
    ln -s $(pwd)/config/pre-commit.hook $(pwd)/.git/hooks/pre-commit
fi

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
    [ ! -z "$SEPARATE_BUILDS" ] && BUILD_DIR="build-linux"
	elif [ "$1" = "rt1051" ]; then
		echo "Building for RT1051."
		TARGET="Target_RT1051.cmake"
		#BT="DBT_STACK_SEL=\"BlueKitchen\""
		BT="DBT_STACK_SEL=\"Bluetopia\""
    [ ! -z "$SEPARATE_BUILDS" ] && BUILD_DIR="build-rt1051"
	fi

	#if there is no taget specific build directory create one
  if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
  fi

	if [ "$1" = "assets" ] ; then
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
	if [ "$1" = "sys" ] ; then
		echo "Copying whole image to sys folder"
		#enter build directory, erase content and make /sys/assets directory
		cd "$BUILD_DIR"
		mkdir -p "$SYS_DIR" 
		rm -rf "$SYS_DIR"/*
		cp -R ../image/* "$SYS_DIR"
		cp "../module-apps/application-viewer/viewerStates.txt" "sys"
		exit 1
	fi
	if [ "$1" = "release" ]; then
		echo "Building for RT1051."
		TARGET="Target_RT1051.cmake"
		TYPE="RelWithDebInfo" 
	fi
# check whether to build in separate folders for linux/rt1051
elif [ ! -z "$SEPARATE_BUILDS" ]; then # require target arch argument
  echo "SEPARATE_BUILDS require explicit target. Specify either linux or rt1051" >&2
  exit 2
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

#if there is no build directory create one
if [ ! -d "$BUILD_DIR" ]; then
	mkdir "$BUILD_DIR"
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
#	cp -R "$MODULE_GUI_DIR/$ASSETS_DIR" "$SYS_DIR"
	cp -R ../image/* "$SYS_DIR"
#	cp -R "../image/Luts.bin" "$SYS_DIR"
	cp "../module-apps/application-viewer/viewerStates.txt" "$SYS_DIR"
	#language profiles
#	cp -R "../image/assets" "$SYS_DIR"
fi

# prepare env var for cmake call
if [ ! -z "$SEPARATE_BUILDS" ]; then
  CMAKE_BUILD_ENV="env PROJECT_BUILD_DIRECTORY=$BUILD_DIR"
else
  CMAKE_BUILD_ENV=""
fi

if [ "$BUILD_TESTS" = "true" ]; then
	$CMAKE_BUILD_ENV cmake -DCMAKE_EXPORT_COMPILE_COMMANDS="True" -DCMAKE_BUILD_TYPE="$TYPE" -DBUILD_UNIT_TESTS="" -DCMAKE_TOOLCHAIN_FILE=../"$TARGET" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ../
else
	$CMAKE_BUILD_ENV cmake -DCMAKE_EXPORT_COMPILE_COMMANDS="True" -DCMAKE_BUILD_TYPE="$TYPE" -DCMAKE_TOOLCHAIN_FILE=../"$TARGET" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ${BT} ../
fi
