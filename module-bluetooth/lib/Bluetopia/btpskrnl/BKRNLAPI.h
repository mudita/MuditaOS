/*****< bkrnlapi.h >***********************************************************/
/*      Copyright 2001 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  BKRNLAPI - Stonestreet One Bluetooth Stack Kernel API Type Definitions,   */
/*             Constants, and Prototypes.                                     */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   05/30/01  D. Lange       Initial creation.                               */
/******************************************************************************/
#ifndef __BKRNLAPIH__
#define __BKRNLAPIH__

#include "BTAPITyp.h"           /* Bluetooth API Type Definitions.            */
#include "BTTypes.h"            /* Bluetooth basic type definitions           */

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

   /* The following preprocessor definitions control the inclusion of   */
   /* debugging output.                                                 */
   /*                                                                   */
   /*    - DEBUG_ENABLED                                                */
   /*         - When defined enables debugging, if no other debugging   */
   /*           preprocessor definitions are defined then the debugging */
   /*           output is logged to a file (and included in the         */
   /*           driver).                                                */
   /*                                                                   */
   /*          - DEBUG_ZONES                                            */
   /*              - When defined (only when DEBUG_ENABLED is defined)  */
   /*                forces the value of this definition (unsigned long)*/
   /*                to be the Debug Zones that are enabled.            */
#define DBG_ZONE_CRITICAL_ERROR           (1 << 0)
#define DBG_ZONE_ENTER_EXIT               (1 << 1)
#define DBG_ZONE_BTPSKRNL                 (1 << 2)
#define DBG_ZONE_GENERAL                  (1 << 3)
#define DBG_ZONE_DEVELOPMENT              (1 << 4)
#define DBG_ZONE_VENDOR                   (1 << 5)

#define DBG_ZONE_ANY                      ((unsigned long)-1)

#ifndef DEBUG_ZONES
   #define DEBUG_ZONES                    DBG_ZONE_CRITICAL_ERROR
#endif

#ifndef MAX_DBG_DUMP_BYTES
   #define MAX_DBG_DUMP_BYTES             (((unsigned int)-1) - 1)
#endif

#ifdef DEBUG_ENABLED
   #define DBG_MSG(_zone_, _x_)           do { if(BTPS_TestDebugZone(_zone_)) BTPS_OutputMessage _x_; } while(0)
   #define DBG_DUMP(_zone_, _x_)          do { if(BTPS_TestDebugZone(_zone_)) BTPS_DumpData _x_; } while(0)
#else
   #define DBG_MSG(_zone_, _x_)
   #define DBG_DUMP(_zone_, _x_)
#endif

   /* The following constant defines a special length of time that      */
   /* specifies that there is to be NO Timeout waiting for some Event to*/
   /* occur (Mutexes, Semaphores, Events, etc).                         */
#define BTPS_INFINITE_WAIT                (0xFFFFFFFF)

#define BTPS_NO_WAIT                      0

   /* The following type definition defines a BTPS Kernel API Event     */
   /* Handle.                                                           */
typedef void *Event_t;

   /* The following type definition defines a BTPS Kernel API Mutex     */
   /* Handle.                                                           */
typedef void *Mutex_t;

   /* The following type definition defines a BTPS Kernel API Thread    */
   /* Handle.                                                           */
typedef void *ThreadHandle_t;

   /* The following type definition defines a BTPS Kernel API Mailbox   */
   /* Handle.                                                           */
typedef void *Mailbox_t;

   /* The following MACRO is a utility MACRO that exists to calculate   */
   /* the offset position of a particular structure member from the     */
   /* start of the structure.  This MACRO accepts as the first          */
   /* parameter, the physical name of the structure (the type name, NOT */
   /* the variable name).  The second parameter to this MACRO represents*/
   /* the actual structure member that the offset is to be determined.  */
   /* This MACRO returns an unsigned integer that represents the offset */
   /* (in bytes) of the structure member.                               */
#define BTPS_STRUCTURE_OFFSET(_x, _y)     ((unsigned int)&(((_x *)0)->_y))

   /* The following type declaration represents the Prototype for a     */
   /* Thread Function.  This function represents the Thread that will be*/
   /* executed when passed to the BTPS_CreateThread() function.         */
   /* * NOTE * Once a Thread is created there is NO way to kill it.  The*/
   /*          Thread must exit by itself.                              */
typedef void *(BTPSAPI *Thread_t)(void *ThreadParameter);

   /* The following type declaration represents the Prototype for the   */
   /* function that is passed to the BTPS_DeleteMailbox() function to   */
   /* process all remaining Queued Mailbox Messages.  This allows a     */
   /* mechanism to free any resources that are attached with each queued*/
   /* Mailbox message.                                                  */
typedef void (BTPSAPI *BTPS_MailboxDeleteCallback_t)(void *MailboxData);

   /* The following function is responsible for delaying the current    */
   /* task for the specified duration (specified in Milliseconds).      */
   /* * NOTE * Very small timeouts might be smaller in granularity than */
   /*          the system can support !!!!                              */
BTPSAPI_DECLARATION void BTPSAPI BTPS_Delay(unsigned long MilliSeconds);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_Delay_t)(unsigned long MilliSeconds);
#endif

   /* The following function is responsible for creating an actual Mutex*/
   /* (Binary Semaphore).  The Mutex is unique in that if a Thread      */
   /* already owns the Mutex, and it requests the Mutex again it will be*/
   /* granted the Mutex.  This is in Stark contrast to a Semaphore that */
   /* will block waiting for the second acquisition of the Sempahore.   */
   /* This function accepts as input whether or not the Mutex is        */
   /* initially Signalled or not.  If this input parameter is TRUE then */
   /* the caller owns the Mutex and any other threads waiting on the    */
   /* Mutex will block.  This function returns a NON-NULL Mutex Handle  */
   /* if the Mutex was successfully created, or a NULL Mutex Handle if  */
   /* the Mutex was NOT created.  If a Mutex is successfully created, it*/
   /* can only be destroyed by calling the BTPS_CloseMutex() function   */
   /* (and passing the returned Mutex Handle).                          */
BTPSAPI_DECLARATION Mutex_t BTPSAPI BTPS_CreateMutex(Boolean_t CreateOwned);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Mutex_t (BTPSAPI *PFN_BTPS_CreateMutex_t)(Boolean_t CreateOwned);
#endif

   /* The following function is responsible for waiting for the         */
   /* specified Mutex to become free.  This function accepts as input   */
   /* the Mutex Handle to wait for, and the Timeout (specified in       */
   /* Milliseconds) to wait for the Mutex to become available.  This    */
   /* function returns TRUE if the Mutex was successfully acquired and  */
   /* FALSE if either there was an error OR the Mutex was not acquired  */
   /* in the specified Timeout.  It should be noted that Mutexes have   */
   /* the special property that if the calling Thread already owns the  */
   /* Mutex and it requests access to the Mutex again (by calling this  */
   /* function and specifying the same Mutex Handle) then it will       */
   /* automatically be granted the Mutex.  Once a Mutex has been granted*/
   /* successfully (this function returns TRUE), then the caller MUST   */
   /* call the BTPS_ReleaseMutex() function.                            */
   /* * NOTE * There must exist a corresponding BTPS_ReleaseMutex()     */
   /*          function call for EVERY successful BTPS_WaitMutex()      */
   /*          function call or a deadlock will occur in the system !!! */
BTPSAPI_DECLARATION Boolean_t BTPSAPI BTPS_WaitMutex(Mutex_t Mutex, unsigned long Timeout);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_BTPS_WaitMutex_t)(Mutex_t Mutex, unsigned long Timeout);
#endif

   /* The following function is responsible for releasing a Mutex that  */
   /* was successfully acquired with the BTPS_WaitMutex() function.     */
   /* This function accepts as input the Mutex that is currently owned. */
   /* * NOTE * There must exist a corresponding BTPS_ReleaseMutex()     */
   /*          function call for EVERY successful BTPS_WaitMutex()      */
   /*          function call or a deadlock will occur in the system !!! */
BTPSAPI_DECLARATION void BTPSAPI BTPS_ReleaseMutex(Mutex_t Mutex);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_ReleaseMutex_t)(Mutex_t Mutex);
#endif

   /* The following function is responsible for destroying a Mutex that */
   /* was created successfully via a successful call to the             */
   /* BTPS_CreateMutex() function.  This function accepts as input the  */
   /* Mutex Handle of the Mutex to destroy.  Once this function is      */
   /* completed the Mutex Handle is NO longer valid and CANNOT be used. */
   /* Calling this function will cause all outstanding BTPS_WaitMutex() */
   /* functions to fail with an error.                                  */
BTPSAPI_DECLARATION void BTPSAPI BTPS_CloseMutex(Mutex_t Mutex);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_CloseMutex_t)(Mutex_t Mutex);
#endif

   /* The following function is responsible for creating an actual      */
   /* Event.  The Event is unique in that it only has two states.  These*/
   /* states are Signalled and Non-Signalled.  Functions are provided to*/
   /* allow the setting of the current state and to allow the option of */
   /* waiting for an Event to become Signalled.  This function accepts  */
   /* as input whether or not the Event is initially Signalled or not.  */
   /* If this input parameter is TRUE then the state of the Event is    */
   /* Signalled and any BTPS_WaitEvent() function calls will immediately*/
   /* return.  This function returns a NON-NULL Event Handle if the     */
   /* Event was successfully created, or a NULL Event Handle if the     */
   /* Event was NOT created.  If an Event is successfully created, it   */
   /* can only be destroyed by calling the BTPS_CloseEvent() function   */
   /* (and passing the returned Event Handle).                          */
BTPSAPI_DECLARATION Event_t BTPSAPI BTPS_CreateEvent(Boolean_t CreateSignalled);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Event_t (BTPSAPI *PFN_BTPS_CreateEvent_t)(Boolean_t CreateSignalled);
#endif

   /* The following function is responsible for waiting for the         */
   /* specified Event to become Signalled.  This function accepts as    */
   /* input the Event Handle to wait for, and the Timeout (specified in */
   /* Milliseconds) to wait for the Event to become Signalled.  This    */
   /* function returns TRUE if the Event was set to the Signalled State */
   /* (in the Timeout specified) or FALSE if either there was an error  */
   /* OR the Event was not set to the Signalled State in the specified  */
   /* Timeout.  It should be noted that Signals have a special property */
   /* in that multiple Threads can be waiting for the Event to become   */
   /* Signalled and ALL calls to BTPS_WaitEvent() will return TRUE      */
   /* whenever the state of the Event becomes Signalled.                */
BTPSAPI_DECLARATION Boolean_t BTPSAPI BTPS_WaitEvent(Event_t Event, unsigned long Timeout);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_BTPS_WaitEvent_t)(Event_t Event, unsigned long Timeout);
#endif

   /* The following function is responsible for changing the state of   */
   /* the specified Event to the Non-Signalled State.  Once the Event is*/
   /* in this State, ALL calls to the BTPS_WaitEvent() function will    */
   /* block until the State of the Event is set to the Signalled State. */
   /* This function accepts as input the Event Handle of the Event to   */
   /* set to the Non-Signalled State.                                   */
BTPSAPI_DECLARATION void BTPSAPI BTPS_ResetEvent(Event_t Event);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_ResetEvent_t)(Event_t Event);
#endif

   /* The following function is responsible for changing the state of   */
   /* the specified Event to the Signalled State.  Once the Event is in */
   /* this State, ALL calls to the BTPS_WaitEvent() function will       */
   /* return.  This function accepts as input the Event Handle of the   */
   /* Event to set to the Signalled State.                              */
BTPSAPI_DECLARATION void BTPSAPI BTPS_SetEvent(Event_t Event);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_SetEvent_t)(Event_t Event);
#endif

   /* The following function is responsible for destroying an Event that*/
   /* was created successfully via a successful call to the             */
   /* BTPS_CreateEvent() function.  This function accepts as input the  */
   /* Event Handle of the Event to destroy.  Once this function is      */
   /* completed the Event Handle is NO longer valid and CANNOT be used. */
   /* Calling this function will cause all outstanding BTPS_WaitEvent() */
   /* functions to fail with an error.                                  */
BTPSAPI_DECLARATION void BTPSAPI BTPS_CloseEvent(Event_t Event);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_CloseEvent_t)(Event_t Event);
#endif

   /* The following function is provided to allow a mechanism to        */
   /* actually allocate a Block of Memory (of at least the specified    */
   /* size).  This function accepts as input the size (in Bytes) of the */
   /* Block of Memory to be allocated.  This function returns a NON-NULL*/
   /* pointer to this Memory Buffer if the Memory was successfully      */
   /* allocated, or a NULL value if the memory could not be allocated.  */
BTPSAPI_DECLARATION void *BTPSAPI BTPS_AllocateMemory(unsigned long MemorySize);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void *(BTPSAPI *PFN_BTPS_AllocateMemory_t)(unsigned long MemorySize);
#endif

   /* The following function is responsible for de-allocating a Block of*/
   /* Memory that was successfully allocated with the                   */
   /* BTPS_AllocateMemory() function.  This function accepts a NON-NULL */
   /* Memory Pointer which was returned from the BTPS_AllocateMemory()  */
   /* function.  After this function completes the caller CANNOT use ANY*/
   /* of the Memory pointed to by the Memory Pointer.                   */
BTPSAPI_DECLARATION void BTPSAPI BTPS_FreeMemory(void *MemoryPointer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_FreeMemory_t)(void *MemoryPointer);
#endif

   /* The following function is responsible for copying a block of      */
   /* memory of the specified size from the specified source pointer to */
   /* the specified destination memory pointer.  This function accepts  */
   /* as input a pointer to the memory block that is to be Destination  */
   /* Buffer (first parameter), a pointer to memory block that points to*/
   /* the data to be copied into the destination buffer, and the size   */
   /* (in bytes) of the Data to copy.  The Source and Destination Memory*/
   /* Buffers must contain AT LEAST as many bytes as specified by the   */
   /* Size parameter.                                                   */
   /* * NOTE * This function does not allow the overlapping of the      */
   /*          Source and Destination Buffers !!!!                      */
BTPSAPI_DECLARATION void BTPSAPI BTPS_MemCopy(void *Destination, BTPSCONST void *Source, unsigned long Size);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_MemCopy_t)(void *Destination, BTPSCONST void *Source, unsigned long Size);
#endif

   /* The following function is responsible for moving a block of memory*/
   /* of the specified size from the specified source pointer to the    */
   /* specified destination memory pointer.  This function accepts as   */
   /* input a pointer to the memory block that is to be Destination     */
   /* Buffer (first parameter), a pointer to memory block that points to*/
   /* the data to be copied into the destination buffer, and the size   */
   /* (in bytes) of the Data to copy.  The Source and Destination Memory*/
   /* Buffers must contain AT LEAST as many bytes as specified by the   */
   /* Size parameter.                                                   */
   /* * NOTE * This function DOES allow the overlapping of the Source   */
   /*          and Destination Buffers.                                 */
BTPSAPI_DECLARATION void BTPSAPI BTPS_MemMove(void *Destination, BTPSCONST void *Source, unsigned long Size);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_MemMove_t)(void *Destination, BTPSCONST void *Source, unsigned long Size);
#endif

   /* The following function is provided to allow a mechanism to fill a */
   /* block of memory with the specified value.  This function accepts  */
   /* as input a pointer to the Data Buffer (first parameter) that is to*/
   /* filled with the specified value (second parameter).  The final    */
   /* parameter to this function specifies the number of bytes that are */
   /* to be filled in the Data Buffer.  The Destination Buffer must     */
   /* point to a Buffer that is AT LEAST the size of the Size parameter.*/
BTPSAPI_DECLARATION void BTPSAPI BTPS_MemInitialize(void *Destination, unsigned char Value, unsigned long Size);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_MemInitialize_t)(void *Destination, unsigned char Value, unsigned long Size);
#endif

   /* The following function is provided to allow a mechanism to Compare*/
   /* two blocks of memory to see if the two memory blocks (each of size*/
   /* Size (in bytes)) are equal (each and every byte up to Size bytes).*/
   /* This function returns a negative number if Source1 is less than   */
   /* Source2, zero if Source1 equals Source2, and a positive value if  */
   /* Source1 is greater than Source2.                                  */
BTPSAPI_DECLARATION int BTPSAPI BTPS_MemCompare(BTPSCONST void *Source1, BTPSCONST void *Source2, unsigned long Size);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_BTPS_MemCompare_t)(BTPSCONST void *Source1, BTPSCONST void *Source2, unsigned long Size);
#endif

   /* The following function is provided to allow a mechanism to Compare*/
   /* two blocks of memory to see if the two memory blocks (each of size*/
   /* Size (in bytes)) are equal (each and every byte up to Size bytes) */
   /* using a Case-Insensitive Compare.  This function returns a        */
   /* negative number if Source1 is less than Source2, zero if Source1  */
   /* equals Source2, and a positive value if Source1 is greater than   */
   /* Source2.                                                          */
BTPSAPI_DECLARATION int BTPSAPI BTPS_MemCompareI(BTPSCONST void *Source1, BTPSCONST void *Source2, unsigned long Size);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_BTPS_MemCompareI_t)(BTPSCONST void *Source1, BTPSCONST void *Source2, unsigned int Size);
#endif

   /* The following function is provided to allow a mechanism to copy a */
   /* source NULL Terminated ASCII (character) String to the specified  */
   /* Destination String Buffer.  This function accepts as input a      */
   /* pointer to a buffer (Destination) that is to receive the NULL     */
   /* Terminated ASCII String pointed to by the Source parameter.  This */
   /* function copies the string byte by byte from the Source to the    */
   /* Destination (including the NULL terminator).                      */
BTPSAPI_DECLARATION void BTPSAPI BTPS_StringCopy(char *Destination, BTPSCONST char *Source);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_StringCopy_t)(char *Destination, BTPSCONST char *Source);
#endif

   /* The following function is provided to allow a mechanism to        */
   /* determine the Length (in characters) of the specified NULL        */
   /* Terminated ASCII (character) String.  This function accepts as    */
   /* input a pointer to a NULL Terminated ASCII String and returns the */
   /* number of characters present in the string (NOT including the     */
   /* terminating NULL character).                                      */
BTPSAPI_DECLARATION unsigned int BTPSAPI BTPS_StringLength(BTPSCONST char *Source);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef unsigned int (BTPSAPI *PFN_BTPS_StringLength_t)(BTPSCONST char *Source);
#endif

   /* The following function is provided to allow a mechanism for a C   */
   /* Run-Time Library sprintf() function implementation.  This function*/
   /* accepts as its imput the output buffer, a format string and a     */
   /* variable number of arguments determined by the format string.     */
BTPSAPI_DECLARATION int BTPSAPI BTPS_SprintF(char *Buffer, BTPSCONST char *Format, ...);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef unsigned int (BTPSAPI *PFN_BTPS_SprintF_t)(char *Buffer, BTPSCONST char *Format, ...);
#endif

   /* The following function is provided to allow a means for the       */
   /* programmer to create a separate thread of execution.  This        */
   /* function accepts as input the Function that represents the Thread */
   /* that is to be installed into the system as its first parameter.   */
   /* The second parameter is the size of the Threads Stack (in bytes)  */
   /* required by the Thread when it is executing.  The final parameter */
   /* to this function represents a parameter that is to be passed to   */
   /* the Thread when it is created.  This function returns a NON-NULL  */
   /* Thread Handle if the Thread was successfully created, or a NULL   */
   /* Thread Handle if the Thread was unable to be created.  Once the   */
   /* thread is created, the only way for the Thread to be removed from */
   /* the system is for the Thread function to run to completion.       */
   /* * NOTE * There does NOT exist a function to Kill a Thread that is */
   /*          present in the system.  Because of this, other means     */
   /*          needs to be devised in order to signal the Thread that it*/
   /*          is to terminate.                                         */
BTPSAPI_DECLARATION ThreadHandle_t BTPSAPI BTPS_CreateThread(Thread_t ThreadFunction, unsigned int StackSize, void *ThreadParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef ThreadHandle_t (BTPSAPI *PFN_BTPS_CreateThread_t)(Thread_t ThreadFunction, unsigned int StackSize, void *ThreadParameter);
#endif

   /* The following function is provided to allow a mechanism to        */
   /* retrieve the handle of the thread which is currently executing.   */
   /* This function require no input parameters and will return a valid */
   /* ThreadHandle upon success.                                        */
BTPSAPI_DECLARATION ThreadHandle_t BTPSAPI BTPS_CurrentThreadHandle(void);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef ThreadHandle_t (BTPSAPI *PFN_BTPS_CurrentThreadHandle_t)(void);
#endif

   /* The following function is provided to allow a mechanism to create */
   /* a Mailbox.  A Mailbox is a Data Store that contains slots (all of */
   /* the same size) that can have data placed into (and retrieved      */
   /* from).  Once Data is placed into a Mailbox (via the               */
   /* BTPS_AddMailbox() function, it can be retrieved by using the      */
   /* BTPS_WaitMailbox() function.  Data placed into the Mailbox is     */
   /* retrieved in a FIFO method.  This function accepts as input the   */
   /* Maximum Number of Slots that will be present in the Mailbox and   */
   /* the Size of each of the Slots.  This function returns a NON-NULL  */
   /* Mailbox Handle if the Mailbox is successfully created, or a NULL  */
   /* Mailbox Handle if the Mailbox was unable to be created.           */
BTPSAPI_DECLARATION Mailbox_t BTPSAPI BTPS_CreateMailbox(unsigned int NumberSlots, unsigned int SlotSize);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Mailbox_t (BTPSAPI *PFN_BTPS_CreateMailbox_t)(unsigned int NumberSlots, unsigned int SlotSize);
#endif

   /* The following function is provided to allow a means to Add data to*/
   /* the Mailbox (where it can be retrieved via the BTPS_WaitMailbox() */
   /* function.  This function accepts as input the Mailbox Handle of   */
   /* the Mailbox to place the data into and a pointer to a buffer that */
   /* contains the data to be added.  This pointer *MUST* point to a    */
   /* data buffer that is AT LEAST the Size of the Slots in the Mailbox */
   /* (specified when the Mailbox was created) and this pointer CANNOT  */
   /* be NULL.  The data that the MailboxData pointer points to is      */
   /* placed into the Mailbox where it can be retrieved via the         */
   /* BTPS_WaitMailbox() function.                                      */
   /* * NOTE * This function copies from the MailboxData Pointer the    */
   /*          first SlotSize Bytes.  The SlotSize was specified when   */
   /*          the Mailbox was created via a successful call to the     */
   /*          BTPS_CreateMailbox() function.                           */
BTPSAPI_DECLARATION Boolean_t BTPSAPI BTPS_AddMailbox(Mailbox_t Mailbox, void *MailboxData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_BTPS_AddMailbox_t)(Mailbox_t Mailbox, void *MailboxData);
#endif

   /* The following function is provided to allow a means to retrieve   */
   /* data from the specified Mailbox.  This function will block until  */
   /* either Data is placed in the Mailbox or an error with the Mailbox */
   /* was detected.  This function accepts as its first parameter a     */
   /* Mailbox Handle that represents the Mailbox to wait for the data   */
   /* with.  This function accepts as its second parameter, a pointer to*/
   /* a data buffer that is AT LEAST the size of a single Slot of the   */
   /* Mailbox (specified when the BTPS_CreateMailbox() function was     */
   /* called).  The MailboxData parameter CANNOT be NULL.  This function*/
   /* will return TRUE if data was successfully retrieved from the      */
   /* Mailbox or FALSE if there was an error retrieving data from the   */
   /* Mailbox.  If this function returns TRUE then the first SlotSize   */
   /* bytes of the MailboxData pointer will contain the data that was   */
   /* retrieved from the Mailbox.                                       */
   /* * NOTE * This function copies to the MailboxData Pointer the data */
   /*          that is present in the Mailbox Slot (of size SlotSize).  */
   /*          The SlotSize was specified when the Mailbox was created  */
   /*          via a successful call to the BTPS_CreateMailbox()        */
   /*          function.                                                */
BTPSAPI_DECLARATION Boolean_t BTPSAPI BTPS_WaitMailbox(Mailbox_t Mailbox, void *MailboxData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_BTPS_WaitMailbox_t)(Mailbox_t Mailbox, void *MailboxData);
#endif

   /* The following function is responsible for destroying a Mailbox    */
   /* that was created successfully via a successful call to the        */
   /* BTPS_CreateMailbox() function.  This function accepts as input the*/
   /* Mailbox Handle of the Mailbox to destroy.  Once this function is  */
   /* completed the Mailbox Handle is NO longer valid and CANNOT be     */
   /* used.  Calling this function will cause all outstanding           */
   /* BTPS_WaitMailbox() functions to fail with an error.  The final    */
   /* parameter specifies an (optional) callback function that is called*/
   /* for each queued Mailbox entry.  This allows a mechanism to free   */
   /* any resources that might be associated with each individual       */
   /* Mailbox item.                                                     */
BTPSAPI_DECLARATION void BTPSAPI BTPS_DeleteMailbox(Mailbox_t Mailbox, BTPS_MailboxDeleteCallback_t MailboxDeleteCallback);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_DeleteMailbox_t)(Mailbox_t Mailbox, BTPS_MailboxDeleteCallback_t MailboxDeleteCallback);
#endif

   /* The following function is used to initialize the Platform module. */
   /* The Platform module relies on some static variables that are used */
   /* to coordinate the abstraction.  When the module is initially      */
   /* started from a cold boot, all variables are set to the proper     */
   /* state.  If the Warm Boot is required, then these variables need to*/
   /* be reset to their default values.  This function sets all static  */
   /* parameters to their default values.                               */
   /* * NOTE * The implementation is free to pass whatever information  */
   /*          required in this parameter.                              */
BTPSAPI_DECLARATION void BTPSAPI BTPS_Init(void *UserParam);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_Init_t)(void *UserParam);
#endif

   /* The following function is used to cleanup the Platform module.    */
BTPSAPI_DECLARATION void BTPSAPI BTPS_DeInit(void);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_DeInit_t)(void);
#endif

   /* Write out the specified NULL terminated Debugging String to the   */
   /* Debug output.                                                     */
BTPSAPI_DECLARATION void BTPSAPI BTPS_OutputMessage(BTPSCONST char *DebugString, ...);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_OutputMessage_t)(BTPSCONST char *DebugString, ...);
#endif

   /* The following function is used to set the Debug Mask that controls*/
   /* which debug zone messages get displayed.  The function takes as   */
   /* its only parameter the Debug Mask value that is to be used.  Each */
   /* bit in the mask corresponds to a debug zone.  When a bit is set,  */
   /* the printing of that debug zone is enabled.                       */
BTPSAPI_DECLARATION void BTPSAPI BTPS_SetDebugMask(unsigned long DebugMask);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_SetDebugMask_t)(unsigned long DebugMask);
#endif

   /* The following function is a utility function that can be used to  */
   /* determine if a specified Zone is currently enabled for debugging. */
BTPSAPI_DECLARATION int BTPSAPI BTPS_TestDebugZone(unsigned long Zone);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_BTPS_TestDebugZone_t)(unsigned long Zone);
#endif

   /* The following function is responsible for displaying binary debug */
   /* data.  The first parameter to this function is the length of data */
   /* pointed to by the next parameter.  The final parameter is a       */
   /* pointer to the binary data to be displayed.                       */
BTPSAPI_DECLARATION int BTPSAPI BTPS_DumpData(unsigned int DataLength, BTPSCONST unsigned char *DataPtr);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_BTPS_DumpData_t)(unsigned int DataLength, BTPSCONST unsigned char *DataPtr);
#endif

#endif
