#!/bin/bash
IMAGE_FILELIST="boot.bin sys/Luts.bin sys/country-codes.db sys/viewerStates.txt sys/assets"

if [ "$1" == "" ]; then
	echo Usage: $0 build-directory
	echo -ne "\tgenerates a pureos-\$(GIT-HASH).tar file\n"
	echo -ne "\tneeds rhash for checksum generation\n\n"
	exit 1
else
	BUILD_DIR=$1
fi

TMP=`mktemp -d`
GIT_HASH=`git log --pretty=format:'%h' -n 1`
OUTFILE="pureos-$GIT_HASH.tar"
mkdir -p $TMP
if [ $? -ne 0 ]; then
	echo "Can't create $TMP directory"
	exit -1
fi

echo "Copying files to $TMP"
for f in $IMAGE_FILELIST; do
	file=$BUILD_DIR/$f
	echo -ne "\t$file\n"
	cp -r $file $TMP
	if [ $? -ne 0 ]; then
		echo "Can't copy $file"
		rm -rf $TMP
		exit -1
	fi
done
oldpwd=`pwd`
cd $TMP
rhash -r -u checksums.txt .
rm -f $oldpwd/$OUTFILE
tar -cf $oldpwd/$OUTFILE .
cd $oldpwd
echo "Cleanup $TMP"
rm -rf $TMP
echo "Created $OUTFILE"
ls -al $OUTFILE
