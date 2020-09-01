#!/bin/bash
# set -eo pipefail
# this is useless for now

if [ ! -e config/common.sh ]; then
  echo "No config/common.sh refuse to continue"
  exit 1
else
  source ./config/common.sh
fi

VERSION_KERNEL=`grep tskKERNEL_VERSION_NUMBER module-os/FreeRTOS/include/task.h | awk '{printf %s, $3}' `
VERSION_CODENAME="salvador"
print_help() {
	echo "Usage: $0 [OPTION] [BUILD-DIR]"
	echo "  -h print help"
	echo "  -c version codename to embed"
	echo "  -k kernel version if needed"
	echo "  -b bootloader file to include"
	echo "  -v bootloader file version"
	echo "  -d build directory"
	echo "  -s sql migration JSON"
	echo
	exit 1
}

while getopts "s:c:k:b:v:d:h" arg; do
	case "${arg}" in
		c)
			VERSION_CODENAME=$OPTARG
			;;
		k)
			VERSION_KERNEL=$OPTARG
			;;
		b)
			BOOTLOADER_FILE=$OPTARG
			;;
		v)
			BOOTLOADER_VERSION=$OPTARG
			;;
		d)
			BUILD_PATH=$OPTARG
			;;
		h)
			print_help
			;;
		*)
			print_help
			;;
	esac
done
shift $((OPTIND-1))

if [ ! -d "$BUILD_PATH" ]; then
	if [ ! -z $BUILD_PATH ]; then
		echo "$BUILD_PATH does not exist"
	fi
	print_help
fi

check_target_rt1051 "$BUILD_PATH"

if [ ! -f $BUILD_PATH/boot.bin ]; then
	echo "No boot.bin in $BUILD_PATH, refuse to continue"
	exit 1
fi

if [ ! -x $(which rhash) ]; then
  echo "Please install the rhash command"
  exit 1
fi

if [ -d update ]; then
	rm -rf update
fi

mkdir -p update/
mkdir -p update/tmp

if [ ! -f config/version.json.template ]; then
	echo "No config/version.json.template"
	exit 1
fi

vjson=update/tmp/version.json
cp config/version.json.template $vjson

sed -i 's/__GIT_BRANCH__/'$PURE_OS_GIT_BRANCH'/g' $vjson
sed -i 's/__GIT_TAG__/'$PURE_OS_GIT_TAG'/g' $vjson
sed -i 's/__GIT_REVISION__/'$PURE_OS_GIT_REV'/g' $vjson

sed -i 's/__VERSION_MAJOR__/'$PURE_OS_VERSION_MAJOR'/g' $vjson
sed -i 's/__VERSION_MINOR__/'$PURE_OS_VERSION_MINOR'/g' $vjson
sed -i 's/__VERSION_PATCH__/'$PURE_OS_VERSION_PATCH'/g' $vjson

BUILD_HOST=`uname -n`
BUILD_USER=`whoami`
BUILD_DATE=`date +'%F-%T'`

sed -i 's/__MISC_CODENAME__/'$VERSION_CODENAME'/g' $vjson
sed -i 's/__MISC_KERNEL__/'$VERSION_KERNEL'/g' $vjson

sed -i 's/__MISC_BUILD_HOST__/'$BUILD_HOST'/g' $vjson
sed -i 's/__MISC_BUILD_DATE__/'$BUILD_DATE'/g' $vjson
sed -i 's/__MISC_BUILD_USER__/'$BUILD_USER'/g' $vjson

echo "-- copy $BUILD_PATH/boot.bin"

cp $BUILD_PATH/boot.bin update/tmp/
for file in $IMAGE_FILES; do
	echo "-- copy $file"
	cp -r image/$file update/tmp/
done

# don't include function files
rm -f update/tmp/.boot.ini
rm -f update/tmp/.boot.ini.crc32

# check for bootloader update
if [ -f "$BOOTLOADER_FILE" ]; then
	echo "-- including bootloader update file: $BOOTLOADER_FILE"
	sed -i 's/__BOOTLOADER_INCLUDED__/1/g' $vjson
	sed -i 's/__BOOTLOADER_INCLUDED_VERSION__/'$BOOTLOADER_VERSION'/g' $vjson
	sed -i 's/__BOOTLOADER_INCLUDED_FILENAME__/'$(basename $BOOTLOADER_FILE)'/g' $vjson
	cp $BOOTLOADER_FILE update/tmp/
else
	sed -i 's/__BOOTLOADER_INCLUDED__/0/g' $vjson
	sed -i 's/__BOOTLOADER_INCLUDED_VERSION__//g' $vjson
	sed -i 's/__BOOTLOADER_INCLUDED_FILENAME__//g' $vjson
fi

rm -f update/*.tar
curpwd=`pwd`
vstr=$PURE_OS_VERSION_MAJOR.$PURE_OS_VERSION_MINOR.$PURE_OS_VERSION_PATCH-$PURE_OS_GIT_REV
echo -ne "-- "
cd update/tmp && rhash -ru checksums.txt .
cd $curpwd
echo "-- create tar update/pureos-$vstr.tar"
cd update/tmp && tar -cf ../pureos-$vstr.tar .
cd $curpwd
ls -alh update/pureos-$vstr.tar
