cmake_minimum_required(VERSION 3.13)
add_library(lwext4 STATIC)
add_library(lwext4::lwext4 ALIAS lwext4)

include(lwext4_macros.cmake)

target_include_directories(lwext4
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lwext4/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_compile_definitions(lwext4
    PRIVATE
    CONFIG_DEBUG_PRINTF=1
    CONFIG_DEBUG_ASSERT=0
    CONFIG_HAVE_OWN_OFLAGS=1
    CONFIG_HAVE_OWN_ERRNO=0
    CONFIG_BLOCK_DEV_CACHE_SIZE=16
)

target_sources(lwext4
    PRIVATE
        lwext4/src/ext4_balloc.c
        lwext4/src/ext4_bcache.c
        lwext4/src/ext4_bitmap.c
        lwext4/src/ext4_blockdev.c
        lwext4/src/ext4_block_group.c
        lwext4/src/ext4.c
        lwext4/src/ext4_crc32.c
        lwext4/src/ext4_debug.c
        lwext4/src/ext4_dir.c
        lwext4/src/ext4_dir_idx.c
        lwext4/src/ext4_extent.c
        lwext4/src/ext4_fs.c
        lwext4/src/ext4_hash.c
        lwext4/src/ext4_ialloc.c
        lwext4/src/ext4_inode.c
        lwext4/src/ext4_journal.c
        lwext4/src/ext4_mbr.c
        lwext4/src/ext4_mkfs.c
        lwext4/src/ext4_super.c
        lwext4/src/ext4_trans.c
        lwext4/src/ext4_xattr.c
    PUBLIC
        lwext4/include/ext4_balloc.h
        lwext4/include/ext4_bcache.h
        lwext4/include/ext4_bitmap.h
        lwext4/include/ext4_blockdev.h
        lwext4/include/ext4_block_group.h
        lwext4/include/ext4_config.h
        lwext4/include/ext4_crc32.h
        lwext4/include/ext4_debug.h
        lwext4/include/ext4_dir.h
        lwext4/include/ext4_dir_idx.h
        lwext4/include/ext4_errno.h
        lwext4/include/ext4_extent.h
        lwext4/include/ext4_fs.h
        lwext4/include/ext4.h
        lwext4/include/ext4_hash.h
        lwext4/include/ext4_ialloc.h
        lwext4/include/ext4_inode.h
        lwext4/include/ext4_journal.h
        lwext4/include/ext4_mbr.h
        lwext4/include/ext4_misc.h
        lwext4/include/ext4_mkfs.h
        lwext4/include/ext4_oflags.h
        lwext4/include/ext4_super.h
        lwext4/include/ext4_trans.h
        lwext4/include/ext4_types.h
        lwext4/include/ext4_xattr.h
        lwext4/include/misc/queue.h
        lwext4/include/misc/tree.h
)

lwext4_output_configure()

