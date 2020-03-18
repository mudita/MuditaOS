/*****< btpsfile.c >***********************************************************/
/*      Copyright 2008 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  BTPSFILE - Stonestreet One Bluetooth Stack File Access Abstraction        */
/*             Implementation.                                                */
/*                                                                            */
/*  Author:  David Wooldridge                                                 */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   03/05/08  D. Wooldridge  Initial implementation.                         */
/*   06/17/09  D. Lange       Updated to support new API type schema.         */
/******************************************************************************/
#include "BTPSFILE.h"      /* BTPS File I/O Prototypes/Constants.             */

   /* The following function is provided to allow a mechanism for the   */
   /* caller to create a directory.  The parameter to this function     */
   /* specifies the (path qualified) directory name to create.  This    */
   /* function returns one of the following:                            */
   /*    - BTPS_FILE_MAKE_DIRECTORY_SUCCESS                             */
   /*    - BTPS_FILE_MAKE_DIRECTORY_ERROR_ALREADY_EXISTS                */
   /*    - BTPS_FILE_MAKE_DIRECTORY_ERROR_UNABLE_TO_CREATE              */
   /* * NOTE * The Path parameter should be specified in the UTF-8      */
   /*          character set.                                           */
int BTPSAPI BTPS_Make_Directory(BTPSCONST char *Path)
{
}

   /* The following function is provided to allow a mechanism for the   */
   /* caller to delete a directory.  The parameter to this function     */
   /* specifies the (path qualified) directory name to delete.  This    */
   /* function returns BOOLEAN TRUE if successfull                      */
   /* * NOTE * The Path parameter should be specified in the UTF-8      */
   /*          character set.                                           */
Boolean_t BTPSAPI BTPS_Delete_Directory(BTPSCONST char *Path)
{
}
   /* The following function is provided to allow a mechanism for the   */
   /* caller to delete a specified file OR directory.  The parameter to */
   /* this function specifies the (path qualified) file or directory    */
   /* name to delete.  This function returns a BOOLEAN TRUE if          */
   /* successful, or FALSE if an error occurred.                        */
   /* * NOTE * The Path parameter should be specified in the UTF-8      */
   /*          character set.                                           */
Boolean_t BTPSAPI BTPS_Delete_File(BTPSCONST char *Path)
{
}

   /* The following function is provided to allow a mechanism for the   */
   /* caller to open a file (of the specified file name).  This first   */
   /* parameter to this function specifies the (path qualified) name of */
   /* the file to open.  The final parameter specifies the mode that    */
   /* the file is to be opened with.  The supported modes are:          */
   /*   - omReadOnly  - open for read only access                       */
   /*   - omWriteOnly - open for write only access                      */
   /*   - omReadWrite - open for read/write access                      */
   /*   - omCreate    - create new file (truncate if already exists)    */
   /*   - omAppend    - open for writing (create if doesn't exist)      */
   /* This function returns a NON-NULL File Descriptor if the specified */
   /* file was opened successfully, or NULL if there was an error.      */
   /* * NOTE * The Path parameter should be specified in the UTF-8      */
   /*          character set.                                           */
BTPS_File_Descriptor_t BTPSAPI BTPS_Open_File(BTPSCONST char *Path, BTPS_Open_Mode_t OpenMode)
{
}

   /* The following function is provided to allow a mechansim for the   */
   /* caller to close a file that was previously successfully opened via*/
   /* the BTPS_Open_File() function.  This function accepts as the first*/
   /* parameter the NON-NULL File Descriptor (successful return value   */
   /* from the BTPS_Open_File() function).                              */
void BTPSAPI BTPS_Close_File(BTPS_File_Descriptor_t FileDescriptor)
{
}

   /* The following function is provided to allow a mechansim for the   */
   /* caller to seek to specific byte offset within the specified file. */
   /* The first parameter specifies the file descriptor of the          */
   /* previously opened file (via the BTPS_Open_File() function).  The  */
   /* second parameter specifies the location to apply the Seek Offset  */
   /* to (final parameter).  The values that this parameter can be is   */
   /* one of the following:                                             */
   /*   - smBeginning - Apply SeekOffset from beginning of file         */
   /*   - smCurrent   - Apply SeekOffset from current location in file  */
   /*   - smEnd       - Apply SeekOffset from end of file               */
   /* The final parameter, SeekOffset, specifies the offset (in bytes)  */
   /* to seek (taking into account the Seek Mode type - second          */
   /* parameter).  This function will return BOOLEAN TRUE if successful */
   /* or a FALSE if there was an error.                                 */
Boolean_t BTPSAPI BTPS_Seek_File(BTPS_File_Descriptor_t FileDescriptor, BTPS_Seek_Mode_t SeekMode, SDWord_t SeekOffset)
{
}

   /* The following function is provided to allow a mechansim for the   */
   /* caller to read data from a previously opened file.  The first     */
   /* parameter specifies the file descriptor of the previously opened  */
   /* file (via the BTPS_Open_File() function).  The second parameter   */
   /* specifies the number of bytes to read from the file.  The third   */
   /* parameter specifies the Buffer to read the data into (must be     */
   /* (at least) as many bytes as specified by the second parameter).   */
   /* The final parameter is optional, and if specified, points to a    */
   /* buffer that will contain (on success) the number of bytes that    */
   /* were actually read and placed into the buffer.  This function     */
   /* returns BOOLEAN TRUE if the data was successfully read or a       */
   /* BOOLEAN FALSE if an error occurred.                               */
Boolean_t BTPSAPI BTPS_Read_File(BTPS_File_Descriptor_t FileDescriptor, DWord_t BufferSize, Byte_t *Buffer, DWord_t *BytesRead)
{
}

   /* The following function is provided to allow a mechansim for the   */
   /* caller to write data to a previously opened file.  The first      */
   /* parameter specifies the file descriptor of the previously opened  */
   /* file (via the BTPS_Open_File() function).  The second parameter   */
   /* specifies the number of bytes to write to the file.  The third    */
   /* parameter specifies the Buffer that contains the bytes to write to*/
   /* the file (must be (at least) as many bytes as specified by the    */
   /* second parameter).  The final parameter is optional, and if       */
   /* specified, points to a buffer that will contain (on success) the  */
   /* number of bytes that were actually written to the file.  This     */
   /* function returns BOOLEAN TRUE if the data was successfully read or*/
   /* a BOOLEAN FALSE if an error occurred.                             */
Boolean_t BTPSAPI BTPS_Write_File(BTPS_File_Descriptor_t FileDescriptor, DWord_t BufferSize, Byte_t *Buffer, DWord_t *BytesWritten)
{
}

   /* The following function is provided to allow a mechansim for the   */
   /* caller to begin the process of iterating through the directory    */
   /* specified by this function.  This is useful to iterate through all*/
   /* entries contained in the specified directory (see                 */
   /* BTPS_Get_Next_Directory_Entry() for more information).  The       */
   /* first parameter to this function specifies the (path qualified)   */
   /* directory name to open.  This function returns a NON-NULL         */
   /* directory descriptor if successful, or NULL if there as an error. */
   /* If this function returns success, then this return value can be   */
   /* passed to the BTPS_Get_Next_Directory_Entry() and the             */
   /* BTPS_Close_Directory() functions.                                 */
   /* * NOTE * The Path parameter should be specified in the UTF-8      */
   /*          character set.                                           */
BTPS_Directory_Descriptor_t BTPSAPI BTPS_Open_Directory(BTPSCONST char *Path)
{
}

   /* The following function is provided to allow a mechansim for the   */
   /* caller to close a directory that was previously successfully      */
   /* opened via the BTPS_Open_Directory() function.  This function     */
   /* accepts as the first parameter the NON-NULL Directory Descriptor  */
   /* (successful return value from the BTPS_Open_Directory() function).*/
void BTPSAPI BTPS_Close_Directory(BTPS_Directory_Descriptor_t DirectoryDescriptor)
{
}

   /* The following function is provided to allow a mechansim for the   */
   /* caller to interate through the directory entries contained in the */
   /* directory specified by the specified Directory Descriptor.  The   */
   /* first parameter specifies the Directory Descriptor (obtained from */
   /* a previously successful call to the BTP_Open_Directory() function)*/
   /* which specifies the directory to iterate through.  The final      */
   /* parameter specifies the last retrieved directory entry name that  */
   /* was returned from this function (to iterate to the next entry), or*/
   /* an empty string if this is the first time this function is being  */
   /* called for the specified directory.  If this function is          */
   /* successful, it will return a BOOLEAN TRUE value and the buffer    */
   /* pointed to by the final parmeter will contain the name of the next*/
   /* directory entry.  If this function returns FALSE then the contents*/
   /* of this buffer will be undefined (error occurred).                */
   /* * NOTE * The final parameter *MUST* point to a buffer that is     */
   /*          (at least) BTPS_MAXIMUM_FILE_NAME_LENGTH bytes long.     */
   /* * NOTE * The directory entry names returned from this function    */
   /*          are not path qualified (i.e. they do not contain the     */
   /*          path of the actual directory name in them).              */
   /* * NOTE * The final parameter should point to an empty string      */
   /*          (first character is a NULL character) to signify that the*/
   /*          first directory entry should be retrieved.  After this,  */
   /*          this parameter should specify the previously returned    */
   /*          directory entry name (to retrieve the next entry).       */
   /* * NOTE * The value that is returned in the final parameter will   */
   /*          be in the UTF-8 character set (NULL terminated).         */
Boolean_t BTPSAPI BTPS_Get_Next_Directory_Entry(BTPS_Directory_Descriptor_t DirectoryDescriptor, char *FileName)
{
}

   /* The following function is provided to allow a mechanism for the   */
   /* caller to determine if the specified Path, specifies the Root of  */
   /* a drive.  Examples of this function returning TRUE are:           */
   /*   - C:\ - TRUE on Windows based machines                          */
   /*   - /   - TRUE on Linux/UNIX based machines                       */
   /* This function returns a BOOLEAN TRUE if the specified Path        */
   /* specifies the root directory of a drive, or FALSE, otherwise.     */
   /* * NOTE * The Path parameter should be specified in the UTF-8      */
   /*          character set.                                           */
Boolean_t BTPSAPI BTPS_Query_Directory_Is_Root_Of_Drive(BTPSCONST char *Path)
{
}

   /* The following function is provided to allow a mechanism for the   */
   /* caller to obtain File Information about a specific File or        */
   /* directory.  The first parameter to this function specifies the    */
   /* (path qualified) file or directory name to query the information  */
   /* about.  The second parameter specifies a buffer that will contain */
   /* the File/Directory information on return if this function is      */
   /* successful.  This function returns a BOOLEAN TRUE if successful,  */
   /* or FALSE if an error occurred.  Note that the final parameter is  */
   /* *CANNOT* be NULL and must point to a valid buffer to store the    */
   /* information into.  If this function returns success (TRUE) then   */
   /* this buffer will contain the file information for the specified   */
   /* input file/directory.  If this function returns an error (FALSE)  */
   /* then the contents of the buffer pointed to by the final parameter */
   /* will be undefined.                                                */
   /* * NOTE * The Path parameter should be specified in the UTF-8      */
   /*          character set.                                           */
Boolean_t BTPSAPI BTPS_Query_File_Information(BTPSCONST char *Path, BTPS_File_Information_t *FileInformation)
{
}
