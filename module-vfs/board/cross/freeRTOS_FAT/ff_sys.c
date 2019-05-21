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

#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portable.h"

#include "ff_headers.h"
#include "ff_sys.h"

#ifndef ARRAY_SIZE
#	define	ARRAY_SIZE(x)	(int)(sizeof(x)/sizeof(x)[0])
#endif

/*
 * Define a collection of 'file systems' as a simple array
 */
typedef struct xSYSTEM
{
	FF_SubSystem_t systems[ ffconfigMAX_FILE_SYS ];
	int fsCount;
} ff_sys_t;


static ff_sys_t file_systems;
static const char rootDir[] = "/";

int FF_FS_Count( void )
{
	return file_systems.fsCount;
}
/*-----------------------------------------------------------*/

void FF_FS_Init( void )
{
	memset( &file_systems, '\0', sizeof( file_systems ) );

	/* There is always a root file system, even if it doesn't have a
	IO manager. */
	file_systems.fsCount = 1;
	/* Can safely use strcpy because 'rootDir' has a fixed length of 1 byte. */
	strcpy( file_systems.systems[0].pcPath, rootDir );
	file_systems.systems[0].xPathlen = 1;
}
/*-----------------------------------------------------------*/

int FF_FS_Add( const char *pcPath, FF_Disk_t *pxDisk )
{
	int ret = 0;

	configASSERT( pxDisk );

	if (*pcPath != '/')
	{
		FF_PRINTF( "FF_FS_Add: Need a \"/\": '%s'\n", pcPath );
	}
	else
	{
		int index = -1;
		int len = ( int ) strlen (pcPath);

		if( file_systems.fsCount == 0 )
		{
			FF_FS_Init();
		}

		if( len == 1 )
		{
			/* This is the "/" path
			 * and will be put at index 0 */
			index = 0;
		}
		else
		{
			int i;
			FF_SubSystem_t *pxSubSystem = file_systems.systems + 1;	/* Skip the root entry */
			for( i = 1; i < file_systems.fsCount; i++, pxSubSystem++ )
			{
				if( ( pxSubSystem->xPathlen == len ) &&
					( memcmp( pxSubSystem->pcPath, pcPath, ( size_t )len ) == 0 ) )
				{
					index = i;	/* A system is updated with a new handler. */
					break;
				}
			}
		}
		if( ( index < 0 ) && ( file_systems.fsCount >= ARRAY_SIZE( file_systems.systems ) ) )
		{
			FF_PRINTF( "FF_FS_Add: Table full '%s' (max = %d)\n", pcPath, (int)ARRAY_SIZE( file_systems.systems ) );
		}
		else
		{
			vTaskSuspendAll();
			{
				if( index < 0 )
				{
					index = file_systems.fsCount++;
				}

				strncpy( file_systems.systems[ index ].pcPath, pcPath, sizeof( file_systems.systems[ index ].pcPath ) );
				file_systems.systems[ index ].xPathlen = len;
				file_systems.systems[ index ].pxManager = pxDisk->pxIOManager;
			}
			xTaskResumeAll( );
			ret = 1;
		}
	}

	return ret;
}
/*-----------------------------------------------------------*/

void FF_FS_Remove( const char *pcPath )
{
int index;
int len;
int i;

	if( pcPath[0] == '/' )
	{
		index = -1;
		len = ( int ) strlen( pcPath );
		/* Is it the "/" path ? */
		if (len == 1)
		{
			index = 0;
		}
		else
		{
			FF_SubSystem_t *pxSubSystem = file_systems.systems + 1;
			for( i = 1; i < file_systems.fsCount; i++, pxSubSystem++ )
			{
				if( ( pxSubSystem->xPathlen == len ) &&
					( memcmp( pxSubSystem->pcPath, pcPath, ( size_t ) len ) == 0 ) )
				{
					index = i;
					break;
				}
			}
		}
		if( index >= 0 )
		{
			file_systems.systems[ index ].pxManager = NULL;
		}
	}
}
/*-----------------------------------------------------------*/

int FF_FS_Find( const char *pcPath, FF_DirHandler_t *pxHandler )
{
FF_SubSystem_t *pxSubSystem;
int len;
int index;
int iReturn;

	pxSubSystem = file_systems.systems + 1;
	len = ( int ) strlen( pcPath );

	memset( pxHandler, '\0', sizeof( *pxHandler ) );
	for( index = 1; index < file_systems.fsCount; index++, pxSubSystem++ )
	{
		if( ( len >= pxSubSystem->xPathlen ) &&
			( memcmp( pxSubSystem->pcPath, pcPath, ( size_t ) pxSubSystem->xPathlen ) == 0 ) &&
			( ( pcPath[pxSubSystem->xPathlen] == '\0' ) || ( pcPath[pxSubSystem->xPathlen] == '/') ) )	/* System "/ram" should not match with "/ramc/etc". */
		{
			if( pcPath[pxSubSystem->xPathlen] == '\0')
			{
				pxHandler->pcPath = rootDir;
			}
			else
			{
				pxHandler->pcPath = pcPath + pxSubSystem->xPathlen;
			}

			pxHandler->pxManager = pxSubSystem->pxManager;
			break;
		}
	}

	if( index == file_systems.fsCount )
	{
		pxHandler->pcPath = pcPath;
		pxHandler->pxManager = file_systems.systems[0].pxManager;
	}

	if( FF_Mounted( pxHandler->pxManager ) )
	{
		iReturn = pdTRUE;
	}
	else
	{
		iReturn = pdFALSE;
	}

	return iReturn;
}
/*-----------------------------------------------------------*/

int FF_FS_Get( int iIndex, FF_SubSystem_t *pxSystem )
{
int iReturn;

	/* Get a copy of a fs info. */
	if( ( iIndex < 0 ) || ( iIndex >= file_systems.fsCount ) )
	{
		iReturn = 0;
	}
	else
	{
		*pxSystem = file_systems.systems[ iIndex ];
		iReturn = 1;
	}

	return iReturn;
}
/*-----------------------------------------------------------*/

