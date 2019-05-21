set(BOARD_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/vfs.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/ff_crc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/ff_dir.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/ff_error.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/ff_fat.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/ff_file.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/ff_format.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/ff_ioman.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/ff_locking.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/ff_memory.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/ff_stdio.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/ff_string.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/ff_sys.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/ff_time.c

        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/portable/ff_eMMC_user_disk.cpp


        CACHE INTERNAL ""
        )

set(BOARD_DIR_INCLUDES

        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/include
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/freeRTOS_FAT/portable

        CACHE INTERNAL "")