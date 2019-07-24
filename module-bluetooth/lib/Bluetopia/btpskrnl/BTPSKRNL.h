/*****< btpskrnl.h >***********************************************************/
/*      Copyright 2012 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  BTPSKRNL - Stonestreet One Bluetooth Stack Kernel Implementation Type     */
/*             Definitions, Constants, and Prototypes.                        */
/*                                                                            */
/*  Author:  Marcus Funk                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   11/08/12  M. Funk        Initial creation.                               */
/******************************************************************************/
#ifndef __BTPSKRNLH__
#define __BTPSKRNLH__

#include "BKRNLAPI.h"           /* Bluetooth Kernel Prototypes/Constants.     */

   /* The following declared type represents the Prototype Function for */
   /* a function that can be registered with the BTPSKRNL module to     */
   /* receive output characters.  This function will be called whenever */
   /* BTPS_OutputMessage() or BTPS_DumpData() is called (or if debug is */
   /* is enabled - DEBUG preprocessor symbol, whenever the DBG_MSG() or */
   /* DBG_DUMP() MACRO is used and there is debug data to output.       */
   /* * NOTE * This function can be registered by passing the address   */
   /*          of the implementation function in the                    */
   /*          MessageOutputCallback member of the                      */
   /*          BTPS_Initialization_t structure which is passed to the   */
   /*          BTPS_Init() function.  If no function is registered then */
   /*          there will be no output (i.e. it will simply be ignored).*/
typedef int (BTPSAPI *BTPS_MessageOutputCallback_t)(int Length, char *Message);

   /* The following structure represents the structure that is passed   */
   /* to the BTPS_Init() function to notify the Bluetooth abstraction   */
   /* layer of the function(s) that are required for proper device      */
   /* functionality.                                                    */
   /* * NOTE * This structure *MUST* be passed to the BTPS_Init()       */
   /*          function AND THE GetTickCountCallback MEMBER MUST BE     */
   /*          SPECIFIED.  Failure to provide this function will cause  */
   /*          the Bluetooth sub-system to not function because the     */
   /*          scheduler will not function (as the Tick Count will      */
   /*          never change).                                           */
typedef struct _tagBTPS_Initialization_t
{
   BTPS_MessageOutputCallback_t MessageOutputCallback;
} BTPS_Initialization_t;

#define BTPS_INITIALIZATION_SIZE                         (sizeof(BTPS_Initialization_t))

   /* The following structure represents the statistics for the memory  */
   /* heap for use with BTPS_QueryMemoryUsage().                        */
typedef struct _tagBTPS_MemoryStatistics_t
{
   unsigned int HeapSize;
   unsigned int CurrentHeapUsed;
   unsigned int MaximumHeapUsed;
   unsigned int FreeFragmentCount;
   unsigned int LargestFreeFragment;
} BTPS_MemoryStatistics_t;

   /* The following function is responsible for the Memory Usage        */
   /* Information.  This function accepts as its parameter a pointer to */
   /* a memory statistics structure and a flag to indicate if fragment  */
   /* information will be determined.  The function will return zero if */
   /* successful or a negative value if there is an error.              */
   /* * NOTE * If the advanced statitistics flag is set to FALSE, then  */
   /*          the largest free fragment and free fragment count will be*/
   /*          set to zero.                                             */
int BTPSAPI BTPS_QueryMemoryUsage(BTPS_MemoryStatistics_t *MemoryStatistics, Boolean_t AdvancedStatitics);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Mutex_t (BTPSAPI *PFN_BTPS_QueryMemoryUsage)(MemoryStatistics_t *MemoryStatistics, Boolean_t AdvancedStatitics);
#endif

#include "BKRNLAPI.h"           /* Bluetooth Kernel Prototypes/Constants.     */

#endif
