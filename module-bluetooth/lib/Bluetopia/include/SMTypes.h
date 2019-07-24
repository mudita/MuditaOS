/*****< smtypes.h >************************************************************/
/*      Copyright 2010 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*      Copyright 2015 Texas Instruments Incorporated.                        */
/*      All Rights Reserved.                                                  */
/*									      									  */	
/*  SMTYPES - Bluetooth LE Security Manager (SM) Type Definitions/Constants.  */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   10/18/10  T. Cook        Initial creation.                               */
/*   12/07/15  L. Gersi	      Updates for BT 4.2 - LE SC		      		  */
/******************************************************************************/
#ifndef __SMTYPESH__
#define __SMTYPESH__

#include "BTTypes.h"             /* Bluetooth Type Definitions.         */
#include "L2CAPTyp.h"            /* Bluetooth L2CAP Type Definitions.   */

   /* The following constant defines the L2CAP Fixed Channel ID that is */
   /* used for the Security Manager (SM).                               */
#define SM_SECURITY_MANAGER_FIXED_CHANNEL_ID                            (L2CAP_CHANNEL_IDENTIFIER_SECURITY_MANAGER_CHANNEL)

   /* The following enumerated type represents currently defined        */
   /* Keypress actions that can be specified with the authentication    */
   /* events.                                                           */
typedef enum
{
   LEkpEntryStarted,
   LEkpDigitEntered,
   LEkpDigitErased,
   LEkpCleared,
   LEkpEntryCompleted
} GAP_LE_Keypress_t;   


   /* The following type declaration represents the structure of a      */
   /* Security Manager public key value as defined by the Bluetooth     */
   /* Specification.                                                  	*/
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM__Public_Key_Value_t
{
   NonAlignedByte_t Public_Key_X_Value0;
   NonAlignedByte_t Public_Key_X_Value1;
   NonAlignedByte_t Public_Key_X_Value2;
   NonAlignedByte_t Public_Key_X_Value3;
   NonAlignedByte_t Public_Key_X_Value4;
   NonAlignedByte_t Public_Key_X_Value5;
   NonAlignedByte_t Public_Key_X_Value6;
   NonAlignedByte_t Public_Key_X_Value7;
   NonAlignedByte_t Public_Key_X_Value8;
   NonAlignedByte_t Public_Key_X_Value9;
   NonAlignedByte_t Public_Key_X_Value10;
   NonAlignedByte_t Public_Key_X_Value11;
   NonAlignedByte_t Public_Key_X_Value12;
   NonAlignedByte_t Public_Key_X_Value13;
   NonAlignedByte_t Public_Key_X_Value14;
   NonAlignedByte_t Public_Key_X_Value15;
   NonAlignedByte_t Public_Key_X_Value16;
   NonAlignedByte_t Public_Key_X_Value17;
   NonAlignedByte_t Public_Key_X_Value18;
   NonAlignedByte_t Public_Key_X_Value19;
   NonAlignedByte_t Public_Key_X_Value20;
   NonAlignedByte_t Public_Key_X_Value21;
   NonAlignedByte_t Public_Key_X_Value22;
   NonAlignedByte_t Public_Key_X_Value23;
   NonAlignedByte_t Public_Key_X_Value24;
   NonAlignedByte_t Public_Key_X_Value25;
   NonAlignedByte_t Public_Key_X_Value26;
   NonAlignedByte_t Public_Key_X_Value27;
   NonAlignedByte_t Public_Key_X_Value28;
   NonAlignedByte_t Public_Key_X_Value29;
   NonAlignedByte_t Public_Key_X_Value30;
   NonAlignedByte_t Public_Key_X_Value31;
   NonAlignedByte_t Public_Key_Y_Value0;
   NonAlignedByte_t Public_Key_Y_Value1;
   NonAlignedByte_t Public_Key_Y_Value2;
   NonAlignedByte_t Public_Key_Y_Value3;
   NonAlignedByte_t Public_Key_Y_Value4;
   NonAlignedByte_t Public_Key_Y_Value5;
   NonAlignedByte_t Public_Key_Y_Value6;
   NonAlignedByte_t Public_Key_Y_Value7;
   NonAlignedByte_t Public_Key_Y_Value8;
   NonAlignedByte_t Public_Key_Y_Value9;
   NonAlignedByte_t Public_Key_Y_Value10;
   NonAlignedByte_t Public_Key_Y_Value11;
   NonAlignedByte_t Public_Key_Y_Value12;
   NonAlignedByte_t Public_Key_Y_Value13;
   NonAlignedByte_t Public_Key_Y_Value14;
   NonAlignedByte_t Public_Key_Y_Value15;
   NonAlignedByte_t Public_Key_Y_Value16;
   NonAlignedByte_t Public_Key_Y_Value17;
   NonAlignedByte_t Public_Key_Y_Value18;
   NonAlignedByte_t Public_Key_Y_Value19;
   NonAlignedByte_t Public_Key_Y_Value20;
   NonAlignedByte_t Public_Key_Y_Value21;
   NonAlignedByte_t Public_Key_Y_Value22;
   NonAlignedByte_t Public_Key_Y_Value23;
   NonAlignedByte_t Public_Key_Y_Value24;
   NonAlignedByte_t Public_Key_Y_Value25;
   NonAlignedByte_t Public_Key_Y_Value26;
   NonAlignedByte_t Public_Key_Y_Value27;
   NonAlignedByte_t Public_Key_Y_Value28;
   NonAlignedByte_t Public_Key_Y_Value29;
   NonAlignedByte_t Public_Key_Y_Value30;
   NonAlignedByte_t Public_Key_Y_Value31;
} __PACKED_STRUCT_END__ SM_Public_Key_Value_t;

#define SM_PUBLIC_KEY_VALUE_SIZE                                           (sizeof(SM_Public_Key_Value_t))


   /* The following type declaration represents the structure of a      */
   /* Security Manager public key X coordinate value as defined by the 	*/
   /* Bluetooth Specification.                                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Public_Key_X_Coordinate_Value_t
{
   NonAlignedByte_t Public_Key_X_Value0;
   NonAlignedByte_t Public_Key_X_Value1;
   NonAlignedByte_t Public_Key_X_Value2;
   NonAlignedByte_t Public_Key_X_Value3;
   NonAlignedByte_t Public_Key_X_Value4;
   NonAlignedByte_t Public_Key_X_Value5;
   NonAlignedByte_t Public_Key_X_Value6;
   NonAlignedByte_t Public_Key_X_Value7;
   NonAlignedByte_t Public_Key_X_Value8;
   NonAlignedByte_t Public_Key_X_Value9;
   NonAlignedByte_t Public_Key_X_Value10;
   NonAlignedByte_t Public_Key_X_Value11;
   NonAlignedByte_t Public_Key_X_Value12;
   NonAlignedByte_t Public_Key_X_Value13;
   NonAlignedByte_t Public_Key_X_Value14;
   NonAlignedByte_t Public_Key_X_Value15;
   NonAlignedByte_t Public_Key_X_Value16;
   NonAlignedByte_t Public_Key_X_Value17;
   NonAlignedByte_t Public_Key_X_Value18;
   NonAlignedByte_t Public_Key_X_Value19;
   NonAlignedByte_t Public_Key_X_Value20;
   NonAlignedByte_t Public_Key_X_Value21;
   NonAlignedByte_t Public_Key_X_Value22;
   NonAlignedByte_t Public_Key_X_Value23;
   NonAlignedByte_t Public_Key_X_Value24;
   NonAlignedByte_t Public_Key_X_Value25;
   NonAlignedByte_t Public_Key_X_Value26;
   NonAlignedByte_t Public_Key_X_Value27;
   NonAlignedByte_t Public_Key_X_Value28;
   NonAlignedByte_t Public_Key_X_Value29;
   NonAlignedByte_t Public_Key_X_Value30;
   NonAlignedByte_t Public_Key_X_Value31;

} __PACKED_STRUCT_END__ SM_Public_Key_X_Coordinate_Value_t;

#define SM_PUBLIC_KEY_X_COORDINATE_VALUE_SIZE                           (sizeof(SM_Public_Key_X_Coordinate_Value_t))

   /* The following type declaration represents the structure of a      */
   /* Security Manager Mackey value which is part of the f5 function 	*/
   /* output as defined by the Bluetooth Specification for LE SC.       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_MacKey_Value_t
{
   NonAlignedByte_t MacKey_Value0;
   NonAlignedByte_t MacKey_Value1;
   NonAlignedByte_t MacKey_Value2;
   NonAlignedByte_t MacKey_Value3;
   NonAlignedByte_t MacKey_Value4;
   NonAlignedByte_t MacKey_Value5;
   NonAlignedByte_t MacKey_Value6;
   NonAlignedByte_t MacKey_Value7;
   NonAlignedByte_t MacKey_Value8;
   NonAlignedByte_t MacKey_Value9;
   NonAlignedByte_t MacKey_Value10;
   NonAlignedByte_t MacKey_Value11;
   NonAlignedByte_t MacKey_Value12;
   NonAlignedByte_t MacKey_Value13;
   NonAlignedByte_t MacKey_Value14;
   NonAlignedByte_t MacKey_Value15;
} __PACKED_STRUCT_END__ SM_MacKey_Value_t;

#define SM_MACKEY_VALUE_SIZE                                           (sizeof(SM_MacKey_Value_t))

   /* The following type declaration represents the structure of a      */
   /* Security Manager DHkey check value as defined by the Bluetooth    */
   /* Specification.                                                  	*/
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_DHKey_Check_Value_t
{
   NonAlignedByte_t DHKey_Check_Value0;
   NonAlignedByte_t DHKey_Check_Value1;
   NonAlignedByte_t DHKey_Check_Value2;
   NonAlignedByte_t DHKey_Check_Value3;
   NonAlignedByte_t DHKey_Check_Value4;
   NonAlignedByte_t DHKey_Check_Value5;
   NonAlignedByte_t DHKey_Check_Value6;
   NonAlignedByte_t DHKey_Check_Value7;
   NonAlignedByte_t DHKey_Check_Value8;
   NonAlignedByte_t DHKey_Check_Value9;
   NonAlignedByte_t DHKey_Check_Value10;
   NonAlignedByte_t DHKey_Check_Value11;
   NonAlignedByte_t DHKey_Check_Value12;
   NonAlignedByte_t DHKey_Check_Value13;
   NonAlignedByte_t DHKey_Check_Value14;
   NonAlignedByte_t DHKey_Check_Value15;
} __PACKED_STRUCT_END__ SM_DHKey_Check_Value_t;

#define SM_DHKEY_CHECK_VALUE_SIZE                                           (sizeof(SM_DHKey_Check_Value_t))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* comparison of two SM_DHKey_Check_Value_t variables.  This MACRO 	*/
   /* only returns whether or not the two SM_DHKey_Check_Value_t 	*/
   /* variables are equal (MACRO returns Boolean result) NOT less than  */
   /* or greater than.  The two parameters to this macro are of type    */
   /* SM_DHKey_Check_Value_t and represent the DHKey Check Values to 	*/
   /*compare.   							*/
#define SM_COMPARE_DHKEY_CHECK_VALUES(_x,_y)               (((_x).DHKey_Check_Value0  == (_y).DHKey_Check_Value0) && ((_x).DHKey_Check_Value1  == (_y).DHKey_Check_Value1) \
														&& ((_x).DHKey_Check_Value2  == (_y).DHKey_Check_Value2) && ((_x).DHKey_Check_Value3  == (_y).DHKey_Check_Value3)  \
														&& ((_x).DHKey_Check_Value4  == (_y).DHKey_Check_Value4) && ((_x).DHKey_Check_Value5  == (_y).DHKey_Check_Value5)  \
														&& ((_x).DHKey_Check_Value6  == (_y).DHKey_Check_Value6)  && ((_x).DHKey_Check_Value7  == (_y).DHKey_Check_Value7) \
														&& ((_x).DHKey_Check_Value8  == (_y).DHKey_Check_Value8) && ((_x).DHKey_Check_Value9  == (_y).DHKey_Check_Value9)  \
														&& ((_x).DHKey_Check_Value10 == (_y).DHKey_Check_Value10) && ((_x).DHKey_Check_Value11 == (_y).DHKey_Check_Value11)\
														&& ((_x).DHKey_Check_Value12 == (_y).DHKey_Check_Value12) && ((_x).DHKey_Check_Value13 == (_y).DHKey_Check_Value13)\
														&& ((_x).DHKey_Check_Value14 == (_y).DHKey_Check_Value14) && ((_x).DHKey_Check_Value15 == (_y).DHKey_Check_Value15))


   /* The following type declaration represents the structure of a      */
   /* Security Manager Confirm value as defined by the Bluetooth        */
   /* Specification.                                                    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Confirm_Value_t
{
   NonAlignedByte_t Confirm_Value0;
   NonAlignedByte_t Confirm_Value1;
   NonAlignedByte_t Confirm_Value2;
   NonAlignedByte_t Confirm_Value3;
   NonAlignedByte_t Confirm_Value4;
   NonAlignedByte_t Confirm_Value5;
   NonAlignedByte_t Confirm_Value6;
   NonAlignedByte_t Confirm_Value7;
   NonAlignedByte_t Confirm_Value8;
   NonAlignedByte_t Confirm_Value9;
   NonAlignedByte_t Confirm_Value10;
   NonAlignedByte_t Confirm_Value11;
   NonAlignedByte_t Confirm_Value12;
   NonAlignedByte_t Confirm_Value13;
   NonAlignedByte_t Confirm_Value14;
   NonAlignedByte_t Confirm_Value15;
} __PACKED_STRUCT_END__ SM_Confirm_Value_t;

#define SM_CONFIRM_VALUE_SIZE                                           (sizeof(SM_Confirm_Value_t))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* comparison of two SM_Confirm_Value_t variable.  This MACRO only   */
   /* returns whether or not the two SM_Confirm_Value_t variables are   */
   /* equal (MACRO returns Boolean result) NOT less than or greater     */
   /* than.  The two parameters to this macro are of type               */
   /* SM_Confirm_Value_t and represent the Confirm Values to compare.   */
#define SM_COMPARE_CONFIRM_VALUES(_x,_y)                                (((_x).Confirm_Value0  == (_y).Confirm_Value0) && ((_x).Confirm_Value1  == (_y).Confirm_Value1)  && ((_x).Confirm_Value2  == (_y).Confirm_Value2) && ((_x).Confirm_Value3  == (_y).Confirm_Value3) && ((_x).Confirm_Value4  == (_y).Confirm_Value4) && ((_x).Confirm_Value5  == (_y).Confirm_Value5) && ((_x).Confirm_Value6  == (_y).Confirm_Value6)  && ((_x).Confirm_Value7  == (_y).Confirm_Value7) && ((_x).Confirm_Value8  == (_y).Confirm_Value8) && ((_x).Confirm_Value9  == (_y).Confirm_Value9) && ((_x).Confirm_Value10 == (_y).Confirm_Value10) && ((_x).Confirm_Value11 == (_y).Confirm_Value11) && ((_x).Confirm_Value12 == (_y).Confirm_Value12) && ((_x).Confirm_Value13 == (_y).Confirm_Value13) && ((_x).Confirm_Value14 == (_y).Confirm_Value14) && ((_x).Confirm_Value15 == (_y).Confirm_Value15))

   /* The following type declaration represents the structure of a      */
   /* Security Manager (SM) Random Value as defined by the Bluetooth    */
   /* Specification.                                                    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Random_Value_t
{
   NonAlignedByte_t Random_Number0;
   NonAlignedByte_t Random_Number1;
   NonAlignedByte_t Random_Number2;
   NonAlignedByte_t Random_Number3;
   NonAlignedByte_t Random_Number4;
   NonAlignedByte_t Random_Number5;
   NonAlignedByte_t Random_Number6;
   NonAlignedByte_t Random_Number7;
   NonAlignedByte_t Random_Number8;
   NonAlignedByte_t Random_Number9;
   NonAlignedByte_t Random_Number10;
   NonAlignedByte_t Random_Number11;
   NonAlignedByte_t Random_Number12;
   NonAlignedByte_t Random_Number13;
   NonAlignedByte_t Random_Number14;
   NonAlignedByte_t Random_Number15;
} __PACKED_STRUCT_END__ SM_Random_Value_t;

#define SM_RANDOM_VALUE_SIZE                                            (sizeof(SM_Random_Value_t))

   /* Security Manager (SM) Protocol Data Unit Identifiers (PDU ID's).  */
#define SM_CODE_PAIRING_REQUEST                                         0x01
#define SM_CODE_PAIRING_RESPONSE                                        0x02
#define SM_CODE_PAIRING_CONFIRM                                         0x03
#define SM_CODE_PAIRING_RANDOM                                          0x04
#define SM_CODE_PAIRING_FAILED                                          0x05
#define SM_CODE_ENCRYPTION_INFORMATION                                  0x06
#define SM_CODE_MASTER_IDENTIFICATION                                   0x07
#define SM_CODE_IDENTITY_INFORMATION                                    0x08
#define SM_CODE_IDENTITY_ADDRESS_INFORMATION                            0x09
#define SM_CODE_SIGNING_INFORMATION                                     0x0A
#define SM_CODE_SECURITY_REQUEST                                        0x0B
#define SM_CODE_PAIRING_PUBLIC_KEY										0x0C
#define SM_CODE_PAIRING_DHKEY_CHECK										0x0D
#define SM_CODE_PAIRING_KEYPRESS_NOTIFICATION							0x0E


   /* Security Manager (SM) Protocol Data Unit Pairing Failed Reason    */
   /* Codes.                                                            */
#define SM_PAIRING_FAILED_REASON_PASSKEY_ENTRY_FAILED                   0x01
#define SM_PAIRING_FAILED_REASON_OOB_NOT_AVAILABLE                      0x02
#define SM_PAIRING_FAILED_REASON_AUTHENTICATION_REQUIREMENTS            0x03
#define SM_PAIRING_FAILED_REASON_CONFIRM_VALUE_FAILED                   0x04
#define SM_PAIRING_FAILED_REASON_PAIRING_NOT_SUPPORTED                  0x05
#define SM_PAIRING_FAILED_REASON_ENCRYPTION_KEY_SIZE                    0x06
#define SM_PAIRING_FAILED_REASON_COMMAND_NOT_SUPPORTED                  0x07
#define SM_PAIRING_FAILED_REASON_UNSPECIFIED_REASON                     0x08
#define SM_PAIRING_FAILED_REASON_REPEATED_ATTEMPTS                      0x09
#define SM_PAIRING_FAILED_REASON_INVALID_PARAMETERS                     0x0A
#define SM_PAIRING_FAILED_REASON_DHKEY_CHECK_FAILED                     0x0B
#define SM_PAIRING_FAILED_REASON_NUMERIC_COMPARISON_FAILED				0x0C


   /* The following type declaration represents the structure of the    */
   /* Header of an Security Manager (SM) Protocol Data Packet.  This    */
   /* Header Information is contained in Every Defined Security Manager */
   /* Protocol Packet.  This structure forms the basis of additional    */
   /* defined Security Manager Protocol Packets.  Since this structure  */
   /* is present at the begining of Every Defined Security Manager      */
   /* Protocol Packet, this structure will be the first element of Every*/
   /* Defined Security Manager Protocol Packet in this file.            */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Data_Unit_Header_t
{
   NonAlignedByte_t Code;
} __PACKED_STRUCT_END__ SM_Data_Unit_Header_t;

#define SM_PROTOCOL_DATA_UNIT_HEADER_SIZE                               (sizeof(SM_Data_Unit_Header_t))

   /* The following type declaration represents the structure of the    */
   /* Security Manager (SM) Pairing Request PDU.                        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Pairing_Request_t
{
   SM_Data_Unit_Header_t SM_Data_Unit_Header;
   NonAlignedByte_t      IO_Capability;
   NonAlignedByte_t      OOB_Data_Flag;
   NonAlignedByte_t      Auth_Req;
   NonAlignedByte_t      Maximimum_Encryption_Key_Size;
   NonAlignedByte_t      Initiator_Key_Distribution;
   NonAlignedByte_t      Responder_Key_Distribution;
} __PACKED_STRUCT_END__ SM_Pairing_Request_t;

#define SM_PAIRING_REQUEST_SIZE                                         (sizeof(SM_Pairing_Request_t))

   /* The following type declaration represents the structure of the    */
   /* Security Manager (SM) Pairing Response PDU.                       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Pairing_Response_t
{
   SM_Data_Unit_Header_t SM_Data_Unit_Header;
   NonAlignedByte_t      IO_Capability;
   NonAlignedByte_t      OOB_Data_Flag;
   NonAlignedByte_t      Auth_Req;
   NonAlignedByte_t      Maximimum_Encryption_Key_Size;
   NonAlignedByte_t      Initiator_Key_Distribution;
   NonAlignedByte_t      Responder_Key_Distribution;
} __PACKED_STRUCT_END__ SM_Pairing_Response_t;

#define SM_PAIRING_RESPONSE_SIZE                                        (sizeof(SM_Pairing_Response_t))

#if BTPS_CONFIGURATION_GAP_SUPPORT_LE_SECURE_CONNECTION
   /* The following type declaration represents the structure of the    */
   /* Security Manager (SM) Pairing Public key PDU.                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Pairing_Public_Key_t
{
   SM_Data_Unit_Header_t SM_Data_Unit_Header;
   SM_Public_Key_Value_t   Public_Key_Value;
} __PACKED_STRUCT_END__ SM_Pairing_Public_Key_t;

#define SM_PAIRING_PUBLIC_KEY_SIZE                                         (sizeof(SM_Pairing_Public_Key_t))

   /* The following type declaration represents the structure of the    */
   /* Security Manager (SM) Pairing Public key PDU.                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Pairing_DHKey_Check_t
{
   SM_Data_Unit_Header_t SM_Data_Unit_Header;
   SM_DHKey_Check_Value_t   DHKey_Check_Value;
} __PACKED_STRUCT_END__ SM_Pairing_DHKey_Check_t;

#define SM_PAIRING_DHKEY_CHECK_SIZE                                         (sizeof(SM_Pairing_DHKey_Check_t))

   /* The following type declaration represents the structure of the    */
   /* Security Manager (SM) Keypress Notification PDU.                  */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Keypress_Notification_t
{
   SM_Data_Unit_Header_t SM_Data_Unit_Header;
   GAP_LE_Keypress_t   Keypress_Notification_Type_Value;
} __PACKED_STRUCT_END__ SM_Keypress_Notification_t;

#define SM_KEYPRESS_NOTIFICATION_SIZE                                         (sizeof(SM_Keypress_Notification_t))
#endif

   /* The following type declaration represents the structure of the    */
   /* Security Manager (SM) Pairing Confirm PDU.                        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Pairing_Confirm_t
{
   SM_Data_Unit_Header_t SM_Data_Unit_Header;
   SM_Confirm_Value_t    Confirm_Value;
} __PACKED_STRUCT_END__ SM_Pairing_Confirm_t;

#define SM_PAIRING_CONFIRM_SIZE                                         (sizeof(SM_Pairing_Confirm_t))

   /* The following type declaration represents the structure of the    */
   /* Security Manager (SM) Pairing Random PDU.                         */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Pairing_Random_t
{
   SM_Data_Unit_Header_t SM_Data_Unit_Header;
   SM_Random_Value_t     Random_Value;
} __PACKED_STRUCT_END__ SM_Pairing_Random_t;

#define SM_PAIRING_RANDOM_SIZE                                          (sizeof(SM_Pairing_Random_t))

   /* The following type declaration represents the structure of the    */
   /* Security Manager (SM) Pairing Random PDU.                         */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Pairing_Failed_t
{
   SM_Data_Unit_Header_t SM_Data_Unit_Header;
   NonAlignedByte_t      Reason;
} __PACKED_STRUCT_END__ SM_Pairing_Failed_t;

#define SM_PAIRING_FAILED_SIZE                                          (sizeof(SM_Pairing_Failed_t))

   /* The following type declaration represents the structure of the    */
   /* Security Manager (SM) Encryption Information PDU.                 */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Encryption_Information_t
{
   SM_Data_Unit_Header_t SM_Data_Unit_Header;
   Long_Term_Key_t       Long_Term_Key;
} __PACKED_STRUCT_END__ SM_Encryption_Information_t;

#define SM_ENCRYPTION_INFORMATION_SIZE                                  (sizeof(SM_Encryption_Information_t))

   /* The following type declaration represents the structure of the    */
   /* Security Manager (SM) Master Identification PDU.                  */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Master_Identification_t
{
   SM_Data_Unit_Header_t SM_Data_Unit_Header;
   NonAlignedWord_t      EDIV;
   Random_Number_t       Rand;
} __PACKED_STRUCT_END__ SM_Master_Identification_t;

#define SM_MASTER_INDENTIFICATION_SIZE                                  (sizeof(SM_Master_Identification_t))

   /* The following type declaration represents the structure of the    */
   /* Security Manager (SM) Identity Information PDU.                   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Identity_Information_t
{
   SM_Data_Unit_Header_t SM_Data_Unit_Header;
   Encryption_Key_t      Identity_Resolving_Key;
} __PACKED_STRUCT_END__ SM_Identity_Information_t;

#define SM_IDENTITY_INFORMATION_SIZE                                    (sizeof(SM_Identity_Information_t))

   /* The following type declaration represents the structure of the    */
   /* Security Manager (SM) Identity Address Information PDU.           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Identity_Address_Information_t
{
   SM_Data_Unit_Header_t SM_Data_Unit_Header;
   NonAlignedByte_t      AddrType;
   BD_ADDR_t             BD_ADDR;
} __PACKED_STRUCT_END__ SM_Identity_Address_Information_t;

#define SM_IDENTITY_ADDRESS_INFORMATION_SIZE                            (sizeof(SM_Identity_Address_Information_t))

   /* The following type declaration represents the structure of the    */
   /* Security Manager (SM) Signing Information PDU.                    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Signing_Information_t
{
   SM_Data_Unit_Header_t SM_Data_Unit_Header;
   Encryption_Key_t      Signature_Key;
} __PACKED_STRUCT_END__ SM_Signing_Information_t;

#define SM_SIGNING_INFORMATION_SIZE                                     (sizeof(SM_Signing_Information_t))

   /* The following type declaration represents the structure of the    */
   /* Security Manager (SM) Security Request PDU.                       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_Security_Request_t
{
   SM_Data_Unit_Header_t SM_Data_Unit_Header;
   NonAlignedByte_t      Auth_Req;
} __PACKED_STRUCT_END__ SM_Security_Request_t;

#define SM_SECURITY_REQUEST_SIZE                                        (sizeof(SM_Security_Request_t))

   /* The following define the acceptable values in the IO Capability   */
   /* field of the Pairing Request and Pairing Response SMP Commands.   */
#define SM_IO_CAPABILITY_DISPLAY_ONLY                                   0x00
#define SM_IO_CAPABILITY_DISPLAY_YES_NO                                 0x01
#define SM_IO_CAPABILITY_DISPLAY_KEYBOARD_ONLY                          0x02
#define SM_IO_CAPABILITY_DISPLAY_NO_INPUT_NO_OUTPUT                     0x03
#define SM_IO_CAPABILITY_DISPLAY_KEYBOARD_DISPLAY                       0x04

   /* The following define the acceptable values in the OOB Data Flag   */
   /* Field of the Pairing Request and Pairing Response SMP Commands.   */
#define SM_OOB_DATA_FLAG_OOB_DATA_NOT_PRESENT                           0x00
#define SM_OOB_DATA_FLAG_OOB_DATA_PRESENT                               0x01

   /* The following Bit Numbers define the format of the Auth_Req field */
   /* accepted by some commands. (Pairing Request, Pairing Response,    */
   /* Security Request).                                                */
#define SM_AUTHENTICATION_REQUIREMENTS_BONDING_BIT_NUMBER               0x00
#define SM_AUTHENTICATION_REQUIREMENTS_MITM_BIT_NUMBER                  0x02
#if BTPS_CONFIGURATION_GAP_SUPPORT_LE_SECURE_CONNECTION
#define SM_AUTHENTICATION_REQUIREMENTS_SC_BIT_NUMBER                 	0x03
#define SM_AUTHENTICATION_REQUIREMENTS_KEYPRESS_BIT_NUMBER      		0x04
#endif
   /* The following defines represent the Bit Numbers for the Initiator */
   /* and Responder Key Distribution fields. These fields are included  */
   /* in both the Pairing Request and Pairing Response PDUs and         */
   /* represent the Keys that each side of a remote connection is       */
   /* requesting to send and receive.                                   */
#define SM_KEY_DISTRIBUTION_ENCRYPTION_KEY_BIT_NUMBER                   0x00
#define SM_KEY_DISTRIBUTION_IDENTITY_KEY_BIT_NUMBER                     0x01
#define SM_KEY_DISTRIBUTION_SIGNING_KEY_BIT_NUMBER                      0x02

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* assigned variables into the Auth_Req field that some PDUs have.   */
   /* (Pairing Request, Pairing Response, Security Request). This field */
   /* is a bit field. This assigns Bonding Requirements into this field.*/
   /* The first parameter to this MACRO should be a Byte_t variable.    */
#define SM_ASSIGN_BONDING_AUTHENTICATION_REQUIREMENTS(_x) \
   (((Byte_t *)(_x))[0]) |= ((0x01) << (SM_AUTHENTICATION_REQUIREMENTS_BONDING_BIT_NUMBER % (BYTE_SIZE*8)))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* assigned variables into the Auth_Req field that some PDUs have.   */
   /* (Pairing Request, Pairing Response, Security Request). This field */
   /* is a bit field. This assigns No Bonding Requirements into this    */
   /* field. The first parameter to this MACRO should be a Byte_t       */
   /* variable.                                                         */
#define SM_ASSIGN_NO_BONDING_AUTHENTICATION_REQUIREMENTS(_x) \
   (((Byte_t *)(_x))[0]) &= (~((0x11) << (SM_AUTHENTICATION_REQUIREMENTS_BONDING_BIT_NUMBER % (BYTE_SIZE*8))))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* testing if the Bonding Bits are set in the Auth_Req fields.       */
   /* This MACRO returns TRUE if Bonding is requested or FALSE if it is */
   /* not.                                                              */
#define SM_TEST_BONDING_AUTHENTICATION_REQUIREMENTS(_x) \
   ((((Byte_t *)(_x))[0]) & ((0x01) << (SM_AUTHENTICATION_REQUIREMENTS_BONDING_BIT_NUMBER % (BYTE_SIZE*8))))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* assigned variables into the Auth_Req field that some PDUs have.   */
   /* (Pairing Request, Pairing Response, Security Request). This field */
   /* is a bit field. This assigns MITM protection  into this field.    */
   /* The first parameter to this MACRO should be a Byte_t variable.    */
#define SM_ASSIGN_MITM_AUTHENTICATION_REQUIREMENTS(_x) \
   (((Byte_t *)(_x))[0]) |= ((0x1) << (SM_AUTHENTICATION_REQUIREMENTS_MITM_BIT_NUMBER % (BYTE_SIZE*8)))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* assigned variables into the Auth_Req field that some PDUs have.   */
   /* (Pairing Request, Pairing Response, Security Request). This field */
   /* is a bit field. This assigns No MITM protection  into this field. */
   /* The first parameter to this MACRO should be a Byte_t variable.    */
#define SM_ASSIGN_NO_MITM_AUTHENTICATION_REQUIREMENTS(_x) \
   (((Byte_t *)(_x))[0]) &= ~((0x1) << (SM_AUTHENTICATION_REQUIREMENTS_MITM_BIT_NUMBER % (BYTE_SIZE*8)))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* testing if the MITM bit is set in the Auth_Req field. This MACRO  */
   /* returns TRUE if MITM is set or FALSE otherwise.                   */
#define SM_TEST_MITM_AUTHENTICATION_REQUIREMENTS(_x) \
   ((((Byte_t *)(_x))[0]) & ((0x1) << (SM_AUTHENTICATION_REQUIREMENTS_MITM_BIT_NUMBER % (BYTE_SIZE*8))))

#if BTPS_CONFIGURATION_GAP_SUPPORT_LE_SECURE_CONNECTION
   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* assigned variables into the Auth_Req field that some PDUs have.   */
   /* (Pairing Request, Pairing Response, Security Request). This field */
   /* is a bit field.This assigns SC - Secure Connection into this field*/
   /* The first parameter to this MACRO should be a Byte_t variable.    */
#define SM_ASSIGN_SC_AUTHENTICATION_REQUIREMENTS(_x) \
   (((Byte_t *)(_x))[0]) |= ((0x01) << (SM_AUTHENTICATION_REQUIREMENTS_SC_BIT_NUMBER % (BYTE_SIZE*8)))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* assigned variables into the Auth_Req field that some PDUs have.   */
   /* (Pairing Request, Pairing Response, Security Request). This field */
   /*is a bit field.This assigns NO SC-Secure Connection into this field*/
   /* The first parameter to this MACRO should be a Byte_t variable	*/
#define SM_ASSIGN_NO_SC_AUTHENTICATION_REQUIREMENTS(_x) \
   (((Byte_t *)(_x))[0]) &= ~((0x1) << (SM_AUTHENTICATION_REQUIREMENTS_SC_BIT_NUMBER % (BYTE_SIZE*8)))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* testing if the SC bit is set in the Auth_Req field. This MACRO    */
   /* returns TRUE if SC is set or FALSE otherwise.                     */
#define SM_TEST_SC_AUTHENTICATION_REQUIREMENTS(_x) \
   ((((Byte_t *)(_x))[0]) & ((0x1) << (SM_AUTHENTICATION_REQUIREMENTS_SC_BIT_NUMBER % (BYTE_SIZE*8))))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* assigned variables into the Auth_Req field that some PDUs have.   */
   /* (Pairing Request, Pairing Response, Security Request). This field */
   /*is a bit field. This assigns keypress notifications into this field*/
   /* The first parameter to this MACRO should be a Byte_t variable.    */
#define SM_ASSIGN_KEYPRESS_AUTHENTICATION_REQUIREMENTS(_x) \
   (((Byte_t *)(_x))[0]) |= ((0x01) << (SM_AUTHENTICATION_REQUIREMENTS_KEYPRESS_BIT_NUMBER % (BYTE_SIZE*8)))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* assigned variables into the Auth_Req field that some PDUs have.   */
   /* (Pairing Request, Pairing Response, Security Request). This field */
   /* is a bit field. This assigns NO keypress notifications into this  */
   /*field.The first parameter to this MACRO should be a Byte_t variable*/
#define SM_ASSIGN_NO_KEYPRESS_AUTHENTICATION_REQUIREMENTS(_x) \
   (((Byte_t *)(_x))[0]) &= ~((0x1) << (SM_AUTHENTICATION_REQUIREMENTS_KEYPRESS_BIT_NUMBER % (BYTE_SIZE*8)))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* testing if the keypress bit is set in the Auth_Req field. This    */
   /* MACRO returns TRUE if keypress is set or FALSE otherwise.         */
#define SM_TEST_KEYPRESS_AUTHENTICATION_REQUIREMENTS(_x) \
   ((((Byte_t *)(_x))[0]) & ((0x1) << (SM_AUTHENTICATION_REQUIREMENTS_KEYPRESS_BIT_NUMBER % (BYTE_SIZE*8))))
#endif

   /* The following MACRO is a utiltity MACRO that exists to aid code   */
   /* readability in assigning into the Initiator and Responder Key     */
   /* Distribution Fields.  The following MACRO is used to set a bit.   */
   /* The first parameter to this MACRO is a Byte_t variable that the   */
   /* bit will be SET in.  The next is the bit to set (should be of the */
   /* form SM_KEY_DISTRIBUTION_xxx_KEY_BIT_NUMBER where 'xxx' is the Key*/
   /* Bit Position to set.                                              */
#define SM_SET_KEY_DISTRIBUTION_BIT(_x,_y)  \
   ((Byte_t *)(_x))[0] |= (Byte_t)(1 << ((_y)%(BYTE_SIZE*8)))

   /* The following MACRO is a utiltity MACRO that exists to aid code   */
   /* readability in assigning into the Initiator and Responder Key     */
   /* Distribution Fields.  The following MACRO is used to clear a bit. */
   /* The first parameter to this MACRO is a Byte_t variable that the   */
   /* bit will be CLEARED in.  The next is the bit to set (should be of */
   /* the form SM_KEY_DISTRIBUTION_xxx_KEY_BIT_NUMBER where 'xxx' is the*/
   /* Key Bit Position to set.                                          */
#define SM_RESET_KEY_DISTRIBUTION_BIT(_x,_y)  \
   ((Byte_t *)(_x))[0] &= (Byte_t)~(1 << ((_y)%(BYTE_SIZE*8)))

   /* The following MACRO is a utiltity MACRO that exists to aid code   */
   /* readability in assigning into the Initiator and Responder Key     */
   /* Distribution Fields. The following MACRO is used to test a bit.   */
   /* The first parameter to this MACRO is a Byte_t variable that       */
   /* the bit will be tested. The next is the bit to be tested (should  */
   /* be of the form SM_KEY_DISTRIBUTION_xxx_KEY_BIT_NUMBER where 'xxx' */
   /* is the Key Bit Position to set.                                   */
#define SM_TEST_KEY_DISTRIBUTION_BIT(_x, _y)  \
   ((((Byte_t *)(_x))[0]) & (Byte_t)(1 << ((_y)%(BYTE_SIZE*8))))

   /* The following structure defines the content of a Security Manager */
   /* (SM) Message Authentication Code (MAC) which is defined to be 64  */
   /* bits in Length.                                                   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSM_MAC_t
{
  NonAlignedByte_t MAC0;
  NonAlignedByte_t MAC1;
  NonAlignedByte_t MAC2;
  NonAlignedByte_t MAC3;
  NonAlignedByte_t MAC4;
  NonAlignedByte_t MAC5;
  NonAlignedByte_t MAC6;
  NonAlignedByte_t MAC7;
} __PACKED_STRUCT_END__ SM_MAC_t;

#define SM_MAC_DATA_SIZE                                                (sizeof(SM_MAC_t))

#endif
