#!/bin/sh

DIRECTORY=build

if [ ! -d "$DIRECTORY" ]; then
  mkdir build
fi

cd build
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug ../
