# Find the FDISK includes and library
#
#  FDISK_INCLUDE_DIR - where to find fuse.h, etc.
#  FDISK_LIBRARIES   - List of libraries when using FDISK.
#  FDISK_FOUND       - True if FDISK lib is found.

# check if already in cache, be silent
IF (FDISK_INCLUDE_DIR)
    SET (FDISK_FIND_QUIETLY TRUE)
ENDIF (FDISK_INCLUDE_DIR)

# find includes
FIND_PATH (FDISK_INCLUDE_DIR libfdisk.h
        /usr/local/include/libfdisk
        /usr/local/include
        /usr/include
        /usr/include/libfdisk
        )

# find lib
if (APPLE)
    SET(FDISK_NAMES libfdisk.dylib fdisk)
else (APPLE)
    SET(FDISK_NAMES fdisk)
endif (APPLE)
FIND_LIBRARY(FDISK_LIBRARIES
        NAMES ${FDISK_NAMES}
        PATHS /lib64 /lib /usr/lib64 /usr/lib /usr/local/lib64 /usr/local/lib /usr/lib/x86_64-linux-gnu
        )

include ("FindPackageHandleStandardArgs")
find_package_handle_standard_args ("FDISK" DEFAULT_MSG
        FDISK_INCLUDE_DIR FDISK_LIBRARIES)

mark_as_advanced (FDISK_INCLUDE_DIR FDISK_LIBRARIES)
