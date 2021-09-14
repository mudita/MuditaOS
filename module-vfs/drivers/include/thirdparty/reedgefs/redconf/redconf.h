// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/** @file
 */

/*  Inherit most settings from the target configuration.
 */
#include "../redconf.h"

#ifndef HOST_REDCONF_H
#define HOST_REDCONF_H

/*  Assuming the host machine is little endian.  If the host machine is actually
    big endian, this can be worked around by changing the below "== 1" to "== 0"
    and setting REDCONF_ENDIAN_BIG to 1 below.
*/
#if REDCONF_ENDIAN_BIG == 1
#define REDCONF_ENDIAN_SWAP
#endif

#undef REDCONF_ENDIAN_BIG
#define REDCONF_ENDIAN_BIG 0

/*  Ignore the target system memory alignment.  For Linux, 4 bytes works well.
 */
#undef REDCONF_ALIGNMENT_SIZE
#define REDCONF_ALIGNMENT_SIZE 4U

/*  Host tools always have output.
 */
#undef REDCONF_OUTPUT
#define REDCONF_OUTPUT 1

/*  Read-only must be disabled for the image builder.
 */
#undef REDCONF_READ_ONLY
#define REDCONF_READ_ONLY 0

/*  Enable the checker host tool.
 */
#undef REDCONF_CHECKER
#define REDCONF_CHECKER 1

/*  Enable the formatter code in POSIX-like API configurations for the image
    builder and formatter host tools.
*/
#undef REDCONF_API_POSIX_FORMAT
#define REDCONF_API_POSIX_FORMAT 1

/*  Enable the image builder host tool.
 */
#undef REDCONF_IMAGE_BUILDER
#define REDCONF_IMAGE_BUILDER 1

/*  The image builder needs red_mkdir().
 */
#undef REDCONF_API_POSIX_MKDIR
#define REDCONF_API_POSIX_MKDIR 1

/*  The image copier utility needs red_readdir().
 */
#undef REDCONF_API_POSIX_READDIR
#define REDCONF_API_POSIX_READDIR 1

/*  The image copier utility needs a handle for every level of directory depth.
    While Reliance Edge has no maximum directory depth or path depth, Windows
    limits paths to 260 bytes, and each level of depth eats up at least two
    characters, 130 handles will be sufficient for all images that can be
    copied.
*/
#undef REDCONF_HANDLE_COUNT
#define REDCONF_HANDLE_COUNT 130U

/*  Use the fastest CRC algorithm for the tools.  Slice-by-8 will work well on
    a host machine and the extra code space is a nonissue.
*/
#undef REDCONF_CRC_ALGORITHM
#define REDCONF_CRC_ALGORITHM CRC_SLICEBY8

/*  The target redconf.h may have configured the memory and string functions to
    use custom implementations that are only available on the target system.  So
    for the host, we just use the C library versions.
*/
#include <string.h>

#undef RedMemCpyUnchecked
#define RedMemCpyUnchecked memcpy
#undef RedMemMoveUnchecked
#define RedMemMoveUnchecked memmove
#undef RedMemSetUnchecked
#define RedMemSetUnchecked memset
#undef RedMemCmpUnchecked
#define RedMemCmpUnchecked memcmp

#undef RedStrLenUnchecked
#define RedStrLenUnchecked strlen
#undef RedStrCmpUnchecked
#define RedStrCmpUnchecked strcmp
#undef RedStrNCmpUnchecked
#define RedStrNCmpUnchecked strncmp
#undef RedStrNCpyUnchecked
#define RedStrNCpyUnchecked strncpy

/*  Avoid extra transactions to improve image builder performance.
 */
#undef REDCONF_TRANSACT_DEFAULT
#define REDCONF_TRANSACT_DEFAULT                                                                                       \
    ((RED_TRANSACT_FSYNC | RED_TRANSACT_VOLFULL | RED_TRANSACT_UMOUNT | RED_TRANSACT_SYNC) & RED_TRANSACT_MASK)

#endif
