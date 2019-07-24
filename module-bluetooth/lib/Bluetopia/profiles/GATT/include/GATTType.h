/*****< gatttype.h >***********************************************************/
/*      Copyright 2010 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*      Copyright 2016 Texas Instruments Incorporated.                        */
/*      All Rights Reserved.                                                  */
/*									      									  */	
/*  GATTTYPE - Stonestreet One Bluetooth Stack Generic Attribute (GATT)       */
/*             Profile Type Definitions/Constants.                            */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname       Description of Modification                  */
/*   --------  -----------       ---------------------------------------------*/
/*   10/19/10  T. Cook           Initial Creation.                            */
/*   04/20/16  L. Gersi	         Adding macro for UUID_32		      		  */
/******************************************************************************/
#ifndef __GATTTYPEH__
#define __GATTTYPEH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */

   /* UUID Comparison Utility Macros.                                   */

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two UUID_16_t variables.  This MACRO only returns   */
   /* whether the two UUID_16_t variables are equal (MACRO returns      */
   /* boolean result) NOT less than/greater than.  The first parameter  */
   /* the the UUID_16_t variable. The next two are the individual byte  */
   /* values to do the comparison with. The bytes are NOT in Little     */
   /* Endian Format.                                                    */
#define COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT(_x, _a, _b) \
(                                                         \
   ((_x).UUID_Byte1 == (_a)) && ((_x).UUID_Byte0 == (_b)) \
)

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two UUID_32_t variables.  This MACRO only returns   */
   /* whether the two UUID_32_t variables are equal (MACRO returns      */
   /* boolean result) NOT less than/greater than.  The first parameter  */
   /* the the UUID_32_t variable. The next two are the individual byte  */
   /* values to do the comparison with. The bytes are NOT in Little     */
   /* Endian Format.                                                    */
#define COMPARE_BLUETOOTH_UUID_32_TO_CONSTANT(_x, _a, _b, _c, _d) \
(                                                                 \
   ((_x).UUID_Byte0 == (_d)) && ((_x).UUID_Byte1 == (_c)) &&      \
   ((_x).UUID_Byte2 == (_b)) && ((_x).UUID_Byte3 == (_a))         \
)

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two UUID_128_t variables.  This MACRO only returns  */
   /* whether the two UUID_128_t variables are equal (MACRO returns     */
   /* boolean result) NOT less than/greater than.  The first parameter  */
   /* this MACRO is of type UUID_128_t represents the UUID_128_t        */
   /* variable to compare.  The next parameters are the constant UUID   */
   /* bytes to do the comparison with.  The individual bytes are NOT in */
   /* Little Endian Format.                                             */
#define COMPARE_BLUETOOTH_UUID_128_TO_CONSTANT(_x, _a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n, _o, _p) \
(                                                                                                                  \
   ((_x).UUID_Byte0  == (_p)) && ((_x).UUID_Byte1  == (_o)) && ((_x).UUID_Byte2  == (_n)) &&                       \
   ((_x).UUID_Byte3  == (_m)) && ((_x).UUID_Byte4  == (_l)) && ((_x).UUID_Byte5  == (_k)) &&                       \
   ((_x).UUID_Byte6  == (_j)) && ((_x).UUID_Byte7  == (_i)) && ((_x).UUID_Byte8  == (_h)) &&                       \
   ((_x).UUID_Byte9  == (_g)) && ((_x).UUID_Byte10 == (_f)) && ((_x).UUID_Byte11 == (_e)) &&                       \
   ((_x).UUID_Byte12 == (_d)) && ((_x).UUID_Byte13 == (_c)) && ((_x).UUID_Byte14 == (_b)) &&                       \
   ((_x).UUID_Byte15 == (_a))                                                                                      \
)

   /* GATT BASE_UUID.                                                   */

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Bluetooth Base Universally Unique Identifier (BASE_UUID) to the   */
   /* specified UUID_128_t variable.  This MACRO accepts one parameter  */
   /* which is the UUID_128_t variable that is receive the BASE_UUID    */
   /* Constant value.                                                   */
#define GATT_ASSIGN_BASE_UUID(_x)                                                ASSIGN_BLUETOOTH_UUID_128((_x), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* GATT Utility Macros.                                              */

   /* The following MACRO is a utility MACRO that exists to compare a   */
   /* UUID_16_t type constant to a UUID_128_t variable. The first       */
   /* is the UUID_128_t variable to do the comparison with. The next    */
   /* parameters are the individual bytes of the UUID_16_t constant to  */
   /* compare to the UUID_128_t variable. This MACRO only returns       */
   /* whether or not the two UUIDs are equal (MACRO returns Boolean     */
   /* result) NOT less than/greater than.                               */
#define COMPARE_BLUETOOTH_UUID_128_TO_UUID_16_CONSTANT(_x, _a, _b)               COMPARE_BLUETOOTH_UUID_128_TO_CONSTANT((_x), 0x00, 0x00, _a, _b, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following MACRO is a utility MACRO that exists to compare a   */
   /* UUID_16_t variable to a UUID_128_t variable. The first is the     */
   /* UUID_128_t variable to do the comparison with. The next parameter */
   /* is the UUID_16_t variable. This MACRO only returns whether or not */
   /* the two UUIDs are equal (MACRO returns Boolean result) NOT less   */
   /* than/greater than.                                                */
#define COMPARE_BLUETOOTH_UUID_128_TO_BLUETOOTH_UUID_16(_x, _y)                  COMPARE_BLUETOOTH_UUID_128_TO_CONSTANT((_x), 0x00, 0x00, (_y).UUID_Byte1, (_y).UUID_Byte0, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following MACRO is a utility MACRO that exists to compare a   */
   /* UUID_32_t type constant to a UUID_128_t variable. The first       */
   /* is the UUID_128_t variable to do the comparison with. The next    */
   /* parameters are the individual bytes of the UUID_32_t constant to  */
   /* compare to the UUID_128_t variable. This MACRO only returns       */
   /* whether or not the two UUIDs are equal (MACRO returns Boolean     */
   /* result) NOT less than/greater than.                               */
#define COMPARE_BLUETOOTH_UUID_128_TO_UUID_32_CONSTANT(_x, _a, _b, _c, _d)       COMPARE_BLUETOOTH_UUID_128_TO_CONSTANT((_x), _a, _b, _c, _d, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following MACRO is a utility MACRO that exists to compare a   */
   /* UUID_32_t variable to a UUID_128_t variable. The first is the     */
   /* UUID_128_t variable to do the comparison with. The next parameter */
   /* is the UUID_32_t variable. This MACRO only returns whether or not */
   /* the two UUIDs are equal (MACRO returns Boolean result) NOT less   */
   /* than/greater than.                                                */
#define COMPARE_BLUETOOTH_UUID_128_TO_BLUETOOTH_UUID_32(_x, _y)                  COMPARE_BLUETOOTH_UUID_128_TO_CONSTANT((_x), (_y).UUID_Byte3, (_y).UUID_Byte2, (_y).UUID_Byte1, (_y).UUID_Byte0, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following MACRO is a utility MACRO that exists to compare a   */
   /* UUID_128_t variable to the Bluetooth Base UUID.  The first is the */
   /* UUID_128_t variable to do the comparison with.  This MACRO only   */
   /* returns whether or not the two UUIDs are equal (MACRO returns     */
   /* Boolean result) NOT less than/greater than.                       */
#define COMPARE_BLUETOOTH_UUID_TO_BLUETOOTH_BASE_UUID(_x)                        COMPARE_BLUETOOTH_UUID_128_TO_CONSTANT((_x), (_x).UUID_Byte15, (_x).UUID_Byte14, (_x).UUID_Byte13, (_x).UUID_Byte12, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* GATT Attribute Types.                                             */

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Profile Primary Service Attribute Type.  This MACRO accepts one   */
   /* parameter which is the UUID_16_t variable that is to receive the  */
   /* ATTRIBUTE_PROTOCOL_UUID_16 Constant value. This MACRO assigns the */
   /* GATT Service UUID 16.                                             */
#define GATT_ASSIGN_PRIMARY_SERVICE_ATTRIBUTE_TYPE(_x)                           ASSIGN_BLUETOOTH_UUID_16((_x), 0x28, 0x00)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Primary Service UUID in UUID16 form.*/
   /* This MACRO only returns whether the UUID_16_t variable is equal   */
   /* to the Primary Service UUID (MACRO returns boolean result) NOT    */
   /* less than/greater than. The first parameter is the UUID_16_t      */
   /* variable to compare to the GATT Primary Service UUID.             */
#define GATT_COMPARE_PRIMARY_SERVICE_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_16(_x)     COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x28, 0x00)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Primary Service UUID in UUID16 form.*/
   /* This MACRO only returns whether the UUID_16_t variable is equal   */
   /* to the Primary Service UUID (MACRO returns boolean result) NOT    */
   /* less than/greater than. The first parameter is the UUID_16_t      */
   /* variable to compare to the GATT Primary Service UUID.             */
#define GATT_COMPARE_PRIMARY_SERVICE_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_128(_x)    COMPARE_BLUETOOTH_UUID_128_TO_UUID_16_CONSTANT((_x), 0x28, 0x00)

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Profile Secondary Service Attribute Type.  This MACRO accepts one */
   /* parameter which is the UUID_16_t variable that is to receive the  */
   /* ATTRIBUTE_PROTOCOL_UUID_16 Constant value. This MACRO assigns the */
   /* GATT Service UUID 16.                                             */
#define GATT_ASSIGN_SECONDARY_SERVICE_ATTRIBUTE_TYPE(_x)                         ASSIGN_BLUETOOTH_UUID_16((_x), 0x28, 0x01)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Secondary Service UUID in UUID16    */
   /* form. This MACRO only returns whether the UUID_16_t variable is   */
   /* equal to the Primary Service UUID (MACRO returns boolean result)  */
   /* NOT less than/greater than. The first parameter is the UUID_16_t  */
   /* variable to compare to the GATT Primary Service UUID.             */
#define GATT_COMPARE_SECONDARY_SERVICE_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_16(_x)   COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x28, 0x01)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Secondary Service UUID in UUID16    */
   /* form. This MACRO only returns whether the UUID_16_t variable is   */
   /* equal to the Primary Service UUID (MACRO returns boolean result)  */
   /* NOT less than/greater than. The first parameter is the UUID_16_t  */
   /* variable to compare to the GATT Primary Service UUID.             */
#define GATT_COMPARE_SECONDARY_SERVICE_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_128(_x)  COMPARE_BLUETOOTH_UUID_128_TO_UUID_16_CONSTANT((_x), 0x28, 0x01)

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Profile Include Attribute Type.  This MACRO accepts one           */
   /* parameter which is the UUID_16_t variable that is to receive the  */
   /* ATTRIBUTE_PROTOCOL_UUID_16 Constant value. This MACRO assigns the */
   /* GATT Service UUID 16.                                             */
#define GATT_ASSIGN_INCLUDE_ATTRIBUTE_TYPE(_x)                                   ASSIGN_BLUETOOTH_UUID_16((_x), 0x28, 0x02)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Include UUID in UUID16              */
   /* form. This MACRO only returns whether the UUID_16_t variable is   */
   /* equal to the Primary Service UUID (MACRO returns boolean result)  */
   /* NOT less than/greater than. The first parameter is the UUID_16_t  */
   /* variable to compare to the GATT Primary Service UUID.             */
#define GATT_COMPARE_INCLUDE_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_16(_x)             COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x28, 0x02)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Include UUID in UUID16              */
   /* form. This MACRO only returns whether the UUID_16_t variable is   */
   /* equal to the Primary Service UUID (MACRO returns boolean result)  */
   /* NOT less than/greater than. The first parameter is the UUID_16_t  */
   /* variable to compare to the GATT Primary Service UUID.             */
#define GATT_COMPARE_INCLUDE_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_128(_x)            COMPARE_BLUETOOTH_UUID_128_TO_UUID_16_CONSTANT((_x), 0x28, 0x02)

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Profile Characteristic Attribute Type.  This MACRO accepts one    */
   /* parameter which is the UUID_16_t variable that is to receive the  */
   /* ATTRIBUTE_PROTOCOL_UUID_16 Constant value. This MACRO assigns the */
   /* GATT Service UUID 16.                                             */
#define GATT_ASSIGN_CHARACTERISTIC_ATTRIBUTE_TYPE(_x)                            ASSIGN_BLUETOOTH_UUID_16((_x), 0x28, 0x03)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Characteristic UUID in UUID16       */
   /* form. This MACRO only returns whether the UUID_16_t variable is   */
   /* equal to the Primary Service UUID (MACRO returns boolean result)  */
   /* NOT less than/greater than. The first parameter is the UUID_16_t  */
   /* variable to compare to the GATT Primary Service UUID.             */
#define GATT_COMPARE_CHARACTERISTIC_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_16(_x)      COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x28, 0x03)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Characteristic UUID in UUID16       */
   /* form. This MACRO only returns whether the UUID_16_t variable is   */
   /* equal to the Primary Service UUID (MACRO returns boolean result)  */
   /* NOT less than/greater than. The first parameter is the UUID_16_t  */
   /* variable to compare to the GATT Primary Service UUID.             */
#define GATT_COMPARE_CHARACTERISTIC_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_128(_x)     COMPARE_BLUETOOTH_UUID_128_TO_UUID_16_CONSTANT((_x), 0x28, 0x03)

   /* GATT Characteristic Descriptor Attribute Types.                   */

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Profile Characteristic Extended Properties Attribute Type.  This  */
   /* MACRO accepts one parameter which is the UUID_16_t variable that  */
   /* is to receive the ATTRIBUTE_PROTOCOL_UUID_16 Constant value. This */
   /* MACRO assigns the GATT Service UUID 16.                           */
#define GATT_ASSIGN_CHARACTERISTIC_EXTENDED_PROPERTIES_ATTRIBUTE_TYPE(_x)        ASSIGN_BLUETOOTH_UUID_16((_x), 0x29, 0x00)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Characteristic Extended Properties  */
   /* UUID in UUID16 form. This MACRO only returns whether the          */
   /* UUID_16_t variable is equal to the Primary Service UUID (MACRO    */
   /* returns boolean result) NOT less than/greater than. The first     */
   /* parameter is the UUID_16_t variable to compare to the GATT        */
   /* Primary Service UUID.                                             */
#define GATT_COMPARE_CHARACTERISTIC_EXTENDED_PROPERTIES_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_16(_x)    COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x29, 0x00)

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Profile Characteristic User Description Attribute Type.  This     */
   /* MACRO accepts one parameter which is the UUID_16_t variable that  */
   /* is to receive the ATTRIBUTE_PROTOCOL_UUID_16 Constant value. This */
   /* MACRO assigns the GATT Service UUID 16.                           */
#define GATT_ASSIGN_CHARACTERISTIC_USER_DESCRIPTION_ATTRIBUTE_TYPE(_x)           ASSIGN_BLUETOOTH_UUID_16((_x), 0x29, 0x01)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Characteristic User Description     */
   /* UUID in UUID16 form. This MACRO only returns whether the          */
   /* UUID_16_t variable is equal to the Primary Service UUID (MACRO    */
   /* returns boolean result) NOT less than/greater than. The first     */
   /* parameter is the UUID_16_t variable to compare to the GATT        */
   /* Primary Service UUID.                                             */
#define GATT_COMPARE_CHARACTERISTIC_USER_DESCRIPTION_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_16(_x)       COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x29, 0x01)

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Profile Client Characteristic Configuration Attribute Type. This  */
   /* MACRO accepts one parameter which is the UUID_16_t variable that  */
   /* is to receive the ATTRIBUTE_PROTOCOL_UUID_16 Constant value. This */
   /* MACRO assigns the GATT Service UUID 16.                           */
#define GATT_ASSIGN_CLIENT_CHARACTERISTIC_CONFIGURATION_ATTRIBUTE_TYPE(_x)       ASSIGN_BLUETOOTH_UUID_16((_x), 0x29, 0x02)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Client Characteristic Configuration */
   /* UUID in UUID16 form. This MACRO only returns whether the          */
   /* UUID_16_t variable is equal to the Primary Service UUID (MACRO    */
   /* returns boolean result) NOT less than/greater than. The first     */
   /* parameter is the UUID_16_t variable to compare to the GATT        */
   /* Primary Service UUID.                                             */
#define GATT_COMPARE_CLIENT_CHARACTERISTIC_CONFIGURATION_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_16(_x)   COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x29, 0x02)

   /* The following defines the GATT Client Characteristic Configuration*/
   /* UUID that is used when building the service tables.               */
#define GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_BLUETOOTH_UUID_CONSTANT         { 0x02, 0x29 }

   /* The following defines the valid length of the GATT Client         */
   /* Characteristic Configuration descriptor.                          */
#define GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH  (NON_ALIGNED_WORD_SIZE)

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Profile Server Characteristic Configuration Attribute Type. This  */
   /* MACRO accepts one parameter which is the UUID_16_t variable that  */
   /* is to receive the ATTRIBUTE_PROTOCOL_UUID_16 Constant value. This */
   /* MACRO assigns the GATT Service UUID 16.                           */
#define GATT_ASSIGN_SERVER_CHARACTERISTIC_CONFIGURATION_ATTRIBUTE_TYPE(_x)       ASSIGN_BLUETOOTH_UUID_16((_x), 0x29, 0x03)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Server Characteristic Configuration */
   /* UUID in UUID16 form. This MACRO only returns whether the          */
   /* UUID_16_t variable is equal to the Primary Service UUID (MACRO    */
   /* returns boolean result) NOT less than/greater than. The first     */
   /* parameter is the UUID_16_t variable to compare to the GATT        */
   /* Primary Service UUID.                                             */
#define GATT_COMPARE_SERVER_CHARACTERISTIC_CONFIGURATION_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_16(_x)   COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x29, 0x03)

   /* The following defines the GATT Server Characteristic Configuration*/
   /* UUID that is used when building the service tables.               */
#define GATT_SERVER_CHARACTERISTIC_CONFIGURATION_BLUETOOTH_UUID_CONSTANT         { 0x03, 0x29 }

   /* The following defines the valid length of the GATT Server         */
   /* Characteristic Configuration descriptor.                          */
#define GATT_SERVER_CHARACTERISTIC_CONFIGURATION_LENGTH  (NON_ALIGNED_WORD_SIZE)

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Profile Characteristic Format Attribute Type. This                */
   /* MACRO accepts one parameter which is the UUID_16_t variable that  */
   /* is to receive the ATTRIBUTE_PROTOCOL_UUID_16 Constant value. This */
   /* MACRO assigns the GATT Service UUID 16.                           */
#define GATT_ASSIGN_CHARACTERISTIC_PRESENTATION_FORMAT_ATTRIBUTE_TYPE(_x) ASSIGN_BLUETOOTH_UUID_16((_x), 0x29, 0x04)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Characteristic Format UUID          */
   /* UUID in UUID16 form. This MACRO only returns whether the          */
   /* UUID_16_t variable is equal to the Primary Service UUID (MACRO    */
   /* returns boolean result) NOT less than/greater than. The first     */
   /* parameter is the UUID_16_t variable to compare to the GATT        */
   /* Primary Service UUID.                                             */
#define GATT_COMPARE_CHARACTERISTIC_PRESENTATION_FORMAT_ATTRIBUTE_TYPE_TO_UUID_16(_x) COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x29, 0x04)

   /* The following defines the GATT Characteristic Presentation Format */
   /* Descriptor UUID that is used when building the service tables.    */
   /* * NOTE * The UUID MUST be assigned in Little-Endian format here.  */
#define GATT_CHARACTERISTIC_PRESENTATION_FORMAT_BLUETOOTH_UUID_CONSTANT { 0x04, 0x29 }

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Profile Characteristic Aggregate Format Attribute Type. This      */
   /* MACRO accepts one parameter which is the UUID_16_t variable that  */
   /* is to receive the ATTRIBUTE_PROTOCOL_UUID_16 Constant value. This */
   /* MACRO assigns the GATT Service UUID 16.                           */
#define GATT_ASSIGN_CHARACTERISTIC_AGGREGATE_FORMAT_ATTRIBUTE_TYPE(_x)           ASSIGN_BLUETOOTH_UUID_16((_x), 0x29, 0x05)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Characteristic Aggregate Format UUID*/
   /* UUID in UUID16 form. This MACRO only returns whether the          */
   /* UUID_16_t variable is equal to the Primary Service UUID (MACRO    */
   /* returns boolean result) NOT less than/greater than. The first     */
   /* parameter is the UUID_16_t variable to compare to the GATT        */
   /* Primary Service UUID.                                             */
#define GATT_COMPARE_CHARACTERISTIC_AGGREGATE_FORMAT_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_16(_x)       COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x29, 0x05)

   /* GATT Service Types. (defined Service UUIDs).                      */

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Service UUID.  This MACRO accepts one parameter which is the      */
   /* UUID_16_t variable that is to receive the GATT Service UUID.  This*/
   /* MACRO assigns the GATT Service UUID 16.                           */
#define GATT_ASSIGN_GATT_SERVICE_UUID(_x)                                        ASSIGN_BLUETOOTH_UUID_16((_x), 0x18, 0x01)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined GATT Service UUID in UUID16 form.  This    */
   /* MACRO only returns whether the UUID_16_t variable is equal to the */
   /* NWAS Service UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the GATT Service UUID.                              */
#define GATT_COMPARE_GATT_SERVICE_UUID_TO_BLUETOOTH_UUID_16(_x)                  COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x01)

   /* The following defines the GATT Service UUID that is used when     */
   /* building the GATT Service Table.                                  */
#define GATT_SERVICE_BLUETOOTH_UUID_CONSTANT                                     { 0x01, 0x18 }

   /* GATT SDP UUID Types.                                              */

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Service UUID (for use with SDP, which is Big Endian).  This MACRO */
   /* accepts one parameter which is the UUID_16_t variable that is to  */
   /* receive the GATT Service UUID (for SDP).  This MACRO assigns the  */
   /* GATT Service UUID 16 (in Big Endian format).                      */
#define SDP_ASSIGN_GATT_UUID_16(_x)                                              ASSIGN_SDP_UUID_16((_x), 0x18, 0x01)

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Service UUID (for use with SDP, which is Big Endian).  This MACRO */
   /* accepts one parameter which is the UUID_32_t variable that is to  */
   /* receive the GATT Service UUID (for SDP).  This MACRO assigns the  */
   /* GATT Service UUID 32 (in Big Endian format).                      */
#define SDP_ASSIGN_GATT_UUID_32(_x)                                              ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x18, 0x01)

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Service UUID (for use with SDP, which is Big Endian).  This MACRO */
   /* accepts one parameter which is the UUID_128_t variable that is to */
   /* receive the GATT Service UUID (for SDP).  This MACRO assigns the  */
   /* GATT Service UUID 128 (in Big Endian format).                     */
#define SDP_ASSIGN_GATT_UUID_128(_x)                                             ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x18, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following MACRO is a utility MACRO that assigns the Attribute */
   /* Protocol UUID (for use with SDP, which is Big Endian).  This MACRO*/
   /* accepts one parameter which is the UUID_16_t variable that is to  */
   /* receive the Attribute Protocol UUID (for SDP).  This MACRO assigns*/
   /* the Attribute Protocol UUID 16 (in Big Endian format).            */
#define SDP_ASSIGN_ATT_UUID_16(_x)                                               ASSIGN_SDP_UUID_16((_x), 0x00, 0x07)

   /* The following MACRO is a utility MACRO that assigns the Attribute */
   /* Protocol UUID (for use with SDP, which is Big Endian).  This MACRO*/
   /* accepts one parameter which is the UUID_32_t variable that is to  */
   /* receive the Attribute Protocol UUID (for SDP).  This MACRO assigns*/
   /* the Attribute Protocol UUID 32 (in Big Endian format).            */
#define SDP_ASSIGN_ATT_UUID_32(_x)                                               ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x00, 0x07)

   /* The following MACRO is a utility MACRO that assigns the Attribute */
   /* Protocol UUID (for use with SDP, which is Big Endian).  This MACRO*/
   /* accepts one parameter which is the UUID_128_t variable that is to */
   /* receive the Attribute Protocol UUID (for SDP).  This MACRO assigns*/
   /* the Attribute Protocol UUID 128 (in Big Endian format).           */
#define SDP_ASSIGN_ATT_UUID_128(_x)                                              ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* GATT Characteristic Types. (defined Characteristic Value UUIDs).  */

   /* The following MACRO is a utility MACRO that assigns the GATT      */
   /* Profile Service Changed Attribute Type. This                      */
   /* MACRO accepts one parameter which is the UUID_16_t variable that  */
   /* is to receive the ATTRIBUTE_PROTOCOL_UUID_16 Constant value. This */
   /* MACRO assigns the GATT Service UUID 16.                           */
#define GATT_ASSIGN_SERVICE_CHANGED_ATTRIBUTE_TYPE(_x)                           ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x05)

   /* The following MACRO is a utility MACRO that exist to compare an   */
   /* Attribute Type to the defined Service Changed UUID                */
   /* UUID in UUID16 form. This MACRO only returns whether the          */
   /* UUID_16_t variable is equal to the Primary Service UUID (MACRO    */
   /* returns boolean result) NOT less than/greater than. The first     */
   /* parameter is the UUID_16_t variable to compare to the GATT        */
   /* Primary Service UUID.                                             */
#define GATT_COMPARE_SERVICE_CHANGED_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_16(_x)     COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x05)

   /* The following defines the GATT Service Changed UUID that is used  */
   /* when building the GATT Service Table.                             */
#define GATT_SERVICE_CHANGED_BLUETOOTH_UUID_CONSTANT                             { 0x05, 0x2A }

   /* GATT Characteristic Properties Definitions.                       */
#define GATT_CHARACTERISTIC_PROPERTIES_BROADCAST                      0x01
#define GATT_CHARACTERISTIC_PROPERTIES_READ                           0x02
#define GATT_CHARACTERISTIC_PROPERTIES_WRITE_WITHOUT_RESPONSE         0x04
#define GATT_CHARACTERISTIC_PROPERTIES_WRITE                          0x08
#define GATT_CHARACTERISTIC_PROPERTIES_NOTIFY                         0x10
#define GATT_CHARACTERISTIC_PROPERTIES_INDICATE                       0x20
#define GATT_CHARACTERISTIC_PROPERTIES_AUTHENTICATED_SIGNED_WRITES    0x40
#define GATT_CHARACTERISTIC_PROPERTIES_EXTENDED_PROPERTIES            0x80

   /* GATT Characteristic Extended Properties Definitions.              */
#define GATT_CHARACTERISTIC_EXTENDED_PROPERTIES_RELIABLE_WRITE        0x0001
#define GATT_CHARACTERISTIC_EXTENDED_PROPERTIES_WRITABLE_AUXILARIES   0x0002

   /* GATT Client Configuration Characteristic Definitions.             */
#define GATT_CLIENT_CONFIGURATION_CHARACTERISTIC_NOTIFY_ENABLE        0x0001
#define GATT_CLIENT_CONFIGURATION_CHARACTERISTIC_INDICATE_ENABLE      0x0002

   /* GATT Server Configuration Characteristic Definitions.             */
#define GATT_SERVER_CONFIGURATION_CHARACTERISTIC_BROADCAST_ENABLE     0x0001

   /* The following type definition defines the structure of the GATT   */
   /* Group Attribute Data structure.  This is used to map the          */
   /* AttributeDataList parameter data of the                           */
   /* ATT_Read_By_Group_Type_Response_PDU_t.                            */
typedef __PACKED_STRUCT_BEGIN__ struct _tagGATT_Group_Attribute_Data_t
{
   NonAlignedWord_t AttributeHandle;
   NonAlignedWord_t EndGroupHandle;
   Byte_t           ServiceUUID[1];
} __PACKED_STRUCT_END__ GATT_Group_Attribute_Data_t;

   /* The following is a utility MACRO provided to determine number of  */
   /* bytes of a GATT Group Attribute Data structure.  The input        */
   /* parameter specifies the size in bytes of the UUID Value           */
#define GATT_GROUP_ATTRIBUTE_DATA_SIZE(_x)               (BTPS_STRUCTURE_OFFSET(GATT_Group_Attribute_Data_t, ServiceUUID) + ((_x) * BYTE_SIZE))

   /* The following is a container structure that is used with the      */
   /* AttributeData member to specify an individual Attribute           */
   /* Handle/Value pair.                                                */
typedef __PACKED_STRUCT_BEGIN__ struct _tagGATT_Handle_Value_Information_t
{
   NonAlignedWord_t AttributeHandle;
   Byte_t           AttributeData[1];
} __PACKED_STRUCT_END__ GATT_Handle_Value_Information_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an individual Handle/Value Pair      */
   /* entry.  The input parameter specifies the length of the Attribute */
   /* Data (in bytes).  This MACRO calculates the total size required   */
   /* (in bytes) to hold the entire entry.                              */
#define GATT_HANDLE_VALUE_INFORMATION_DATA_SIZE(_x)      (BTPS_STRUCTURE_OFFSET(GATT_Handle_Value_Information_t, AttributeData) + ((_x) * BYTE_SIZE))

   /* The following type definition defines the structure of the GATT   */
   /* Handle Information structure.  This is used to map the            */
   /* HandleInformation parameter data of the                           */
   /* ATT_Find_By_Type_Value_Response_PDU_t.                            */
typedef __PACKED_STRUCT_BEGIN__ struct _tagGATT_Primary_Service_By_UUID_Entry_t
{
   NonAlignedWord_t StartingHandle;
   NonAlignedWord_t EndingHandle;
} __PACKED_STRUCT_END__ GATT_Primary_Service_By_UUID_Entry_t;

   /* The following is a utility MACRO provided to determine number of  */
   /* bytes of a Primary Service Infromation structure.  The input      */
   /* parameter specifies the size in bytes of the UUID Value           */
#define GATT_PRIMARY_SERVICE_BY_UUID_ENTRY_DATA_SIZE      (sizeof(GATT_Primary_Service_By_UUID_Entry_t))

   /* The following type definition defines the structure of the        */
   /* Included Service Information structure.  This is used to map the  */
   /* AttributeDataList parameter data of the                           */
   /* ATT_Read_By_Type_Response_PDU_t.                                  */
typedef __PACKED_STRUCT_BEGIN__ struct _tagGATT_Included_Service_Info_t
{
   NonAlignedWord_t AttributeHandle;
   NonAlignedWord_t IncludedServiceHandle;
   NonAlignedWord_t EndGroupHandle;
   Byte_t           ServiceUUID[1];
} __PACKED_STRUCT_END__ GATT_Included_Service_Info_t;

   /* The following is a utility MACRO provided to determine number of  */
   /* bytes of a Included Service Infromation structure.  The input     */
   /* parameter specifies the size in bytes of the UUID Value           */
#define GATT_INCLUDED_SERVICE_INFO_DATA_SIZE(_x)         (BTPS_STRUCTURE_OFFSET(GATT_Included_Service_Info_t, ServiceUUID) + ((_x) * BYTE_SIZE))

   /* The following type definition defines the structure of the        */
   /* Included Service Value.                                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagGATT_Included_Service_Value_t
{
   NonAlignedWord_t IncludedServiceHandle;
   NonAlignedWord_t EndGroupHandle;
   Byte_t           ServiceUUID[1];
} __PACKED_STRUCT_END__ GATT_Included_Service_Value_t;

   /* The following is a utility MACRO provided to determine number of  */
   /* bytes of a Included Service Infromation structure.  The input     */
   /* parameter specifies the size in bytes of the UUID Value           */
#define GATT_INCLUDED_SERVICE_VALUE_DATA_SIZE(_x)         (BTPS_STRUCTURE_OFFSET(GATT_Included_Service_Value_t, ServiceUUID) + ((_x) * BYTE_SIZE))

   /* The following type definition defines the structure of the        */
   /* Characteristic Value structure.                                   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagGATT_Characteristic_Declaration_Value_Field_t
{
   NonAlignedByte_t Properties;
   NonAlignedWord_t CharacteristicHandle;
   NonAlignedByte_t UUID[1];
} __PACKED_STRUCT_END__ GATT_Characteristic_Declaration_Value_Field_t;

   /* The following is a utility MACRO provided to determine number of  */
   /* bytes of a Characteristic Infromation structure.  The input       */
   /* parameter specifies the size in bytes of the UUID Value           */
#define GATT_CHARACTERISTIC_DECLARATION_VALUE_FIELD_DATA_SIZE(_x) (BTPS_STRUCTURE_OFFSET(GATT_Characteristic_Declaration_Value_Field_t, UUID) + ((_x) * BYTE_SIZE))

   /* The following type definition defines the structure of the        */
   /* Characteristic Information structure.  This is used to map the    */
   /* AttributeDataList parameter data of the                           */
   /* ATT_Read_By_Type_Response_PDU_t.                                  */
typedef __PACKED_STRUCT_BEGIN__ struct _tagGATT_Characteristic_Info_t
{
   NonAlignedWord_t DeclarationHandle;
   NonAlignedByte_t Properties;
   NonAlignedWord_t CharacteristicHandle;
   NonAlignedByte_t UUID[1];
} __PACKED_STRUCT_END__ GATT_Characteristic_Info_t;

   /* The following is a utility MACRO provided to determine number of  */
   /* bytes of a Characteristic Infromation structure.  The input       */
   /* parameter specifies the size in bytes of the UUID Value           */
#define GATT_CHARACTERISTIC_INFO_DATA_SIZE(_x)           (BTPS_STRUCTURE_OFFSET(GATT_Characteristic_Info_t, UUID) + ((_x) * BYTE_SIZE))

   /* The following type definition defines the structure of the GATT   */
   /* Service Changed characteristic value.                             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagGATT_Service_Changed_Characteristic_Value_t
{
   NonAlignedWord_t Start_Handle;
   NonAlignedWord_t End_Handle;
} __PACKED_STRUCT_END__ GATT_Service_Changed_Characteristic_Value_t;

#define GATT_SERVICE_CHANGED_CHARACTERISTIC_VALUE_DATA_SIZE (sizeof(GATT_Service_Changed_Characteristic_Value_t))

   /* The following type definition defines the structure of the GATT   */
   /* Data Time Characteristic value.                                   */
   /* * NOTE * The assigned number for this format is 0x2A08.           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagGATT_Date_Time_Characteristic_t
{
   NonAlignedWord_t Year;
   NonAlignedByte_t Month;
   NonAlignedByte_t Day;
   NonAlignedByte_t Hours;
   NonAlignedByte_t Minutes;
   NonAlignedByte_t Seconds;
} __PACKED_STRUCT_END__ GATT_Date_Time_Characteristic_t;

#define GATT_DATE_TIME_CHARACTERISTIC_DATA_SIZE          (sizeof(GATT_Date_Time_Characteristic_t))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of assign the Year into a                             */
   /* GATT_Date_Time_Characteristic_t structure.  The first parameter to*/
   /* this MACRO is a pointer to the GATT_Date_Time_Characteristic_t to */
   /* assign the year from.  The second parameter is the Year to assign.*/
#define GATT_DATE_TIME_ASSIGN_YEAR(_x, _y)               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(((GATT_Date_Time_Characteristic_t *)(_x))->Year), (_y))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of reading the Year from a                            */
   /* GATT_Date_Time_Characteristic_t structure.  The only parameter to */
   /* this MACRO is a pointer to the GATT_Date_Time_Characteristic_t to */
   /* read the year from.  This MACRO returns the Year that was read    */
   /* from the structure.                                               */
#define GATT_DATE_TIME_READ_YEAR(_x)                     READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((GATT_Date_Time_Characteristic_t *)(_x))->Year))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified Year value in a */
   /* GATT_Date_Time_Characteristic_t structure is valid.  The first    */
   /* parameter to this MACRO is the Year value to verify.  This MACRO  */
   /* returns a boolean value based upon whether or not the specified   */
   /* Year value is valid.  This MACRO returns a boolean TRUE if the    */
   /* specified Year is valid, or a boolean FALSE if the specified Year */
   /* value is invalid.                                                 */
#define GATT_DATE_TIME_VALID_YEAR(_x)                    ((!(_x))  || (((_x) >= 1582) && ((_x) <= 9999)))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of assign the Month into a                            */
   /* GATT_Date_Time_Characteristic_t structure.  The first parameter to*/
   /* this MACRO is a pointer to the GATT_Date_Time_Characteristic_t to */
   /* assign the year from.  The second parameter is the Month to       */
   /* assign.                                                           */
#define GATT_DATE_TIME_ASSIGN_MONTH(_x, _y)               ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(((GATT_Date_Time_Characteristic_t *)(_x))->Month), (_y))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of reading the Month from a                           */
   /* GATT_Date_Time_Characteristic_t structure.  The only parameter to */
   /* this MACRO is a pointer to the GATT_Date_Time_Characteristic_t to */
   /* read the year from.  This MACRO returns the Month that was read   */
   /* from the structure.                                               */
#define GATT_DATE_TIME_READ_MONTH(_x)                    READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((GATT_Date_Time_Characteristic_t *)(_x))->Month))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified Month value in a*/
   /* GATT_Date_Time_Characteristic_t structure is valid.  The first    */
   /* parameter to this MACRO is the Month value to verify.  This MACRO */
   /* returns a boolean value based upon whether or not the specified   */
   /* Month value is valid.  This MACRO returns a boolean TRUE if the   */
   /* specified Month is valid, or a boolean FALSE if the specified     */
   /* Month value is invalid.                                           */
#define GATT_DATE_TIME_VALID_MONTH(_x)                   ((!(_x))  || (((_x) >= 1) && ((_x) <= 12)))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of assign the Day into a                              */
   /* GATT_Date_Time_Characteristic_t structure.  The first parameter to*/
   /* this MACRO is a pointer to the GATT_Date_Time_Characteristic_t to */
   /* assign the year from.  The second parameter is the Day to assign. */
#define GATT_DATE_TIME_ASSIGN_DAY(_x, _y)               ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(((GATT_Date_Time_Characteristic_t *)(_x))->Day), (_y))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of reading the Day from a                             */
   /* GATT_Date_Time_Characteristic_t structure.  The only parameter to */
   /* this MACRO is a pointer to the GATT_Date_Time_Characteristic_t to */
   /* read the year from.  This MACRO returns the Day that was read from*/
   /* the structure.                                                    */
#define GATT_DATE_TIME_READ_DAY(_x)                    READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((GATT_Date_Time_Characteristic_t *)(_x))->Day))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified Day value in a  */
   /* GATT_Date_Time_Characteristic_t structure is valid.  The first    */
   /* parameter to this MACRO is the Day value to verify.  This MACRO   */
   /* returns a boolean value based upon whether or not the specified   */
   /* Day value is valid.  This MACRO returns a boolean TRUE if the     */
   /* specified Day is valid, or a boolean FALSE if the specified Day   */
   /* value is invalid.                                                 */
#define GATT_DATE_TIME_VALID_DAY(_x)                     ((!(_x))  || (((_x) >= 1) && ((_x) <= 31)))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of assign the Hours into a                            */
   /* GATT_Date_Time_Characteristic_t structure.  The first parameter to*/
   /* this MACRO is a pointer to the GATT_Date_Time_Characteristic_t to */
   /* assign the year from.  The second parameter is the Hours to       */
   /* assign.                                                           */
#define GATT_DATE_TIME_ASSIGN_HOURS(_x, _y)              ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(((GATT_Date_Time_Characteristic_t *)(_x))->Hours), (_y))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of reading the Hours from a                           */
   /* GATT_Date_Time_Characteristic_t structure.  The only parameter to */
   /* this MACRO is a pointer to the GATT_Date_Time_Characteristic_t to */
   /* read the year from.  This MACRO returns the Hours that was read   */
   /* from the structure.                                               */
#define GATT_DATE_TIME_READ_HOURS(_x)                    READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((GATT_Date_Time_Characteristic_t *)(_x))->Hours))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified Hours value in a*/
   /* GATT_Date_Time_Characteristic_t structure is valid.  The first    */
   /* parameter to this MACRO is the Hours value to verify.  This MACRO */
   /* returns a boolean value based upon whether or not the specified   */
   /* Hours value is valid.  This MACRO returns a boolean TRUE if the   */
   /* specified Hours is valid, or a boolean FALSE if the specified     */
   /* Hours value is invalid.                                           */
#define GATT_DATE_TIME_VALID_HOURS(_x)                   (((_x) >= 0) && ((_x) <= 23))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of assign the Minutes into a                          */
   /* GATT_Date_Time_Characteristic_t structure.  The first parameter to*/
   /* this MACRO is a pointer to the GATT_Date_Time_Characteristic_t to */
   /* assign the year from.  The second parameter is the Minutes to     */
   /* assign.                                                           */
#define GATT_DATE_TIME_ASSIGN_MINUTES(_x, _y)            ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(((GATT_Date_Time_Characteristic_t *)(_x))->Minutes), (_y))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of reading the Minutes from a                         */
   /* GATT_Date_Time_Characteristic_t structure.  The only parameter to */
   /* this MACRO is a pointer to the GATT_Date_Time_Characteristic_t to */
   /* read the year from.  This MACRO returns the Minutes that was read */
   /* from the structure.                                               */
#define GATT_DATE_TIME_READ_MINUTES(_x)                  READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((GATT_Date_Time_Characteristic_t *)(_x))->Minutes))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified Minutes value in*/
   /* a GATT_Date_Time_Characteristic_t structure is valid.  The first  */
   /* parameter to this MACRO is the Minutes value to verify.  This     */
   /* MACRO returns a boolean value based upon whether or not the       */
   /* specified Minutes value is valid.  This MACRO returns a boolean   */
   /* TRUE if the specified Minutes is valid, or a boolean FALSE if the */
   /* specified Minutes value is invalid.                               */
#define GATT_DATE_TIME_VALID_MINUTES(_x)                 (((_x) >= 0) && ((_x) <= 59))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of assign the Seconds into a                          */
   /* GATT_Date_Time_Characteristic_t structure.  The first parameter to*/
   /* this MACRO is a pointer to the GATT_Date_Time_Characteristic_t to */
   /* assign the year from.  The second parameter is the Seconds to     */
   /* assign.                                                           */
#define GATT_DATE_TIME_ASSIGN_SECONDS(_x, _y)            ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(((GATT_Date_Time_Characteristic_t *)(_x))->Seconds), (_y))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of reading the Seconds from a                         */
   /* GATT_Date_Time_Characteristic_t structure.  The only parameter to */
   /* this MACRO is a pointer to the GATT_Date_Time_Characteristic_t to */
   /* read the year from.  This MACRO returns the Seconds that was read */
   /* from the structure.                                               */
#define GATT_DATE_TIME_READ_SECONDS(_x)                  READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((GATT_Date_Time_Characteristic_t *)(_x))->Seconds))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified Seconds value in*/
   /* a GATT_Date_Time_Characteristic_t structure is valid.  The first  */
   /* parameter to this MACRO is the Seconds value to verify.  This     */
   /* MACRO returns a boolean value based upon whether or not the       */
   /* specified Seconds value is valid.  This MACRO returns a boolean   */
   /* TRUE if the specified Seconds is valid, or a boolean FALSE if the */
   /* specified Seconds value is invalid.                               */
#define GATT_DATE_TIME_VALID_SECONDS(_x)                 (((_x) >= 0) && ((_x) <= 59))

#endif
