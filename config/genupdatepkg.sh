#!/bin/bash

# this is useless for now
VERSION_KERNEL=`grep tskKERNEL_VERSION_NUMBER module-os/FreeRTOS/include/task.h | awk '{print $3}'`
VERSION_CODENAME="salvador"

while getopts "c:k:" arg; do
	case $arg in
		c)
			VERSION_CODENAME=$OPTARG
			;;
		k)
			VERSION_KERNEL=$OPTARG
			;;
	esac
done

if [ ! -e config/common.sh ]; then
  echo "No config/common.sh refuse to continue"
  exit 1
fi

source config/common.sh

if [ -z $1 ]; then
	BUILD_PATH=./build-rt1051-Debug
	echo -e "usage $0 <build-folder>\n"
else
	BUILD_PATH=$1
fi

check_target_rt1051 "$BUILD_PATH"

if [ ! -x $(which rhash) ]; then
  echo "Please install the rhash command"
  exit 1
fi

gittag=`git describe --tags | awk -F'-' '{print $2}'`
version=( ${gittag//./ } )
gitrev=`git log --pretty=format:'%h' -n 1`
gitbranch=`git rev-parse --abbrev-ref HEAD`

if [ ! -e $BUILD_PATH/boot.bin ]; then
	echo "No boot.bin in $BUILD_PATH, refuse to continue"
	exit 1
fi

if [ -d update ]; then
	rm -rf update
fi

mkdir -p update/
mkdir -p update/tmp

if [ ! -e config/version.json.template ]; then
	echo "No config/version.json.template"
	exit 1
fi

vjson=update/tmp/version.json
cp config/version.json.template $vjson

sed -i 's/__GIT_BRANCH__/'$gitbranch'/g' $vjson
sed -i 's/__GIT_TAG__/'$gittag'/g' $vjson
sed -i 's/__GIT_REVISION__/'$gitrev'/g' $vjson

sed -i 's/__VERSION_MAJOR__/'${version[0]}'/g' $vjson
sed -i 's/__VERSION_MINOR__/'${version[1]}'/g' $vjson
sed -i 's/__VERSION_PATCH__/'${version[2]}'/g' $vjson

BUILD_HOST=`uname -r`
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

rm -f update/pureos-$lasthash.tar
curpwd=`pwd`
vstr=${version[0]}.${version[1]}.${version[2]}-$gitrev
echo -ne "-- "
cd update/tmp && rhash -ru checksums.txt .
cd $curpwd
echo "-- create tar update/pureos-$vstr.tar"
cd update/tmp && tar -cf ../pureos-$vstr.tar *
cd $curpwd
ls -alh update/pureos-$vstr.tar
