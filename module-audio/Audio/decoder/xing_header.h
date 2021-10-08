// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <stdint.h>
#include <stddef.h>

#define XING_HEADER_SIZE 120

typedef struct
{
    uint32_t TotalFrames;
    uint32_t TotalBytes;
    uint8_t TOC[100];
    uint32_t Quality;
} xing_info_t;

/**
 * @brief This function is used to parse Xing header in MP3 file
 * @param data [in] Pointer to data buffer which contains data to be parsed
 * @param datalen [in] Size of data buffer
 * @param info [out] Pointer to structure describing xing header
 * @return 1 if success, otherwise 0.
 */
uint8_t parseXingHeader(uint8_t *data, size_t datasize, xing_info_t *info);

