#!/bin/bash -e
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# Redefine these variables to fit the required configuration
SERIAL='180416261221'
MODEL='1.00'
COLOUR='black'
PCB_MB_VERSION='7v1'
PCB_UM_VERSION='5v1'
PCB_LM_VERSION='5v1'
PCB_AM_VERSION='N50-4001-01EB'
BATTERY_REVISION='A'
IMAGE_FILE_NAME='eeprom.bin'

printf "\nTool to generate default EEPROM file for Mudita Pure\n\n"

# Check if file already exists
if [[ -f ${IMAGE_FILE_NAME} ]]; then
  printf "File '%s' already exists, continuing will result in its contents being overwritten!\n" ${IMAGE_FILE_NAME}
  read -p "Do you want to continue? (y/n): " -n 1 -r
  printf "\n"
  if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    printf "Aborting...\n"
    exit 1
  fi

  # Remove previous image
  rm ${IMAGE_FILE_NAME}
fi


# Generate new image
./eepromgen.sh ${IMAGE_FILE_NAME} \
               serial=${SERIAL} \
               model=${MODEL} \
               case_colour=${COLOUR}  \
               pcb_mb_version=${PCB_MB_VERSION} \
               pcb_um_version=${PCB_UM_VERSION} \
               pcb_lm_version=${PCB_LM_VERSION} \
               pcb_am_version=${PCB_AM_VERSION} \
               battery_revision=${BATTERY_REVISION}

printf "\nEEPROM file '%s' has been created!\n" ${IMAGE_FILE_NAME}
