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

#ifndef _FF_FATDEF_H_
#define _FF_FATDEF_H_

/*
	This file defines offsets to various data for the FAT specification.
*/

/* MBR / PBR Offsets. */

#define FF_FAT_BYTES_PER_SECTOR		0x00B
#define FF_FAT_SECTORS_PER_CLUS		0x00D
#define FF_FAT_RESERVED_SECTORS		0x00E
#define FF_FAT_NUMBER_OF_FATS		0x010
#define FF_FAT_ROOT_ENTRY_COUNT		0x011
#define FF_FAT_16_TOTAL_SECTORS		0x013
#define	FF_FAT_MEDIA_TYPE           0x015

#define FF_FAT_32_TOTAL_SECTORS		0x020
#define FF_FAT_16_SECTORS_PER_FAT	0x016
#define FF_FAT_32_SECTORS_PER_FAT	0x024
#define FF_FAT_ROOT_DIR_CLUSTER		0x02C
#define FF_FAT_EXT_BOOT_SIGNATURE	0x042

#define FF_FAT_16_VOL_LABEL			0x02B
#define FF_FAT_32_VOL_LABEL			0x047

#define FF_FAT_PTBL					0x1BE
#define FF_FAT_PTBL_LBA				0x008
#define FF_FAT_PTBL_SECT_COUNT		0x00C
#define FF_FAT_PTBL_ACTIVE          0x000
#define FF_FAT_PTBL_ID              0x004

#define FF_DOS_EXT_PART             0x05
#define FF_LINUX_EXT_PART           0x85
#define FF_WIN98_EXT_PART           0x0f

#define FF_FAT_MBR_SIGNATURE        0x1FE

#define FF_FAT_DELETED				0xE5

/* Directory Entry Offsets. */
#define FF_FAT_DIRENT_SHORTNAME		0x000
#define FF_FAT_DIRENT_ATTRIB		0x00B
#define FF_FAT_DIRENT_CREATE_TIME	0x00E	/* Creation Time. */
#define FF_FAT_DIRENT_CREATE_DATE	0x010	/* Creation Date. */
#define FF_FAT_DIRENT_LASTACC_DATE	0x012	/* Date of Last Access. */
#define FF_FAT_DIRENT_CLUS_HIGH		0x014
#define FF_FAT_DIRENT_LASTMOD_TIME	0x016	/* Time of Last modification. */
#define FF_FAT_DIRENT_LASTMOD_DATE	0x018	/* Date of Last modification. */
#define FF_FAT_DIRENT_CLUS_LOW		0x01A
#define FF_FAT_DIRENT_FILESIZE		0x01C
#define FF_FAT_LFN_ORD				0x000
#define FF_FAT_LFN_NAME_1			0x001
#define	FF_FAT_LFN_CHECKSUM			0x00D
#define FF_FAT_LFN_NAME_2			0x00E
#define FF_FAT_LFN_NAME_3			0x01C

/* Dirent Attributes. */
#define FF_FAT_ATTR_READONLY		0x01
#define FF_FAT_ATTR_HIDDEN			0x02
#define FF_FAT_ATTR_SYSTEM			0x04
#define FF_FAT_ATTR_VOLID			0x08
#define FF_FAT_ATTR_DIR				0x10
#define FF_FAT_ATTR_ARCHIVE			0x20
#define FF_FAT_ATTR_LFN				0x0F

/**
 * -- Hein_Tibosch additions for mixed case in shortnames --
 *
 * Specifically, bit 4 means lowercase extension and bit 3 lowercase basename,
 * which allows for combinations such as "example.TXT" or "HELLO.txt" but not "Mixed.txt"
 */

#define FF_FAT_CASE_OFFS			0x0C	/* After NT/XP : 2 case bits. */
#define FF_FAT_CASE_ATTR_BASE		0x08
#define FF_FAT_CASE_ATTR_EXT		0x10

#if( ffconfigLFN_SUPPORT != 0 ) && ( ffconfigINCLUDE_SHORT_NAME != 0 )
#define FF_FAT_ATTR_IS_LFN			0x40	/* artificial attribute, for debugging only. */
#endif

#endif

