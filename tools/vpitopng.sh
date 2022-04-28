#!/bin/bash
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

set -euo pipefail

build_tool () {
  cat > "${1}.c" <<\EOF
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>

void write_uint8(uint8_t val)
{
  assert(fputc(val, stdout) == val);
}

uint8_t read_uint8(uint8_t* val)
{
  return read(STDIN_FILENO, val, sizeof(uint8_t));
}

uint8_t read_uint16(uint16_t* val)
{
  return read(STDIN_FILENO, val, sizeof(uint16_t));
}

int main()
{
  uint16_t width;
  uint16_t height;
  uint8_t alpha;

  uint16_t vcount, offset;
  uint8_t runlength, value;

  // get header
  read_uint16(&width);
  read_uint16(&height);
  read_uint8(&alpha); // discard alpha

  // PGM header
  printf("P5\n%u %u\n15\n", width, height);

  while(read_uint16(&vcount) == 2)
  {
    while(vcount && vcount--)
    {
      read_uint16(&offset);
      read_uint8(&runlength);
      read_uint8(&value);
      while(runlength--)
        write_uint8(value);
    }
  }

  exit(EXIT_SUCCESS);
}
EOF
  $CC "${1}.c" -o "${1}"
  rm "${1}.c"
}

usage () {
  echo "$(basename "${0}") [files]"
  echo "$(basename "${0}") --buildtool"
}

main () {
  if [ $# -eq 0 ] ||
     [ "${1}" = "-?" ] ||
     [ "${1}" = "-h" ] ||
     [ "${1}" = "--help" ]
  then
    usage
    exit 1
  fi

  if ! command -v dirname > /dev/null 2>&1 ||
     ! command -v basename > /dev/null 2>&1 ||
     ! command -v cat > /dev/null 2>&1
  then
    echo "Core POSIX utilities are inaccessible."
    exit 1
  fi

  if [ -z ${CC+unbound} ]; then
    if command -v gcc > /dev/null 2>&1; then
      CC=gcc
    elif command -v clang > /dev/null 2>&1; then
      CC=clang
    else
      echo "No C compiler found!"
      exit 1
    fi
  fi

  if [ $# -eq 1 ] &&
     [ "${1}" = "--buildtool" ]
  then
    build_tool vpitopgm
    echo "Built: vpitopgm"
    exit 0
  fi

  if ! command -v pamseq > /dev/null 2>&1 ||
     ! command -v pamtopnm > /dev/null 2>&1 ||
     ! command -v pamtopng > /dev/null 2>&1 ||
     ! command -v pnmremap > /dev/null 2>&1
  then
    echo "Netpbm must be installed."
    exit 1
  fi

  if [ ! -e "tmp_vpitopgm" ]; then
    build_tool tmp_vpitopgm
  fi

  pamseq -tupletype=GRAYSCALE 1 255 | pamtopnm > palette.pgm
  for filename in "$@"
  do
    outname=$(dirname "${filename}")/$(basename "${filename}" .vpi).png
    echo "writing: ${outname}"
    cat "${filename}" | ./tmp_vpitopgm | pnmremap -quiet -nofs -mapfile=palette.pgm | pamtopng > "${outname}"
  done
  rm palette.pgm
  rm tmp_vpitopgm
}

main "$@"
