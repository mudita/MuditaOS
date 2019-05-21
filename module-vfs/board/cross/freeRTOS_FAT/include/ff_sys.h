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
	ff_sys.h

	This module allow to map several separate file-sub-systems into a root directory

	For instance, a system with 3 sub sytems:

		/flash :  NAND flash driver
		/ram   :  RAM-disk driver
		/      :  SD-card driver

	In this example, the SD-card driver handles ALL files and directories which
	do not match /flash/ * or /ram/ *

	Now for instance a file call "/flash/etc/network.ini"
	will be stored as "/etc/network.ini" on the NAND drive

	This module along with ff_stdio.c make translations between absolute
	and relative paths
*/

#ifndef FF_SYS_H
#define FF_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FILE_SUB_SYSTEM
{
	char pcPath[16];
	BaseType_t xPathlen;
	FF_IOManager_t *pxManager;
} FF_SubSystem_t;

typedef struct FF_DIR_HANDLER
{
	union
	{
		struct
		{
			unsigned
				bEndOfDir : 1,
				bFirstCalled : 1,
				bIsValid : 1,
				bAddDotEntries : 2;
		} bits;
		unsigned uFlags;
	} u;
	/*
	 * path will contain the relative path. It will be used when calling +FAT functions
	 * like FF_FindFirst() / FF_FindNext()
	 * For instance, for "/flash/etc" path will become "/etc"
	 */
	const char *pcPath;
	FF_IOManager_t *pxManager;	/* Will point to handler of this partition. */
	BaseType_t xFSIndex;		/* The index of this entry, where 0 always means: the root system. */
} FF_DirHandler_t;

/*
 * Initialise (clear) the file system table
 * This will also called by FF_FS_Add()
 */
void FF_FS_Init( void );

/*
 * Add a file system
 * The path must be absolute, e.g. start with a slash
 * The second argument is the FF_Disk_t structure that is handling the driver
 */
int FF_FS_Add( const char *pcPath, FF_Disk_t *pxDisk );

/*
 * Remove a file system
 * which ws earlier added by ff_fs_ad()
 */
void FF_FS_Remove( const char *pcPath );

/*
 * Internally used by ff_stdio:
 * The ff_dir_handler helps to iterate through a mounte directory
 *
 * FF_FS_Find() will find a ff_dir_handler for a given path
 */
int FF_FS_Find( const char *pcPath, FF_DirHandler_t *pxHandler );

/*
 * For internal use:
 * Get the file system information, based on an index
 */
int FF_FS_Get( int iIndex, FF_SubSystem_t *pxSystem );

/*
 * Returns the number of registered
 * file systems
 */
int FF_FS_Count( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FF_SYS_H */
