/*
 * FreeRTOS+FAT Labs Build 160919 (C) 2016 Real Time Engineers ltd.
 * Authors include James Walmsley, Hein Tibosch and Richard Barry
 *
 *******************************************************************************
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 ***                                                                         ***
 ***                                                                         ***
 ***   FREERTOS+FAT IS STILL IN THE LAB:                                     ***
 ***                                                                         ***
 ***   This product is functional and is already being used in commercial    ***
 ***   products.  Be aware however that we are still refining its design,    ***
 ***   the source code does not yet fully conform to the strict coding and   ***
 ***   style standards mandated by Real Time Engineers ltd., and the         ***
 ***   documentation and testing is not necessarily complete.                ***
 ***                                                                         ***
 ***   PLEASE REPORT EXPERIENCES USING THE SUPPORT RESOURCES FOUND ON THE    ***
 ***   URL: http://www.FreeRTOS.org/contact  Active early adopters may, at   ***
 ***   the sole discretion of Real Time Engineers Ltd., be offered versions  ***
 ***   under a license other than that described below.                      ***
 ***                                                                         ***
 ***                                                                         ***
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 *******************************************************************************
 *
 * FreeRTOS+FAT can be used under two different free open source licenses.  The
 * license that applies is dependent on the processor on which FreeRTOS+FAT is
 * executed, as follows:
 *
 * If FreeRTOS+FAT is executed on one of the processors listed under the Special
 * License Arrangements heading of the FreeRTOS+FAT license information web
 * page, then it can be used under the terms of the FreeRTOS Open Source
 * License.  If FreeRTOS+FAT is used on any other processor, then it can be used
 * under the terms of the GNU General Public License V2.  Links to the relevant
 * licenses follow:
 *
 * The FreeRTOS+FAT License Information Page: http://www.FreeRTOS.org/fat_license
 * The FreeRTOS Open Source License: http://www.FreeRTOS.org/license
 * The GNU General Public License Version 2: http://www.FreeRTOS.org/gpl-2.0.txt
 *
 * FreeRTOS+FAT is distributed in the hope that it will be useful.  You cannot
 * use FreeRTOS+FAT unless you agree that you use the software 'as is'.
 * FreeRTOS+FAT is provided WITHOUT ANY WARRANTY; without even the implied
 * warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. Real Time Engineers Ltd. disclaims all conditions and terms, be they
 * implied, expressed, or statutory.
 *
 * 1 tab == 4 spaces!
 *
 * http://www.FreeRTOS.org
 * http://www.FreeRTOS.org/plus
 * http://www.FreeRTOS.org/labs
 *
 */

 /*
	As of 15/3/2015 all +FAT configuration items changed their prefix,
	e.g. FF_LITTLE_ENDIAN has become ffconfigLITTLE_ENDIAN
	This tempoary header file checks for the presence old configuration items
	and issue a compiler error if any is defined.
*/

#ifdef FF_LITTLE_ENDIAN
	#error FF_LITTLE_ENDIAN was dropped and replaced with 'ffconfigBYTE_ORDER == pdFREERTOS_LITTLE_ENDIAN'
#endif

#ifdef FF_BIG_ENDIAN
	#error FF_BIG_ENDIAN was dropped and replaced with 'ffconfigBYTE_ORDER == pdFREERTOS_BIG_ENDIAN'
#endif

#ifdef ffconfigLITTLE_ENDIAN
	#error ffconfigLITTLE_ENDIAN was dropped.
#endif

#ifdef ffconfigBIG_ENDIAN
	#error ffconfigBIG_ENDIAN was dropped.
#endif

#ifdef FF_HAS_CWD
	#error FF_HAS_CWD still defined. Please use ffconfig prefix.
#endif

#if !defined( pdFREERTOS_LITTLE_ENDIAN ) || !defined( pdFREERTOS_BIG_ENDIAN )
	#error Missing defines from FreeRTOS
#endif

#ifdef FF_LFN_SUPPORT
	#error FF_LFN_SUPPORT still defined. Please use ffconfig prefix.
#endif

#ifdef FF_INCLUDE_SHORT_NAME
	#error FF_INCLUDE_SHORT_NAME still defined. Please use ffconfig prefix.
#endif

#ifdef FF_SHORTNAME_CASE
	#error FF_SHORTNAME_CASE still defined. Please use ffconfig prefix.
#endif

#ifdef FF_UNICODE_UTF16_SUPPORT
	#error FF_UNICODE_UTF16_SUPPORT still defined. Please use ffconfig prefix.
#endif

#ifdef FF_UNICODE_UTF8_SUPPORT
	#error FF_UNICODE_UTF8_SUPPORT still defined. Please use ffconfig prefix.
#endif

#ifdef FF_FAT12_SUPPORT
	#error FF_FAT12_SUPPORT still defined. Please use ffconfig prefix.
#endif

#ifdef FF_OPTIMISE_UNALIGNED_ACCESS
	#error FF_OPTIMISE_UNALIGNED_ACCESS still defined. Please use ffconfig prefix.
#endif

#ifdef FF_CACHE_WRITE_THROUGH
	#error FF_CACHE_WRITE_THROUGH still defined. Please use ffconfig prefix.
#endif

#ifdef FF_WRITE_BOTH_FATS
	#error FF_WRITE_BOTH_FATS still defined. Please use ffconfig prefix.
#endif

#ifdef FF_WRITE_FREE_COUNT
	#error FF_WRITE_FREE_COUNT still defined. Please use ffconfig prefix.
#endif

#ifdef FF_TIME_SUPPORT
	#error FF_TIME_SUPPORT still defined. Please use ffconfig prefix.
#endif

#ifdef FF_REMOVABLE_MEDIA
	#error FF_REMOVABLE_MEDIA still defined. Please use ffconfig prefix.
#endif

#ifdef FF_MOUNT_FIND_FREE
	#error FF_MOUNT_FIND_FREE still defined. Please use ffconfig prefix.
#endif

#ifdef FF_FINDAPI_ALLOW_WILDCARDS
	#error FF_FINDAPI_ALLOW_WILDCARDS still defined. Please use ffconfig prefix.
#endif

#ifdef FF_WILDCARD_CASE_INSENSITIVE
	#error FF_WILDCARD_CASE_INSENSITIVE still defined. Please use ffconfig prefix.
#endif

#ifdef FF_PATH_CACHE
	#error FF_PATH_CACHE still defined. Please use ffconfig prefix.
#endif

#ifdef FF_PATH_CACHE_DEPTH
	#error FF_PATH_CACHE_DEPTH still defined. Please use ffconfig prefix.
#endif

#ifdef FF_HASH_CACHE
	#error FF_HASH_CACHE still defined. Please use ffconfig prefix.
#endif

#ifdef FF_HASH_FUNCTION
	#error FF_HASH_FUNCTION still defined. Please use ffconfig prefix.
#endif

#ifdef FF_HASH_TABLE_SIZE
	#error FF_HASH_TABLE_SIZE still defined. Please use ffconfig prefix.
#endif

#ifdef FF_HASH_TABLE_SIZE
	#error FF_HASH_TABLE_SIZE still defined. Please use ffconfig prefix.
#endif

#ifdef FF_MKDIR_RECURSIVE
	#error FF_MKDIR_RECURSIVE still defined. Please use ffconfig prefix.
#endif

#ifdef FF_BLKDEV_USES_SEM
	#error FF_BLKDEV_USES_SEM is not used any more
#endif

#ifdef ffconfigBLKDEV_USES_SEM
	#error ffconfigBLKDEV_USES_SEM is not used any more
#endif

#ifdef FF_MALLOC
	#error FF_MALLOC still defined. Please use ffconfig prefix.
#endif

#ifdef FF_FREE
	#error FF_FREE still defined. Please use ffconfig prefix.
#endif

#ifdef FF_64_NUM_SUPPORT
	#error FF_64_NUM_SUPPORT still defined. Please use ffconfig prefix.
#endif

#ifdef FF_MAX_PARTITIONS
	#error FF_MAX_PARTITIONS still defined. Please use ffconfig prefix.
#endif

#ifdef FF_MAX_FILE_SYS
	#error FF_MAX_FILE_SYS still defined. Please use ffconfig prefix.
#endif

#ifdef FF_DRIVER_BUSY_SLEEP_MS
	#error FF_DRIVER_BUSY_SLEEP_MS still defined. Please use ffconfig prefix.
#endif

#ifdef FF_FPRINTF_SUPPORT
	#error FF_FPRINTF_SUPPORT still defined. Please use ffconfig prefix.
#endif

#ifdef FF_FPRINTF_BUFFER_LENGTH
	#error FF_FPRINTF_BUFFER_LENGTH still defined. Please use ffconfig prefix.
#endif

#ifdef FF_DEBUG
	#error FF_DEBUG still defined. Please use ffconfig prefix.
#endif

#ifdef FF_HAS_FUNCTION_TAB
	#error FF_HAS_FUNCTION_TAB still defined. Please use ffconfig prefix.
#endif

#ifdef FF_FAT_CHECK
	#error FF_FAT_CHECK still defined. Please use ffconfig prefix.
#endif

#ifdef FF_MAX_FILENAME
	#error FF_MAX_FILENAME still defined. Please use ffconfig prefix.
#endif

#ifdef FF_PRINTFFF_PRINTF
	#error FF_PRINTFFF_PRINTF still defined. Please use ffconfig prefix.
#endif

#ifdef FF_FAT_USES_STAT
	#error FF_FAT_USES_STAT still defined. Please use ffconfig prefix.
#endif

#ifdef BUF_STORE_COUNT
	#error BUF_STORE_COUNT still defined. Please use ffconfig prefix.
#endif

#ifdef FF_USE_NOTIFY
	#error FF_USE_NOTIFY still defined. Please use ffconfig prefix.
#endif

#ifdef FF_DEV_SUPPORT
	#error FF_DEV_SUPPORT still defined. Please use ffconfig prefix.
#endif

#ifdef FF_FSINFO_TRUSTED
	#error FF_FSINFO_TRUSTED still defined. Please use ffconfig prefix.
#endif

#ifdef FF_LONG_ERR_MSG
	#error FF_LONG_ERR_MSG still defined. Please use ffconfig prefix.
#endif

#ifdef FF_INLINE_MEMORY_ACCESS
	#error FF_INLINE_MEMORY_ACCESS still defined. Please use ffconfig prefix.
#endif

#ifdef FF_MIRROR_FATS_UMOUNT
	#error FF_MIRROR_FATS_UMOUNT still defined. Please use ffconfig prefix.
#endif

#ifdef FF_HASH_CACHE_DEPTH
	#error FF_HASH_CACHE_DEPTH still defined. Please use ffconfig prefix.
#endif

#ifdef FF_HASH_TABLE_SUPPORT
	#error FF_HASH_TABLE_SUPPORT was dropped
#endif

#ifdef FF_INLINE_BLOCK_CALCULATIONS
	#error FF_INLINE_BLOCK_CALCULATIONS was dropped
#endif

#ifdef FF_CWD_THREAD_LOCAL_INDEX
	#error FF_CWD_THREAD_LOCAL_INDEX is now called ffconfigCWD_THREAD_LOCAL_INDEX
#endif

#ifdef FF_DEV_PATH
	#error FF_DEV_PATH was dropped
#endif
