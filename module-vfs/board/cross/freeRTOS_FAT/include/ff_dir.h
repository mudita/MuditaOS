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

/**
 *	@file		ff_dir.h
 *	@ingroup	DIR
 **/
#ifndef _FF_DIR_H_

#define _FF_DIR_H_

#ifndef PLUS_FAT_H
	#error this header will be included from "plusfat.h"
#endif

#define FIND_FLAG_SHORTNAME_SET		0x01u
#define FIND_FLAG_SHORTNAME_CHECKED	0x02u
#define FIND_FLAG_SHORTNAME_FOUND	0x04u
#define FIND_FLAG_FITS_SHORT		0x08u
#define FIND_FLAG_SIZE_OK			0x10u
#define FIND_FLAG_CREATE_FLAG		0x20u

#define FIND_FLAG_FITS_SHORT_OK		( FIND_FLAG_FITS_SHORT | FIND_FLAG_SIZE_OK )

typedef struct
{
	uint32_t ulChainLength;
	uint32_t ulDirCluster;
	uint32_t ulCurrentClusterLCN;
	uint32_t ulCurrentClusterNum;
	FF_Buffer_t *pxBuffer;
} FF_FetchContext_t;

typedef struct
{
	uint32_t ulFileSize;
	uint32_t ulObjectCluster;

	/* Book Keeping. */
	uint32_t ulCurrentCluster;
	uint32_t ulAddrCurrentCluster;
	uint32_t ulDirCluster;
	uint16_t usCurrentItem;
	/* End Book Keeping. */

#if( ffconfigTIME_SUPPORT != 0 )
	FF_SystemTime_t xCreateTime;		/* Date and Time Created. */
	FF_SystemTime_t xModifiedTime;	/* Date and Time Modified. */
	FF_SystemTime_t xAccessedTime;	/* Date of Last Access. */
#endif

#if( ffconfigFINDAPI_ALLOW_WILDCARDS != 0 )
	#if( ffconfigUNICODE_UTF16_SUPPORT != 0 )
		FF_T_WCHAR pcWildCard[ ffconfigMAX_FILENAME ];
	#else
		char pcWildCard[ ffconfigMAX_FILENAME ];
	#endif
	BaseType_t xInvertWildCard;
#endif

#if( ffconfigUNICODE_UTF16_SUPPORT != 0 )
	FF_T_WCHAR pcFileName[ ffconfigMAX_FILENAME ];
#else
	char pcFileName[ ffconfigMAX_FILENAME ];
#endif

#if( ffconfigLFN_SUPPORT != 0 ) && ( ffconfigINCLUDE_SHORT_NAME != 0 )
	char pcShortName[ 13 ];
#endif
	uint8_t	ucAttrib;
#if( ffconfigDEV_SUPPORT != 0 )
	uint8_t	ucIsDeviceDir;
#endif
	FF_FetchContext_t xFetchContext;
} FF_DirEnt_t;



/*
 * Some public API's, i.e. they're used but ff_stdio.c
 */
#if( ffconfigUNICODE_UTF16_SUPPORT != 0 )
	FF_Error_t FF_FindFirst( FF_IOManager_t *pxIOManager, FF_DirEnt_t *pxDirent, const FF_T_WCHAR *pcPath );
	FF_Error_t FF_MkDir( FF_IOManager_t *pxIOManager, const FF_T_WCHAR *pcPath );
#else
	FF_Error_t FF_FindFirst( FF_IOManager_t *pxIOManager, FF_DirEnt_t *pxDirent, const char *pcPath );
	FF_Error_t FF_MkDir( FF_IOManager_t *pxIOManager, const char *pcPath );
#endif

FF_Error_t FF_FindNext( FF_IOManager_t *pxIOManager, FF_DirEnt_t *pxDirent );

static portINLINE void FF_RewindFind( FF_DirEnt_t *pxDirent )
{
	pxDirent->usCurrentItem = 0;
}

/*
 * Some API's internal to the +FAT library.
 */
FF_Error_t FF_GetEntry( FF_IOManager_t *pxIOManager, uint16_t usEntry, uint32_t ulDirCluster, FF_DirEnt_t *pxDirent );
FF_Error_t FF_PutEntry( FF_IOManager_t *pxIOManager, uint16_t usEntry, uint32_t ulDirCluster, FF_DirEnt_t *pxDirent, uint8_t *pucContents );
int8_t FF_FindEntry( FF_IOManager_t *pxIOManager, uint32_t ulDirCluster, int8_t *Name, FF_DirEnt_t *pxDirent, BaseType_t LFNs );

void FF_PopulateShortDirent( FF_IOManager_t *pxIOManager, FF_DirEnt_t *pxDirent, const uint8_t *pucEntryBuffer );
FF_Error_t FF_PopulateLongDirent( FF_IOManager_t *pxIOManager, FF_DirEnt_t *pxDirent, uint16_t usEntry, FF_FetchContext_t *pFetchContext );

FF_Error_t FF_InitEntryFetch( FF_IOManager_t *pxIOManager, uint32_t ulDirCluster, FF_FetchContext_t *pContext );
FF_Error_t FF_FetchEntryWithContext( FF_IOManager_t *pxIOManager, uint32_t ulEntry, FF_FetchContext_t *pContext, uint8_t *pEntryBuffer );
FF_Error_t FF_PushEntryWithContext( FF_IOManager_t *pxIOManager, uint32_t ulEntry, FF_FetchContext_t *pContext, uint8_t *pEntryBuffer );
FF_Error_t FF_CleanupEntryFetch( FF_IOManager_t *pxIOManager, FF_FetchContext_t *pContext );

int8_t FF_PushEntry( FF_IOManager_t *pxIOManager, uint32_t ulDirCluster, uint16_t usEntry, uint8_t *buffer, void *pParam );

static portINLINE BaseType_t FF_isEndOfDir( const uint8_t *pucEntryBuffer )
{
	return pucEntryBuffer[ 0 ] == ( uint8_t ) 0;
}

static portINLINE BaseType_t FF_isDeleted( const uint8_t *pucEntryBuffer )
{
	return pucEntryBuffer[ 0 ] == ( uint8_t ) FF_FAT_DELETED;
}

struct _FF_FIND_PARAMS
{
	uint32_t ulDirCluster;		/* The beginning cluster of this directory. */
	int32_t	lFreeEntry;			/* The first free entry big enough to add the file. */
	uint32_t ulFlags;			/* See FIND_FLAG_xxx defines above. */
	char pcEntryBuffer[ 32 ];	/* LFN converted to short name. */
	uint8_t ucCaseAttrib;
	uint8_t ucFirstTilde;
};

typedef struct _FF_FIND_PARAMS FF_FindParams_t;

#if( ffconfigUNICODE_UTF16_SUPPORT != 0 )
	uint32_t FF_CreateFile( FF_IOManager_t *pxIOManager, FF_FindParams_t *findParams, FF_T_WCHAR *FileName,
		FF_DirEnt_t *pxDirent, FF_Error_t *pError );

	uint32_t FF_FindEntryInDir( FF_IOManager_t *pxIOManager, FF_FindParams_t *findParams, const FF_T_WCHAR *name,
		uint8_t pa_Attrib, FF_DirEnt_t *pxDirent, FF_Error_t *pError );

	uint32_t FF_FindDir( FF_IOManager_t *pxIOManager, const FF_T_WCHAR *pcPath, uint16_t pathLen, FF_Error_t *pError );
	void FF_CreateShortName( FF_FindParams_t *pxFindParams, const FF_T_WCHAR *pcLongName );
#else
	uint32_t FF_CreateFile( FF_IOManager_t *pxIOManager, FF_FindParams_t *findParams, char *FileName,
		FF_DirEnt_t *pxDirent, FF_Error_t *pError );

	uint32_t FF_FindEntryInDir( FF_IOManager_t *pxIOManager, FF_FindParams_t *findParams, const char *name,
		uint8_t pa_Attrib, FF_DirEnt_t *pxDirent, FF_Error_t *pError );

	uint32_t FF_FindDir( FF_IOManager_t *pxIOManager, const char *pcPath, uint16_t pathLen, FF_Error_t *pError );
	void FF_CreateShortName( FF_FindParams_t *pxFindParams, const char *pcLongName );
#endif

int32_t FF_FindShortName( FF_IOManager_t *pxIOManager, FF_FindParams_t *findParams );

FF_Error_t FF_CreateDirent( FF_IOManager_t *pxIOManager, FF_FindParams_t *findParams, FF_DirEnt_t *pxDirent );
FF_Error_t FF_ExtendDirectory( FF_IOManager_t *pxIOManager, uint32_t ulDirCluster );
FF_Error_t FF_RmLFNs( FF_IOManager_t *pxIOManager, uint16_t usDirEntry, FF_FetchContext_t *pContext );

#if( ffconfigHASH_CACHE != 0 )
	BaseType_t FF_CheckDirentHash( FF_IOManager_t *pxIOManager, uint32_t ulDirCluster, uint32_t ulHash );
	BaseType_t FF_DirHashed( FF_IOManager_t *pxIOManager, uint32_t ulDirCluster );
	void FF_AddDirentHash( FF_IOManager_t *pxIOManager, uint32_t ulDirCluster, uint32_t ulHash );
	FF_Error_t FF_HashDir( FF_IOManager_t *pxIOManager, uint32_t ulDirCluster );
	void FF_UnHashDir( FF_IOManager_t *pxIOManager, uint32_t ulDirCluster );
#endif

struct SBuffStats {
	unsigned sectorMatch;
	unsigned sectorMiss;
	unsigned bufCounts;
	unsigned bufCalls;
};

extern struct SBuffStats buffStats;

#endif



