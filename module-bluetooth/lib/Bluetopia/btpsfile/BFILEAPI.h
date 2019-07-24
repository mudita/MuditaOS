/*
 * Copyright 2008 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  David Wooldridge
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   03/05/08  D. Wooldridge  Initial implementation.
 *   06/17/09  D. Lange       Updated to support new API type schema.
 ****************************************************************************
 */

/**
 *  @file BFILEAPI.h
 *
 *  @brief Stonestreet One Bluetooth Stack File Access Abstraction API
*       Type Definitions, Constants, and Prototypes. To use any of the following API's,
 *      include the following declaration in your application.
 *
 *  @code
 *  #include "BTPSFILE.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __BFILEAPIH__
#define __BFILEAPIH__

#include "BTAPITyp.h"           /*! Bluetooth API Type Definitions. */
#include "BTTypes.h"            /*! Bluetooth basic type definitions */

   /*! The following values define the valid return values for the
    * BTPS_Make_Directory() function.
    */
#define BTPS_FILE_MAKE_DIRECTORY_SUCCESS                       0
#define BTPS_FILE_MAKE_DIRECTORY_ERROR_ALREADY_EXISTS       (-1)
#define BTPS_FILE_MAKE_DIRECTORY_ERROR_UNABLE_TO_CREATE     (-2)

   /*! The following type is a generic container type that represents a
    * File Descriptor (supported by this module).
    */
typedef void *BTPS_File_Descriptor_t;

   /*! The following type is a generic container type that represents a
    * Directory Descriptor (supported by this module).
    */
typedef void *BTPS_Directory_Descriptor_t;

   /*! The following enumerated type defines the supported File Types
    * that this module can detect and report.
    */
typedef enum
{
   ftUnknown,
   ftFile,
   ftDirectory,
   ftLink
} BTPS_File_Type_t;

   /*! The following enumerated type defines the supported modes that can
    * be specified to open an individual file.
    */
typedef enum
{
   omReadOnly,
   omWriteOnly,
   omReadWrite,
   omCreate,
   omAppend
} BTPS_Open_Mode_t;

   /*! The following enumerated type defines the supported modes that can
    * be specified to seek to a particular offset in an individual file.
    */
typedef enum
{
   smBeginning,
   smCurrent,
   smEnd
} BTPS_Seek_Mode_t;

   /*! The following structure defines the Date/Time Information that
    * this module can detect and report.
    */
typedef struct _tagBTPS_File_Time_t
{
   Word_t    Seconds;
   Word_t    Minute;
   Word_t    Hour;
   Word_t    DayOfMonth;
   Word_t    Month;
   Word_t    Year;
   Boolean_t UTC_Time;
} BTPS_File_Time_t;

#define BTPS_FILE_TIME_SIZE                        (sizeof(BTPS_File_Time_t))

   /*! The following structure represents the information that this
    * module supports for complete File Information about a particular
    * File/Directory entry.
    */
typedef struct _tagBTPS_File_Information_t
{
   BTPS_File_Type_t FileType;
   DWord_t          FileSize;
   BTPS_File_Time_t FileTime;
} BTPS_File_Information_t;

#define BTPS_FILE_INFORMATION_SIZE                 (sizeof(BTPS_File_Information_t))

   /*! @brief The following function is provided to allow a mechanism for the
    * caller to create a directory. @param Path The parameter to this function
    * specifies the (path qualified) directory name to create.
    * @return This function returns one of the following:
    *    - BTPS_FILE_MAKE_DIRECTORY_SUCCESS
    *    - BTPS_FILE_MAKE_DIRECTORY_ERROR_ALREADY_EXISTS
    *    - BTPS_FILE_MAKE_DIRECTORY_ERROR_UNABLE_TO_CREATE
    * \note The Path parameter should be specified in the UTF-8
    *          character set.
    */
BTPSAPI_DECLARATION int BTPSAPI BTPS_Make_Directory(BTPSCONST char *Path);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_BTPS_Make_Directory_t)(BTPSCONST char *Path);
#endif

   /*! @brief The following function is provided to allow a mechanism for the
    * caller to delete a specified file. @param Path The parameter to this
    * function specifies the (path qualified) file name to delete.
    * @return This function returns a BOOLEAN TRUE if successful, or FALSE if an
    * error occurred.
    * \note The Path parameter should be specified in the UTF-8
    *          character set.
    */
BTPSAPI_DECLARATION Boolean_t BTPSAPI BTPS_Delete_File(BTPSCONST char *Path);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_BTPS_Delete_File_t)(BTPSCONST char *Path);
#endif

   /*! @brief The following function is provided to allow a mechanism for the
    * caller to delete a specified directory. @param Path The parameter to this
    * function specifies the (path qualified) directory name to delete.
    * @return This function returns a BOOLEAN TRUE if successful, or FALSE if an
    * error occurred.
    * \note The Path parameter should be specified in the UTF-8
    *          character set.
    */
BTPSAPI_DECLARATION Boolean_t BTPSAPI BTPS_Delete_Directory(BTPSCONST char *Path);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_BTPS_Delete_Directory_t)(BTPSCONST char *Path);
#endif

   /*! @brief The following function is provided to allow a mechanism for the
    * caller to open a file (of the specified file name). @param Path This first
    * parameter to this function specifies the (path qualified) name of
    * the file to open.  @param OpenMode The final parameter specifies the mode that
    * the file is to be opened with.  The supported modes are:
    *   - omReadOnly  - open for read only access
    *   - omWriteOnly - open for write only access
    *   - omReadWrite - open for read/write access
    *   - omCreate    - create new file (truncate if already exists)
    *   - omAppend    - open for writing (create if doesn't exist)
    * @return This function returns a NON-NULL File Descriptor if the specified
    * file was opened successfully, or NULL if there was an error.
    * \note The Path parameter should be specified in the UTF-8
    *          character set.
    */
BTPSAPI_DECLARATION BTPS_File_Descriptor_t BTPSAPI BTPS_Open_File(BTPSCONST char *Path, BTPS_Open_Mode_t OpenMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef BTPS_File_Descriptor_t (BTPSAPI *PFN_BTPS_Open_File_t)(BTPSCONST char *Path, BTPS_Open_Mode_t OpenMode);
#endif

   /*! @brief The following function is provided to allow a mechansim for the
    * caller to close a file that was previously successfully opened via
    * the BTPS_Open_File() function.
    * This function accepts as @param FileDescriptor The first parameter the NON-NULL File Descriptor (successful return value
    * from the BTPS_Open_File() function).
    */
BTPSAPI_DECLARATION void BTPSAPI BTPS_Close_File(BTPS_File_Descriptor_t FileDescriptor);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_Close_File_t)(BTPS_File_Descriptor_t FileDescriptor);
#endif

   /*! @brief The following function is provided to allow a mechansim for the
    * caller to seek to specific byte offset within the specified file.
    * @param FileDescriptor The first parameter specifies the file descriptor of the
    * previously opened file (via the BTPS_Open_File() function).
    * @param SeekMode The second parameter specifies the location to apply the Seek Offset
    * to (final parameter). The values that this parameter can be is
    * one of the following:
    *   - smBeginning - Apply SeekOffset from beginning of file
    *   - smCurrent   - Apply SeekOffset from current location in file
    *   - smEnd       - Apply SeekOffset from end of file
    * @param SeekOffset The final parameter, SeekOffset, specifies the offset (in bytes)
    * to seek (taking into account the Seek Mode type - second
    * parameter).  This function will return BOOLEAN TRUE if successful
    * or a FALSE if there was an error.
    */
BTPSAPI_DECLARATION Boolean_t BTPSAPI BTPS_Seek_File(BTPS_File_Descriptor_t FileDescriptor, BTPS_Seek_Mode_t SeekMode, SDWord_t SeekOffset);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_BTPS_Seek_File_t)(BTPS_File_Descriptor_t FileDescriptor, BTPS_Seek_Mode_t SeekMode, SDWord_t SeekOffset);
#endif

   /*! @brief The following function is provided to allow a mechansim for the
    * caller to read data from a previously opened file.  @param FileDescriptor The first parameter specifies the file descriptor of the previously opened
    * file (via the BTPS_Open_File() function).  @param BufferSize The second parameter
    * specifies the number of bytes to read from the file.  @param Buffer The third parameter specifies the Buffer to read the data into (must be
    * (at least) as many bytes as specified by the second parameter).
    * @param BytesRead The final parameter is optional, and if specified, points to a
    * buffer that will contain (on success) the number of bytes that
    * were actually read and placed into the buffer.
    * @return This function returns BOOLEAN TRUE if the data was successfully read or a
    * BOOLEAN FALSE if an error occurred.
    */
BTPSAPI_DECLARATION Boolean_t BTPSAPI BTPS_Read_File(BTPS_File_Descriptor_t FileDescriptor, DWord_t BufferSize, Byte_t *Buffer, DWord_t *BytesRead);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_BTPS_Read_File_t)(BTPS_File_Descriptor_t FileDescriptor, DWord_t BufferSize, Byte_t *Buffer, DWord_t *BytesRead);
#endif

   /*! @brief The following function is provided to allow a mechansim for the
    * caller to write data to a previously opened file.
    * @param FileDescriptor The first parameter specifies the file descriptor of the previously opened
    * file (via the BTPS_Open_File() function).  @param BufferSize The second parameter
    * specifies the number of bytes to write to the file.
    * @param Buffer The third parameter specifies the Buffer that contains the bytes to write to
    * the file (must be (at least) as many bytes as specified by the second parameter).
    * @param BytesWritten The first parameter is optional, and if
    * specified, points to a buffer that will contain (on success) the
    * number of bytes that were actually written to the file.
    * @return This function returns BOOLEAN TRUE if the data was successfully read or
    * a BOOLEAN FALSE if an error occurred.
    */
BTPSAPI_DECLARATION Boolean_t BTPSAPI BTPS_Write_File(BTPS_File_Descriptor_t FileDescriptor, DWord_t BufferSize, Byte_t *Buffer, DWord_t *BytesWritten);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_BTPS_Write_File_t)(BTPS_File_Descriptor_t FileDescriptor, DWord_t BufferSize, Byte_t *Buffer, DWord_t *BytesWritten);
#endif

   /*! @brief The following function is provided to allow a mechansim for the
    * caller to begin the process of iterating through the directory
    * specified by this function. This is useful to iterate through all
    * entries contained in the specified directory (see
    * BTPS_Get_Next_Directory_Entry() for more information).
    * @param Path The first parameter to this function specifies the (path qualified)
    * directory name to open.  @return This function returns a NON-NULL
    * directory descriptor if successful, or NULL if there as an error.
    * If this function returns success, then this return value can be
    * passed to the BTPS_Get_Next_Directory_Entry() and the
    * BTPS_Close_Directory() functions.
    * \note The Path parameter should be specified in the UTF-8
    *          character set.
    */
BTPSAPI_DECLARATION BTPS_Directory_Descriptor_t BTPSAPI BTPS_Open_Directory(BTPSCONST char *Path);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef BTPS_Directory_Descriptor_t (BTPSAPI *PFN_BTPS_Open_Directory_t)(BTPSCONST char *Path);
#endif

   /*! @brief The following function is provided to allow a mechansim for the
    * caller to close a directory that was previously successfully
    * opened via the BTPS_Open_Directory() function.  This function
    * accepts as @param DirectoryDescriptor The first parameter the NON-NULL Directory Descriptor
    * (successful return value from the BTPS_Open_Directory() function).
    */
BTPSAPI_DECLARATION void BTPSAPI BTPS_Close_Directory(BTPS_Directory_Descriptor_t DirectoryDescriptor);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_BTPS_Close_Directory_t)(BTPS_Directory_Descriptor_t DirectoryDescriptor);
#endif

   /*! @brief The following function is provided to allow a mechansim for the
    * caller to interate through the directory entries contained in the
    * directory specified by the specified Directory Descriptor.
    * @param DirectoryDescriptor The first parameter specifies the Directory Descriptor (obtained from
    * a previously successful call to the BTP_Open_Directory() function)
    * which specifies the directory to iterate through.
    * @param EntryName The final parameter specifies the last retrieved directory entry name that
    * was returned from this function (to iterate to the next entry), or
    * an empty string if this is the first time this function is being
    * called for the specified directory.  If this function is
    * successful, it will return a BOOLEAN TRUE value and the buffer
    * pointed to by the final parmeter will contain the name of the next
    * directory entry.  If @return This function returns FALSE then the contents
    * of this buffer will be undefined (error occurred).
    * \note The final parameter *MUST* point to a buffer that is
    *          (at least) BTPS_MAXIMUM_FILE_NAME_LENGTH bytes long.
    * \note The directory entry names returned from this function
    *          are not path qualified (i.e. they do not contain the
    *          path of the actual directory name in them).
    * \note The final parameter should point to an empty string
    *          (first character is a NULL character) to signify that the
    *          first directory entry should be retrieved.  After this,
    *          this parameter should specify the previously returned
    *          directory entry name (to retrieve the next entry).
    * \note The value that is returned in the first parameter will
    *          be in the UTF-8 character set (NULL terminated).
    */
BTPSAPI_DECLARATION Boolean_t BTPSAPI BTPS_Get_Next_Directory_Entry(BTPS_Directory_Descriptor_t DirectoryDescriptor, char *EntryName);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_BTPS_Get_Next_Directory_Entry_t)(BTPS_Directory_Descriptor_t DirectoryDescriptor, char *EntryName);
#endif

   /*! @brief The following function is provided to allow a mechanism for the
    * caller to determine if the specified Path, specifies the Root of
    * a drive.
    * @param Path The first parameter to this function specifies the (path qualified)
    * directory name to open.
    * Examples of this function returning TRUE are:
    *   - C:\ - TRUE on Windows based machines
    *   - /   - TRUE on Linux/UNIX based machines
    * @return This function returns a BOOLEAN TRUE if the specified Path
    * specifies the root directory of a drive, or FALSE, otherwise.
    * \note The Path parameter should be specified in the UTF-8
    *          character set.
    */
BTPSAPI_DECLARATION Boolean_t BTPSAPI BTPS_Query_Directory_Is_Root_Of_Drive(BTPSCONST char *Path);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_BTPS_Query_Directory_Is_Root_Of_Drive_t)(BTPSCONST char *Path);
#endif

   /*! @brief The following function is provided to allow a mechanism for the
    * caller to obtain File Information about a specific File or
    * directory.  @param Path The first parameter to this function specifies the
    * (path qualified) file or directory name to query the information
    * about.  @param FileInformation The second parameter specifies a buffer that will contain
    * the File/Directory information on return if this function is
    * successful.  @return This function returns a BOOLEAN TRUE if successful,
    * or FALSE if an error occurred.  Note that the final parameter is
    * *CANNOT* be NULL and must point to a valid buffer to store the
    * information into.  If this function returns success (TRUE) then
    * this buffer will contain the file information for the specified
    * input file/directory.  If this function returns an error (FALSE)
    * then the contents of the buffer pointed to by the final parameter
    * will be undefined.
    * \note The Path parameter should be specified in the UTF-8
    *          character set.
    */
BTPSAPI_DECLARATION Boolean_t BTPSAPI BTPS_Query_File_Information(BTPSCONST char *Path, BTPS_File_Information_t *FileInformation);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_BTPS_Query_File_Information_t)(BTPSCONST char *Path, BTPS_File_Information_t *FileInformation);
#endif

#endif
