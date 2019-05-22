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
 *	@file		ff_string.c
 *	@ingroup	STRING
 *
 *	@defgroup	STRING	FreeRTOS+FAT String Library
 *	@brief		Portable String Library for FreeRTOS+FAT
 *
 *
 **/

#ifndef _FF_STRING_H_
#define _FF_STRING_H_

#include "FreeRTOSFATConfig.h"
#include <string.h>

#if( ffconfigUNICODE_UTF16_SUPPORT != 0 )
#include <wchar.h>
typedef wchar_t			FF_T_WCHAR;		/*/< Unicode UTF-16 Character type, for FreeRTOS+FAT when UNICODE is enabled. */
#endif

#if defined( _MSC_VER ) && ( _MSC_VER <= 1600 )
	#define FF_stricmp	_stricmp
#else
	#define FF_stricmp	strcasecmp
#endif

#if( ffconfigUNICODE_UTF16_SUPPORT != 0 )
	void			FF_tolower		( FF_T_WCHAR *string, uint32_t strLen );
	void			FF_toupper		( FF_T_WCHAR *string, uint32_t strLen );
	BaseType_t		FF_strmatch		( const FF_T_WCHAR *str1, const FF_T_WCHAR *str2, BaseType_t len );
	FF_T_WCHAR		*FF_strtok		( const FF_T_WCHAR *string, FF_T_WCHAR *token, uint16_t *tokenNumber, BaseType_t *last, BaseType_t xLength );
	BaseType_t		FF_wildcompare	( const FF_T_WCHAR *pcWildCard, const FF_T_WCHAR *pszString );

	/* ASCII to UTF16 and UTF16 to ASCII routines. -- These are lossy routines, and are only for converting ASCII to UTF-16 */
	/* and the equivalent back to ASCII. Do not use them for international text. */
	void FF_cstrtowcs(FF_T_WCHAR *wcsDest, const char *szpSource);
	void FF_wcstocstr(char *szpDest, const FF_T_WCHAR *wcsSource);
	void FF_cstrntowcs(FF_T_WCHAR *wcsDest, const char *szpSource, uint32_t len);
	void FF_wcsntocstr(char *szpDest, const FF_T_WCHAR *wcsSource, uint32_t len);
#else
	void			FF_tolower		( char *string, uint32_t strLen );
	void			FF_toupper		( char *string, uint32_t strLen );
	BaseType_t		FF_strmatch		( const char *str1, const char *str2, BaseType_t len );
	char			*FF_strtok		( const char *string, char *token, uint16_t *tokenNumber, BaseType_t *last, BaseType_t xLength );
	BaseType_t		FF_wildcompare	( const char *pcWildCard, const char *pszString );
#endif /* ffconfigUNICODE_UTF16_SUPPORT */

/* UTF8 / UTF16 Transformation Functions. */

#if ( ( ffconfigUNICODE_UTF16_SUPPORT != 0 ) && ( WCHAR_MAX > 0xFFFF ) ) || ( ffconfigUNICODE_UTF8_SUPPORT != 0 )
	UBaseType_t FF_GetUtf16SequenceLen	(uint16_t usLeadChar);
#endif

#if( ffconfigUNICODE_UTF8_SUPPORT != 0 )
	int32_t FF_Utf8ctoUtf16c		(uint16_t *utf16Dest, const uint8_t *utf8Source, uint32_t ulSize);
	int32_t FF_Utf16ctoUtf8c		(uint8_t *utf8Dest, const uint16_t *utf16Source, uint32_t ulSize);
#endif	/* ffconfigUNICODE_UTF8_SUPPORT */

/* UTF16 / UTF32 Transformation Functions. */

#if( ffconfigNOT_USED_FOR_NOW != 0 )
	int32_t FF_Utf16ctoUtf32c(uint32_t *utf32Dest, const uint16_t *utf16Source);
#endif

#if( ffconfigUNICODE_UTF16_SUPPORT != 0 ) && ( WCHAR_MAX > 0xFFFF )
	int32_t FF_Utf32ctoUtf16c(uint16_t *utf16Dest, uint32_t utf32char, uint32_t ulSize);
#endif

/* String transformations. */
int32_t FF_Utf32stoUtf8s(uint8_t *Utf8String, uint32_t *Utf32String);

#if( ffconfigUNICODE_UTF16_SUPPORT != 0 )
	#define STRNCPY( target, src, maxlen )	wcsncpy( ( target ), ( src ), ( maxlen ) )
	#define STRLEN( string )				wcslen( ( string ) )
#else
	#define STRNCPY( target, src, maxlen )	strncpy( ( target ), ( src ), ( maxlen ) );
	#define STRLEN( string )				strlen( ( string ) )
#endif

#endif

