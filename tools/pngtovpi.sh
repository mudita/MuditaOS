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

static uint8_t *data = NULL;
void free_data(void)
{
  if(data != NULL)
    free(data);
  data = NULL;
}

int fill_buffer(void* buffer, ssize_t size)
{
  ssize_t nread;
  do
  {
    nread = read(STDIN_FILENO, buffer, size);
    buffer += nread;
    size -= nread;
  } while (size && nread > 0);
  return nread < 0 ? errno : 0;
}

void write_uint8(uint8_t val)
{
  assert(fputc(val, stdout) == val);
}

void write_uint16(uint16_t val)
{
  write_uint8(val & 0xFF);
  write_uint8(val >> 8);
}

int main()
{
  const int max_header = 32;
  unsigned int width, height;
  intptr_t offset;
  uint16_t vcount;
  uint8_t runlength, value;
  uint8_t *pos, *line_start, *line_end;

  pos = line_start = line_end = NULL;
  width = height = 0;

  data = malloc(max_header);
  atexit(free_data);

  fill_buffer(data, max_header);

  if(sscanf((const char*)data, "P5\n%u %u\n15\n", &width, &height) < 2)
  {
    perror("Bad file signature. Expected \"P5\"");
    exit(EXIT_FAILURE);
  }

  for(value = 3, pos = data; value; ++pos)
    if(*pos == '\n')
      --value;

  offset = pos - data;

  memmove(data, pos, offset);

  data = realloc(data, width * height);
  offset = max_header - offset;
  if(fill_buffer(data + offset, width * height - offset))
  {
    perror("Failed to read data");
    exit(EXIT_FAILURE);
  }

  write_uint16((uint16_t)width);
  write_uint16((uint16_t)height);
  write_uint8(0x10); /* alpha */

  line_start = data;
  for(unsigned int y = 0; y < height; ++y, line_start = line_end)
  {
    vcount = 1;
    pos = line_start;
    line_end = pos + width;

    while(++pos, pos < line_end)
      if(*(pos - 1) != *pos)
        ++vcount;

    write_uint16(vcount);
    pos = line_start;
    while(vcount-- && pos < line_end)
    {
      value = pos[0];

      runlength = 0;
      do { ++pos, ++runlength; }
      while(*(pos - 1) == *pos && pos < line_end);

      write_uint16(0);
      write_uint8(runlength);
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
    build_tool pgmtovpi
    echo "Built: vpgmtovpi"
    exit 0
  fi

  if ! command -v pamseq > /dev/null 2>&1 ||
     ! command -v pamtopnm > /dev/null 2>&1 ||
     ! command -v pngtopam > /dev/null 2>&1 ||
     ! command -v pnmremap > /dev/null 2>&1
  then
    echo "Netpbm must be installed."
    exit 1
  fi

  if [ ! -e "tmp_pgmtovpi" ]; then
    build_tool tmp_pgmtovpi
  fi

  pamseq -tupletype=GRAYSCALE 1 15 | pamtopnm > palette.pgm
  for filename in "$@"
  do
    outname=$(dirname "${filename}")/$(basename "${filename}" .png).vpi
    echo "writing: ${outname}"
    cat "${filename}" | pngtopam -mix -background=white | pnmremap -quiet -nofs -mapfile=palette.pgm | ./tmp_pgmtovpi > "${outname}"
  done
  rm palette.pgm
  rm tmp_pgmtovpi
}

main "$@"
