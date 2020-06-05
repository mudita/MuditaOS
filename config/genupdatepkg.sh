#!/bin/bash
source config/common.sh

if [ -z $1 ]; then
	BUILD_PATH=./build-rt1051-Debug
	echo -e "usage $0 <build-folder>\n"
else
	BUILD_PATH=$1
fi

check_target_rt1051 "$BUILD_PATH"

taginfo=`git describe --tags`
lasthash=`git log --pretty=format:'%h' -n 1`

rm -rf update/tmp
mkdir -p update/
mkdir -p update/tmp

echo -ne '{\n\t{"pureversion":'\"$taginfo\"' },\n' > update/tmp/pureinfo.txt
echo -ne '\t{"lasthash":'\"$lasthash\"' },\n' >> update/tmp/pureinfo.txt
echo -ne '\t{"builddate":'\"$(date)\"'},\n' >> update/tmp/pureinfo.txt
echo -ne '\t{"buildon":'\"$(uname -a)\"'},\n' >> update/tmp/pureinfo.txt
echo -ne '\t{"user":'\"$(whoami)\"'}\n}\n' >> update/tmp/pureinfo.txt

cat update/tmp/pureinfo.txt

echo "-- copy $BUILD_PATH/boot.bin"

cp $BUILD_PATH/boot.bin update/tmp/
for file in $IMAGE_FILES; do
	echo "-- copy $file"
	cp -r image/$file update/tmp/
done

rm -f update/pureos-$lasthash.tar
curpwd=`pwd`

echo -ne "-- "
cd update/tmp && rhash -ru checksums.txt .
cd $curpwd
echo "-- create tar update/pureos-$lasthash.tar"
cd update/tmp && tar -cf ../pureos-$lasthash.tar *
cd $curpwd
ls -alh update/pureos-$lasthash.tar
