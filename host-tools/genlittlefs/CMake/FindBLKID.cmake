# Find the BLKID includes and library
#
#  BLKID_INCLUDE_DIR - where to find fuse.h, etc.
#  BLKID_LIBRARIES   - List of libraries when using FDISK.
#  BLKID_FOUND       - True if FDISK lib is found.

# check if already in cache, be silent
IF (BLKID_INCLUDE_DIR)
    SET (BLKID_FIND_QUIETLY TRUE)
ENDIF (BLKID_INCLUDE_DIR)

# find includes
FIND_PATH (BLKID_INCLUDE_DIR blkid.h
        /usr/local/include/blkid
        /usr/local/include
        /usr/include
        /usr/include/blkid
)

# find lib
if (APPLE)
    SET(BLKID_NAMES libblkid.dylib blkid)
else (APPLE)
    SET(BLKID_NAMES blkid)
endif (APPLE)
FIND_LIBRARY(BLKID_LIBRARIES
        NAMES ${BLKID_NAMES}
        PATHS /lib64 /lib /usr/lib64 /usr/lib /usr/local/lib64 /usr/local/lib /usr/lib/x86_64-linux-gnu /usr/lib/aarch64-linux-gnu
        )

include ("FindPackageHandleStandardArgs")
find_package_handle_standard_args ("BLKID" DEFAULT_MSG
        BLKID_INCLUDE_DIR BLKID_LIBRARIES)

mark_as_advanced (BLKID_INCLUDE_DIR FDISK_LIBRARIES)
