// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "xing_header.h"
#include <string.h>

/* XING VBR-Header

 size   description
 4      'Xing' or 'Info'
 4      flags (indicates which fields are used)
 4      frames (optional)
 4      bytes (optional)
 100    toc (optional)
 4      a VBR quality indicator: 0=best 100=worst (optional)

*/

typedef struct
{
    char desc[4];
    uint32_t flags;
} xing_header_t;

// for XING VBR Header flags
#define FRAMES_FLAG    0x0001
#define BYTES_FLAG     0x0002
#define TOC_FLAG       0x0004
#define VBR_SCALE_FLAG 0x0008

//! Byte swap unsigned int
static inline uint32_t swap32(uint32_t ui)
{
    return (ui >> 24) | ((ui << 8) & 0x00FF0000) | ((ui >> 8) & 0x0000FF00) | (ui << 24);
}

uint8_t parseXingHeader(uint8_t *data, size_t datasize, xing_info_t *info)
{
    char *p1 = memmem(data, datasize, "Xing", 4);
    char *p2 = memmem(data, datasize, "Info", 4);

    if (p1 != NULL || p2 != NULL) {
        xing_header_t *head = (xing_header_t *)(p1 != NULL ? p1 : p2);
        uint8_t *raw_data   = (uint8_t *)head + sizeof(xing_header_t);

        head->flags = swap32(head->flags);

        uint8_t offset = 0;
        // get flags (mandatory in XING header)

        // extract total number of frames in file
        if (head->flags & FRAMES_FLAG) {
            uint32_t *tmp     = (uint32_t *)&raw_data[offset];
            info->TotalFrames = swap32(*tmp);
            offset += 4;
        }
        else {
            return 0;
        }

        // extract total number of bytes in file
        if (head->flags & BYTES_FLAG) {
            uint32_t *tmp    = (uint32_t *)&raw_data[offset];
            info->TotalBytes = swap32(*tmp);
            offset += 4;
        }

        // extract TOC
        if (head->flags & TOC_FLAG) {
            memcpy(info->TOC, &raw_data[offset], sizeof(info->TOC));
            offset += sizeof(info->TOC);
        }

        // extract quality status
        if (head->flags & VBR_SCALE_FLAG) {
            info->Quality = raw_data[offset + 3];
        }

        return 1;
    }
    else {
        return 0;
    }
}
