/*****< basetypes.h >**********************************************************/
/*      Copyright 2005 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  BASETYPES - Platform Specific Defined Types.                              */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   12/28/05  D. Lange       Initial creation.                               */
/******************************************************************************/
#ifndef __BTTYPESH_INC__
#error "BaseTypes.h should never be used directly.  Include BTTypes.h, instead."
#endif

#ifndef __BASETYPESH__
#define __BASETYPESH__

   /* Miscellaneous defined type declarations.                          */

   /* Simply BOOLEAN type.                                              */
typedef char Boolean_t;

   /* Miscellaneous Type definitions that should already be defined,    */
   /* but are necessary.                                                */
#ifndef NULL
   #define NULL ((void *)0)
#endif

#ifndef TRUE
   #define TRUE (1 == 1)
#endif

#ifndef FALSE
   #define FALSE (0 == 1)
#endif

   /* Unsigned basic types.                                             */
typedef unsigned char Byte_t;                   /* Generic 8 bit Container.   */

#define BYTE_SIZE                               (sizeof(Byte_t))

typedef unsigned short Word_t;                  /* Generic 16 bit Container.  */

#define WORD_SIZE                               (sizeof(Word_t))

typedef unsigned long DWord_t;                  /* Generic 32 bit Container.  */

#define DWORD_SIZE                              (sizeof(DWord_t))

typedef unsigned long long QWord_t;             /* Generic 64 bit Container.  */

#define QWORD_SIZE                              (sizeof(QWord_t))

   /* Signed basic types.                                               */
typedef signed char SByte_t;                    /* Signed 8 bit Container.    */

#define SBYTE_SIZE                              (sizeof(SByte_t))

typedef signed short SWord_t;                   /* Signed 16 bit Container.   */

#define SWORD_SIZE                              (sizeof(SWord_t))

typedef signed long SDWord_t;                   /* Signed 32 bit Container.   */

#define SDWORD_SIZE                             (sizeof(SDWord_t))

typedef signed long long SQWord_t;              /* Signed 64 bit Container.   */

#define SQWORD_SIZE                             (sizeof(SQWord_t))

   /* Unaligned Unsigned basic types.                                   */
typedef unsigned char NonAlignedByte_t;         /* Unaligned Generic 8 Bit    */
                                                /* Container.                 */

#define NON_ALIGNED_BYTE_SIZE                   (sizeof(NonAlignedByte_t))

/*************  ECDH  ************************/
#define RCT_PLATFORM_LITTLE 1
#define RCT_PLATFORM_BIG    0

#define	RCT_WORDSIZE		32
#define	RCE_WORDSIZE		32
#define	RCT_OCTETSIZE		8
#define RCT_OCTETSPERWORD	(RCT_WORDSIZE/RCT_OCTETSIZE)

typedef Byte_t				rct_octet;
typedef DWord_t				rct_word;
typedef SDWord_t			rct_index;
typedef Word_t				rct_short;
typedef QWord_t				rct_dword;
typedef QWord_t				rct_dint;
typedef	QWord_t				rce_dint;
typedef Byte_t				rce_octet;
typedef DWord_t				rce_word;
typedef SDWord_t			rce_index;
typedef void		    	rct_context;

/**********************************************/

typedef __PACKED_STRUCT_BEGIN__ struct _tagNonAlignedWord_t    /* Unaligned Generic 16 Bit   */
{                                                              /* Container.                 */
   Byte_t NonAlignedWord0;
   Byte_t NonAlignedWord1;
} __PACKED_STRUCT_END__ NonAlignedWord_t;

#define NON_ALIGNED_WORD_SIZE                   (sizeof(NonAlignedWord_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagNonAlignedDWord_t   /* Unaligned Generic 32 Bit   */
{                                                              /* Container.                 */
   Byte_t NonAlignedDWord0;
   Byte_t NonAlignedDWord1;
   Byte_t NonAlignedDWord2;
   Byte_t NonAlignedDWord3;
} __PACKED_STRUCT_END__ NonAlignedDWord_t;

#define NON_ALIGNED_DWORD_SIZE                  (sizeof(NonAlignedDWord_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagNonAlignedQWord_t   /* Unaligned Generic 64 Bit   */
{                                                              /* Container.                 */
   Byte_t NonAlignedQWord0;
   Byte_t NonAlignedQWord1;
   Byte_t NonAlignedQWord2;
   Byte_t NonAlignedQWord3;
   Byte_t NonAlignedQWord4;
   Byte_t NonAlignedQWord5;
   Byte_t NonAlignedQWord6;
   Byte_t NonAlignedQWord7;
} __PACKED_STRUCT_END__ NonAlignedQWord_t;

#define NON_ALIGNED_QWORD_SIZE                  (sizeof(NonAlignedQWord_t))

   /* Unaligned Signed basic types.                                     */
typedef signed char NonAlignedSByte_t;          /* Unaligned Signed 8 bit     */
                                                /* Container.                 */

#define NON_ALIGNED_SBYTE_SIZE                  (sizeof(NonAlignedSByte_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagNonAlignedSWord_t   /* Unaligned Signed 16 Bit    */
{                                                              /* Container.                 */
   Byte_t NonAlignedSWord0;
   Byte_t NonAlignedSWord1;
} __PACKED_STRUCT_END__ NonAlignedSWord_t;

#define NON_ALIGNED_SWORD_SIZE                  (sizeof(NonAlignedSWord_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagNonAlignedSDWord_t  /* Unaligned Signed 32 Bit    */
{                                                              /* Container.                 */
   Byte_t NonAlignedSDWord0;
   Byte_t NonAlignedSDWord1;
   Byte_t NonAlignedSDWord2;
   Byte_t NonAlignedSDWord3;
} __PACKED_STRUCT_END__ NonAlignedSDWord_t;

#define NON_ALIGNED_SDWORD_SIZE                 (sizeof(NonAlignedSDWord_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagNonAlignedSQWord_t  /* Unaligned Signed 64 Bit    */
{                                                              /* Container.                 */
   Byte_t NonAlignedSQWord0;
   Byte_t NonAlignedSQWord1;
   Byte_t NonAlignedSQWord2;
   Byte_t NonAlignedSQWord3;
   Byte_t NonAlignedSQWord4;
   Byte_t NonAlignedSQWord5;
   Byte_t NonAlignedSQWord6;
   Byte_t NonAlignedSQWord7;
} __PACKED_STRUCT_END__ NonAlignedSQWord_t;

#define NON_ALIGNED_SQWORD_SIZE                 (sizeof(NonAlignedSQWord_t))

   /* The following MACRO is a utility MACRO that exists to calculate   */
   /* the offset position of a particular structure member from the     */
   /* start of the structure.  This MACRO accepts as the first          */
   /* parameter, the physical name of the structure (the type name, NOT */
   /* the variable name).  The second parameter to this MACRO represents*/
   /* the actual structure member that the offset is to be determined.  */
   /* This MACRO returns an unsigned integer that represents the offset */
   /* (in bytes) of the structure member.                               */
#define STRUCTURE_OFFSET(_x, _y)                ((unsigned int )(unsigned long *)&(((_x *)0)->_y))

   /* The following MACRO is a utility MACRO that exists to swap the    */
   /* byte ordering of the individual Byte values in the specified Byte.*/
   /* This MACRO accepts as input a Byte_t and returns a Byte_t which   */
   /* is the Reverse Byte Ordering of the Input parameter.              */
#define CHANGE_BYTE_ORDERING_BYTE(_x)                   ((Byte_t)(_x))

   /* The following MACRO is a utility MACRO that exists to swap the    */
   /* byte ordering of the individual Byte values in the specified      */
   /* Word.  This MACRO accepts as input a Word_t and returns a Word_t  */
   /* which is the Reverse Byte Ordering of the Input parameter.        */
#define CHANGE_BYTE_ORDERING_WORD(_x)                   ((Word_t)(((Word_t)(_x) >> 8) | ((Word_t)(_x) << 8)))

   /* The following MACRO is a utility MACRO that exists to swap the    */
   /* byte ordering of the individual Byte values in the specified      */
   /* DWord.  This MACRO accepts as input a DWord_t and returns a       */
   /* DWord_t which is the Reverse Byte Ordering of the Input parameter.*/
#define CHANGE_BYTE_ORDERING_DWORD(_x)                  ((DWord_t)(((DWord_t)(_x) << 24) | (((DWord_t)(_x) << 8) & 0x00FF0000L) | (((DWord_t)(_x) >> 8) & 0x0000FF00L) | ((DWord_t)(_x) >> 24)))

   /* The following MACRO is a utility MACRO that exists to swap the    */
   /* byte ordering of the individual Byte values in the specified      */
   /* QWord.  This MACRO accepts as input a QWord_t and returns a       */
   /* QWord_t which is the Reverse Byte Ordering of the Input parameter.*/
#define CHANGE_BYTE_ORDERING_QWORD(_x) \
   (((QWord_t)(_x) << 56) | (((QWord_t)(((DWord_t)(_x)) & 0x0000FF00L)) << 40) | (((QWord_t)(((DWord_t)(_x)) & 0x00FF0000L)) << 24) | (((QWord_t)(((DWord_t)(_x)) & 0xFF000000L)) << 8) | (((QWord_t)(_x)) >> 56) | (((DWord_t)(((QWord_t)(_x)) >> 40)) & 0x0000FF00L) | (((DWord_t)(((QWord_t)(_x)) >> 24)) & 0x00FF0000L) | (((DWord_t)(((QWord_t)(_x)) >> 8)) & 0xFF000000L))

   /* The following is a utility MACRO that exists to Assign a          */
   /* specified Byte_t to an unaligned Memory Address.  This MACRO      */
   /* accepts as it's first parameter the Memory Address to store the   */
   /* specified Little Endian Byte_t into.  The second parameter is the */
   /* actual Byte_t value to store into the specified Memory Location.  */
   /* * NOTE * The second parameter needs to be stored in the Endian    */
   /*          format of the Native Host's processor.                   */
#define ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(_x, _y)    \
{                                                                   \
  ((Byte_t *)(_x))[0] = ((Byte_t)(_y));                             \
}

   /* The following is a utility MACRO that exists to Assign a          */
   /* specified Word_t to an unaligned Memory Address.  This MACRO      */
   /* accepts as it's first parameter the Memory Address to store the   */
   /* specified Little Endian Word_t into.  The second parameter is the */
   /* actual Word_t value to store into the specified Memory Location.  */
   /* * NOTE * The second parameter needs to be stored in the Endian    */
   /*          format of the Native Host's processor.                   */
#define ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(_x, _y)    \
{                                                                   \
  ((Byte_t *)(_x))[0] = ((Byte_t)(((Word_t)(_y)) & 0xFF));          \
  ((Byte_t *)(_x))[1] = ((Byte_t)((((Word_t)(_y)) >> 8) & 0xFF));   \
}

   /* The following is a utility MACRO that exists to Assign a          */
   /* specified DWord_t to an unaligned Memory Address.  This MACRO     */
   /* accepts as it's first parameter the Memory Address to store the   */
   /* specified Little Endian DWord_t into.  The second parameter is the*/
   /* actual DWord_t value to store into the specified Memory Location. */
   /* * NOTE * The second parameter needs to be stored in the Endian    */
   /*          format of the Native Host's processor.                   */
#define ASSIGN_HOST_DWORD_TO_LITTLE_ENDIAN_UNALIGNED_DWORD(_x, _y)  \
{                                                                   \
  ((Byte_t *)(_x))[0] = ((Byte_t)(((DWord_t)(_y)) & 0xFF));         \
  ((Byte_t *)(_x))[1] = ((Byte_t)((((DWord_t)(_y)) >> 8) & 0xFF));  \
  ((Byte_t *)(_x))[2] = ((Byte_t)((((DWord_t)(_y)) >> 16) & 0xFF)); \
  ((Byte_t *)(_x))[3] = ((Byte_t)((((DWord_t)(_y)) >> 24) & 0xFF)); \
}

   /* The following is a utility MACRO that exists to Assign a          */
   /* specified QWord_t to an unaligned Memory Address.  This MACRO     */
   /* accepts as it's first parameter the Memory Address to store the   */
   /* specified Little Endian QWord_t into.  The second parameter is the*/
   /* actual QWord_t value to store into the specified Memory Location. */
   /* * NOTE * The second parameter needs to be stored in the Endian    */
   /*          format of the Native Host's processor.                   */
#define ASSIGN_HOST_QWORD_TO_LITTLE_ENDIAN_UNALIGNED_QWORD(_x, _y)  \
{                                                                   \
  ((Byte_t *)(_x))[0] = ((Byte_t)(((QWord_t)(_y)) & 0xFF));         \
  ((Byte_t *)(_x))[1] = ((Byte_t)((((QWord_t)(_y)) >> 8) & 0xFF));  \
  ((Byte_t *)(_x))[2] = ((Byte_t)((((QWord_t)(_y)) >> 16) & 0xFF)); \
  ((Byte_t *)(_x))[3] = ((Byte_t)((((QWord_t)(_y)) >> 24) & 0xFF)); \
  ((Byte_t *)(_x))[4] = ((Byte_t)((((QWord_t)(_y)) >> 32) & 0xFF)); \
  ((Byte_t *)(_x))[5] = ((Byte_t)((((QWord_t)(_y)) >> 40) & 0xFF)); \
  ((Byte_t *)(_x))[6] = ((Byte_t)((((QWord_t)(_y)) >> 48) & 0xFF)); \
  ((Byte_t *)(_x))[7] = ((Byte_t)((((QWord_t)(_y)) >> 56) & 0xFF)); \
}

   /* The following is a utility MACRO that exists to Read an unaligned */
   /* Little Endian Byte_t from a specifed Memory Address.  This MACRO  */
   /* accepts as it's first parameter the unaligned Memory Address of   */
   /* the Little Endian Byte_t to read.  This function returns a Byte_t */
   /* (in the Endian-ness of the Native Host Processor).                */
#define READ_UNALIGNED_BYTE_LITTLE_ENDIAN(_x)  (((Byte_t *)(_x))[0])

   /* The following is a utility MACRO that exists to Read an unaligned */
   /* Little Endian Word_t from a specifed Memory Address.  This MACRO  */
   /* accepts as it's first parameter the unaligned Memory Address of   */
   /* the Little Endian Word_t to read.  This function returns a Word_t */
   /* (in the Endian-ness of the Native Host Processor).                */
#define READ_UNALIGNED_WORD_LITTLE_ENDIAN(_x)  ((Word_t)((((Word_t)(((Byte_t *)(_x))[1])) << 8) | ((Word_t)(((Byte_t *)(_x))[0]))))

   /* The following is a utility MACRO that exists to Read an unaligned */
   /* Little Endian DWord_t from a specifed Memory Address.  This MACRO */
   /* accepts as it's first parameter the unaligned Memory Address of   */
   /* the Little Endian DWord_t to read.  This function returns a       */
   /* DWord_t (in the Endian-ness of the Native Host Processor).        */
#define READ_UNALIGNED_DWORD_LITTLE_ENDIAN(_x)  ((DWord_t)((((DWord_t)(((Byte_t *)(_x))[3])) << 24) | (((DWord_t)(((Byte_t *)(_x))[2])) << 16) | (((DWord_t)(((Byte_t *)(_x))[1])) << 8) | ((DWord_t)(((Byte_t *)(_x))[0]))))

   /* The following is a utility MACRO that exists to Read an unaligned */
   /* Little Endian QWord_t from a specifed Memory Address.  This MACRO */
   /* accepts as it's first parameter the unaligned Memory Address of   */
   /* the Little Endian QWord_t to read.  This function returns a       */
   /* QWord_t (in the Endian-ness of the Native Host Processor).        */
#define READ_UNALIGNED_QWORD_LITTLE_ENDIAN(_x)  ((QWord_t)((((QWord_t)(((Byte_t *)(_x))[7])) << 56) | (((QWord_t)(((Byte_t *)(_x))[6])) << 48) | (((QWord_t)(((Byte_t *)(_x))[5])) << 40) | (((QWord_t)(((Byte_t *)(_x))[4])) << 32) | (((DWord_t)(((Byte_t *)(_x))[3])) << 24) | (((DWord_t)(((Byte_t *)(_x))[2])) << 16) | (((DWord_t)(((Byte_t *)(_x))[1])) << 8) | ((DWord_t)(((Byte_t *)(_x))[0]))))

   /* The following is a utility MACRO that exists to Assign a          */
   /* specified Byte_t to an unaligned Memory Address.  This MACRO      */
   /* accepts as it's first parameter the Memory Address to store the   */
   /* specified Big Endian Byte_t into.  The second parameter is the    */
   /* actual Byte_t value to store into the specified Memory Location.  */
   /* * NOTE * The second parameter needs to be stored in the Endian    */
   /*          format of the Native Host's processor.                   */
#define ASSIGN_HOST_BYTE_TO_BIG_ENDIAN_UNALIGNED_BYTE(_x, _y)       \
{                                                                   \
  ((Byte_t *)(_x))[0] = ((Byte_t)(_y));                             \
}

   /* The following is a utility MACRO that exists to Assign a          */
   /* specified Word_t to an unaligned Memory Address.  This MACRO      */
   /* accepts as it's first parameter the Memory Address to store the   */
   /* specified Big Endian Word_t into.  The second parameter is the    */
   /* actual Word_t value to store into the specified Memory Location.  */
   /* * NOTE * The second parameter needs to be stored in the Endian    */
   /*          format of the Native Host's processor.                   */
#define ASSIGN_HOST_WORD_TO_BIG_ENDIAN_UNALIGNED_WORD(_x, _y)       \
{                                                                   \
  ((Byte_t *)(_x))[1] = ((Byte_t)(((Word_t)(_y)) & 0xFF));          \
  ((Byte_t *)(_x))[0] = ((Byte_t)((((Word_t)(_y)) >> 8) & 0xFF));   \
}

   /* The following is a utility MACRO that exists to Assign a          */
   /* specified DWord_t to an unaligned Memory Address.  This MACRO     */
   /* accepts as it's first parameter the Memory Address to store the   */
   /* specified Big Endian DWord_t into.  The second parameter is the   */
   /* actual DWord_t value to store into the specified Memory Location. */
   /* * NOTE * The second parameter needs to be stored in the Endian    */
   /*          format of the Native Host's processor.                   */
#define ASSIGN_HOST_DWORD_TO_BIG_ENDIAN_UNALIGNED_DWORD(_x, _y)     \
{                                                                   \
  ((Byte_t *)(_x))[3] = ((Byte_t)(((DWord_t)(_y)) & 0xFF));         \
  ((Byte_t *)(_x))[2] = ((Byte_t)((((DWord_t)(_y)) >> 8) & 0xFF));  \
  ((Byte_t *)(_x))[1] = ((Byte_t)((((DWord_t)(_y)) >> 16) & 0xFF)); \
  ((Byte_t *)(_x))[0] = ((Byte_t)((((DWord_t)(_y)) >> 24) & 0xFF)); \
}

   /* The following is a utility MACRO that exists to Assign a          */
   /* specified QWord_t to an unaligned Memory Address.  This MACRO     */
   /* accepts as it's first parameter the Memory Address to store the   */
   /* specified Big Endian QWord_t into.  The second parameter is the   */
   /* actual QWord_t value to store into the specified Memory Location. */
   /* * NOTE * The second parameter needs to be stored in the Endian    */
   /*          format of the Native Host's processor.                   */
#define ASSIGN_HOST_QWORD_TO_BIG_ENDIAN_UNALIGNED_QWORD(_x, _y)     \
{                                                                   \
  ((Byte_t *)(_x))[7] = ((Byte_t)(((QWord_t)(_y)) & 0xFF));         \
  ((Byte_t *)(_x))[6] = ((Byte_t)((((QWord_t)(_y)) >> 8) & 0xFF));  \
  ((Byte_t *)(_x))[5] = ((Byte_t)((((QWord_t)(_y)) >> 16) & 0xFF)); \
  ((Byte_t *)(_x))[4] = ((Byte_t)((((QWord_t)(_y)) >> 24) & 0xFF)); \
  ((Byte_t *)(_x))[3] = ((Byte_t)((((QWord_t)(_y)) >> 32) & 0xFF)); \
  ((Byte_t *)(_x))[2] = ((Byte_t)((((QWord_t)(_y)) >> 40) & 0xFF)); \
  ((Byte_t *)(_x))[1] = ((Byte_t)((((QWord_t)(_y)) >> 48) & 0xFF)); \
  ((Byte_t *)(_x))[0] = ((Byte_t)((((QWord_t)(_y)) >> 56) & 0xFF)); \
}

   /* The following is a utility MACRO that exists to Read an unaligned */
   /* Big Endian Byte_t from a specifed Memory Address.  This MACRO     */
   /* accepts as it's first parameter the unaligned Memory Address of   */
   /* the Big Endian Byte_t to read.  This function returns a Byte_t (in*/
   /* the Endian-ness of the Native Host Processor).                    */
#define READ_UNALIGNED_BYTE_BIG_ENDIAN(_x)      (((Byte_t *)(_x))[0])

   /* The following is a utility MACRO that exists to Read an unaligned */
   /* Big Endian Word_t from a specifed Memory Address.  This MACRO     */
   /* accepts as it's first parameter the unaligned Memory Address of   */
   /* the Big Endian Word_t to read.  This function returns a Word_t    */
   /* (in the Endian-ness of the Native Host Processor).                */
#define READ_UNALIGNED_WORD_BIG_ENDIAN(_x)      ((Word_t)((((Word_t)(((Byte_t *)(_x))[0])) << 8) | ((Word_t)(((Byte_t *)(_x))[1]))))

   /* The following is a utility MACRO that exists to Read an unaligned */
   /* Big Endian DWord_t from a specifed Memory Address.  This MACRO    */
   /* accepts as it's first parameter the unaligned Memory Address of   */
   /* the Big Endian DWord_t to read.  This function returns a DWord_t  */
   /* (in the Endian-ness of the Native Host Processor).                */
#define READ_UNALIGNED_DWORD_BIG_ENDIAN(_x)     ((DWord_t)((((DWord_t)(((Byte_t *)(_x))[0])) << 24) | (((DWord_t)(((Byte_t *)(_x))[1])) << 16) | (((DWord_t)(((Byte_t *)(_x))[2])) << 8) | ((DWord_t)(((Byte_t *)(_x))[3]))))

   /* The following is a utility MACRO that exists to Read an unaligned */
   /* Big Endian QWord_t from a specifed Memory Address.  This MACRO    */
   /* accepts as it's first parameter the unaligned Memory Address of   */
   /* the Big Endian QWord_t to read.  This function returns a QWord_t  */
   /* (in the Endian-ness of the Native Host Processor).                */
#define READ_UNALIGNED_QWORD_BIG_ENDIAN(_x)     ((QWord_t)((((QWord_t)(((Byte_t *)(_x))[0])) << 56) | (((QWord_t)(((Byte_t *)(_x))[1])) << 48) | (((QWord_t)(((Byte_t *)(_x))[2])) << 40) | (((QWord_t)(((Byte_t *)(_x))[3])) << 32) | (((DWord_t)(((Byte_t *)(_x))[4])) << 24) | (((DWord_t)(((Byte_t *)(_x))[5])) << 16) | (((DWord_t)(((Byte_t *)(_x))[6])) << 8) | ((DWord_t)(((Byte_t *)(_x))[7]))))

   /* The following is a utility MACRO that exists to Assign a generic  */
   /* Type into the specified unaligned Memory Address.  This MACRO     */
   /* accepts as it's first parameter the Memory Address to store the   */
   /* specified Generic Type into.  The second parameter is the actual  */
   /* Generic Type value to store into the specified Memory Location.   */
   /* * NOTE * The first parameter *MUST* be a Typed pointer that is    */
   /*          able to have it's value set using the equal operator (=).*/
   /*          This MACRO is envisioned to be used with the types       */
   /*          defined in this file that are typed (at their lowest     */
   /*          level) as structures.                                    */
   /* * NOTE * The second parameter needs to be stored in the Correct   */
   /*          Endian format because it will be copied verbatim (i.e.   */
   /*          Endian-ness of the second parameter will be maintained   */
   /*          into the Memory Address of the first parameter.          */
#define ASSIGN_UNALIGNED_GENERIC_TYPE(_x, _y)                       \
{                                                                   \
   (*(_x)) = (_y);                                                  \
}

   /* The following is a utility MACRO that exists to Read an unaligned */
   /* Generic Type from a specifed Memory Address.  This MACRO accepts  */
   /* as it's first parameter the unaligned Memory Address of the       */
   /* Generic Type to read.  This function returns a Generic Type (in   */
   /* the Endian-ness of the Generic Type).                             */
   /* * NOTE * The first parameter *MUST* be a Typed pointer that is    */
   /*          able to have it's value set using the equal operator (=).*/
   /*          This MACRO is envisioned to be used with the types       */
   /*          defined in this file that are typed (at their lowest     */
   /*          level) as structures.                                    */
   /* * NOTE * The second parameter needs to be stored in the Correct   */
   /*          Endian format because it will be copied verbatim (i.e.   */
   /*          Endian-ness of the second parameter will be maintained   */
   /*          into the Memory Address of the first parameter.          */
#define READ_UNALIGNED_GENERIC_TYPE(_x)         (*(_x))

#endif
