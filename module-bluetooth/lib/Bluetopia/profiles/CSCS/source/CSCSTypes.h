/*****< CSCStypes.h >**********************************************************/
/*																			  */
/* Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/ 	  */
/* All Rights Reserved.														  */
/*																			  */
/*  CSCSTypes - Bluetooth Stack Cycling Speed and Cadence Service Types.      */
/*                                                              			  */
/*  Author:  Dan Horowitz                                                     */
/*																			  */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   11/25/14  D. Horowitz    Initial creation.                               */
/******************************************************************************/
#ifndef __CSCSTYPESH__
#define __CSCSTYPESH__
#include "BTTypes.h"            /* Bluetooth Type Definitions.          */

   /* The following define the defined CSCS Error Codes that may be sent*/
   /* in a GATT Error Response.                                         */
#define CSCS_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS                                           0x80
#define CSCS_ERROR_CODE_CLIENT_CCD_IMPROPERLY_CONFIGURED                                        0x81

   /* The following MACRO is a utility MACRO that assigns the Cycling	*/
   /* Speed and Cadence Service 16 bit UUID to the specified UUID_16_t	*/
   /* variable. This MACRO accepts one parameter which is a pointer to	*/
   /* a UUID_16_t variable that is to receive the CSCS UUID Constant	*/
   /* value.								*/
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define CSCS_ASSIGN_CSCS_SERVICE_UUID_16(_x)                                                    ASSIGN_BLUETOOTH_UUID_16(*((UUID_16_t *)(_x)), 0x18, 0x16)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined CSCS Service UUID in UUID16 form.  This    */
   /* MACRO only returns whether the UUID_16_t variable is equal to the */
   /* CSCS Service UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the CSCS Service UUID.                              */
#define CSCS_COMPARE_CSCS_SERVICE_UUID_TO_UUID_16(_x)                                           COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x16)

   /* The following defines the Cycling Speed and Cadence Service UUID	*/
   /* that is used when building the CSCS Service Table.		*/
#define CSCS_SERVICE_BLUETOOTH_UUID_CONSTANT                                                    { 0x16, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the CSCS	*/
   /* Cycling Speed and Cadence Measurement Characteristic 16 bit UUID	*/
   /* to the specified UUID_16_t variable. This MACRO accepts one	*/
   /* parameter which is the UUID_16_t variable that is to receive the	*/
   /* CSCS Cycling Speed and Cadence Measurement UUID Constant value.	*/
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
 #define CSCS_ASSIGN_CYCLING_SPEED_AND_CADENCE_MEASUREMENT_UUID_16(_x)                          ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x5B)

   /* The following MACRO is a utility MACRO that exist to compare a  	*/
   /* UUID 16 to the defined CSCS Cycling Speed and Cadence Measurement	*/
   /* UUID in UUID16 form. This MACRO only returns whether the		*/
   /* UUID_16_t variable is equal to the Cycling Speed and Cadence	*/
   /* Measurement UUID (MACRO returns boolean result) NOT less than/	*/
   /* greater than. The first parameter is the UUID_16_t variable to 	*/
   /* compare to the CSCS Cycling Speed and Cadence Measurement UUID.	*/
#define CSCS_COMPARE_CSCS_CYCLING_SPEED_AND_CADENCE_MEASUREMENT_UUID_TO_UUID_16(_x)             COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x5B)

   /* The following defines the CSCS Cycling Speed and Cadence		*/
   /* Measurement Characteristic UUID that is used when building the	*/
   /* CSCS Service Table.						*/
#define CSCS_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT                                 { 0x5B, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the CSCS	*/
   /* Cycling Speed and Cadence Feature Characteristic 16 bit UUID	*/
   /* to the specified UUID_16_t variable. This MACRO accepts one	*/
   /* parameter which is the UUID_16_t variable that is to receive the	*/
   /* CSCS Cycling Speed and Cadence Feature UUID Constant value.	*/
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
 #define CSCS_ASSIGN_CYCLING_SPEED_AND_CADENCE_FEATURE_UUID_16(_x)                              ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x5C)

   /* The following MACRO is a utility MACRO that exist to compare a  	*/
   /* UUID 16 to the defined CSCS Cycling Speed and Cadence Feature	*/
   /* UUID in UUID16 form. This MACRO only returns whether the		*/
   /* UUID_16_t variable is equal to the Cycling Speed and Cadence	*/
   /* Feature UUID (MACRO returns boolean result) NOT less than/greater	*/
   /* than. The first parameter is the UUID_16_t variable to compare	*/
   /* to the CSCS Cycling Speed and Cadence Feature UUID.		*/
#define CSCS_COMPARE_CSCS_CYCLING_SPEED_AND_CADENCE_FEATURE_UUID_TO_UUID_16(_x)                 COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x5C)

   /* The following defines the CSCS Cycling Speed and Cadence		*/
   /* Feature Characteristic UUID that is used when building the	*/
   /* CSCS Service Table.						*/
#define CSCS_FEATURE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT                                     { 0x5C, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the CSCS	*/
   /* Sensor Location Characteristic 16 bit UUID to the specified       */
   /* UUID_16_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_16_t variable that is to receive the CSCS Sensor Location	*/
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define CSCS_ASSIGN_SENSOR_LOCATION_UUID_16(_x)                                                 ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x5D)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined CSCS Sensor Location UUID in UUID16 form. 	*/
   /* This MACRO only returns whether the UUID_16_t variable is equal	*/
   /* to the Sensor Location UUID (MACRO returns boolean result) NOT	*/
   /* less than/greater than.  The first parameter is the UUID_16_t	*/
   /* variable to compare to the CSCS Sensor Location UUID.     	*/
#define CSCS_COMPARE_CSCS_SENSOR_LOCATION_UUID_TO_UUID_16(_x)                                   COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x5D)

   /* The following defines the CSCS Sensor Location Characteristic 	*/
   /* UUID that is used when building the CSCS Service Table.           */
#define CSCS_SENSOR_LOCATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT                             { 0x5D, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the CSCS SC	*/
   /* Control Point Characteristic 16 bit UUID to the specified    	*/
   /* UUID_16_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_16_t variable that is to receive the CSCS SC Control Point 	*/
   /* UUID Constant value.                                        	*/
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define CSCS_ASSIGN_SC_CONTROL_POINT_UUID_16(_x)                                                ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x55)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined CSCS SC Control Point UUID in UUID16	*/
   /* form.  This MACRO only returns whether the UUID_16_t variable is  */
   /* equal to the SC Control Point UUID (MACRO returns boolean		*/
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* UUID_16_t variable to compare to the CSCS SC Control Point UUID.	*/
#define CSCS_COMPARE_CSCS_SC_CONTROL_POINT_UUID_TO_UUID_16(_x)                                  COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x55)

   /* The following defines the CSCS SC Control Point Characteristic	*/
   /* UUID that is used when building the CSCS Service Table.           */
#define CSCS_SC_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT                            { 0x55, 0x2A }

   /* The following structure defines the structure of the values that  */
   /* Wheel Revolution Data Present enables.                            */
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_WHEEL_REVOLUTION_DATA)
typedef struct _tagCSCS_Wheel_Value_t
{
   NonAlignedDWord_t    Cumulative_Wheel_Revolutions;
   NonAlignedWord_t     Last_Wheel_Event_Time;
} CSCS_Wheel_Value_t;

   /* The following defines the length of the structure of a Cycle Speed*/
   /* and Cadence notification.                                         */
#define CSCS_WHEEL_VALUE_SIZE                                                                   (sizeof(CSCS_Wheel_Value_t))
#endif



#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_CRANK_REVOLUTION_DATA)
   /* The following structure defines the structure of the values that  */
   /* Crank Revolution Data Present enables.                            */
typedef struct _tagCSCS_Crank_Value_t
{
   NonAlignedWord_t     Cumulative_Crank_Revolutions;
   NonAlignedWord_t     Last_Crank_Event_Time;
} CSCS_Crank_Value_t;
   /* The following defines the length of the structure of a Cycle Speed*/
   /* and Cadence notification.                                         */
#define CSCS_CRANK_VALUE_SIZE                                                                   (sizeof(CSCS_Crank_Value_t))
#endif


   /* The following structure defines the structure of a Cycle Speed    */
   /* and Cadence notification.                                         */
typedef __PACKED_STRUCT_BEGIN__ struct _tagCSCS_Measurements_Notification_t
{
   NonAlignedByte_t     Flags;
} __PACKED_STRUCT_END__ CSCS_Measurements_Notification_t;

   /* The following defines the length of the structure of a Cycle Speed*/
   /* and Cadence notification.                                         */
#define CSCS_MEASUREMENTS_NOTIFICATION_DATA_SIZE                                                (sizeof(CSCS_Measurements_Notification_t))

   /* The following structure defines the structure of a CS Control     */
   /* Point Response.							*/
typedef __PACKED_STRUCT_BEGIN__ struct _tagCSCS_Control_Point_Indication_Value_t
{
   NonAlignedByte_t 	Request_Op_Code;
   NonAlignedByte_t 	Response_Value;
   NonAlignedByte_t 	Response_Parameter[1];
} __PACKED_STRUCT_END__ CSCS_Control_Point_Indication_Value_t;

   /* The following defines the length of the Supported CS Control      */
   /* Point Response characteristic value.                              */
   #define CSCS_CONTROL_POINT_INDICATION_SIZE(_x)                                               (BTPS_STRUCTURE_OFFSET(CSCS_Control_Point_Indication_Value_t, Response_Parameter[0]) + ((_x)*sizeof(NonAlignedByte_t)))
   #define CSCS_CONTROL_POINT_INDICATION_LIST_SIZE                                              (BTPS_STRUCTURE_OFFSET(CSCS_Control_Point_Indication_Value_t, Response_Parameter[0]))

   /* The following structure defines the structure of a CS Control     */
   /* Point Command.                               			*/
typedef __PACKED_STRUCT_BEGIN__ struct _tagCSCS_Control_Point_Value_t
{
   NonAlignedByte_t 	Op_Code;
   union _tagCommand_Data_Buffer
   {
      NonAlignedDWord_t                         Cumulative_Value;
      NonAlignedByte_t 	                        Sensor_Location_Value;
      CSCS_Control_Point_Indication_Value_t     Indication;
   } __PACKED_STRUCT_END__ Command_Data_Buffer;
} __PACKED_STRUCT_END__ CSCS_Control_Point_Value_t;

   /* The following defines the length of the Supported CS Control      */
   /* Point Command characteristic value.                               */
#define CSCS_CONTROL_POINT_VALUE_SIZE                                                           (sizeof(CSCS_Control_Point_Value_t))
#define CSCS_CONTROL_POINT_INDICATION_VALUE_SIZE(_x)                                            (sizeof(CSCS_Control_Point_Value_t) + ((_x)*BYTE_SIZE))

#define CSCS_CONTROL_POINT_SET_CUMULATIVE_VALUE_SIZE                                            (sizeof(NonAlignedByte_t) + sizeof(NonAlignedDWord_t))
#define CSCS_CONTROL_POINT_UPDATE_SENSOR_LOCATION_VALUE_SIZE                                    (sizeof(NonAlignedByte_t) + sizeof(NonAlignedByte_t))
#define CSCS_CONTROL_POINT_LIST_REQUEST_VALUE_SIZE                                              (sizeof(NonAlignedByte_t))
#define CSCS_CONTROL_POINT_RESPONSE_VALUE_SIZE                                                  (sizeof(NonAlignedByte_t) + CSCS_CONTROL_POINT_INDICATION_LIST_SIZE)
#define CSCS_CONTROL_POINT_INDICATION_LIST_VALUE_SIZE(_x)                                       (sizeof(NonAlignedByte_t) + CSCS_CONTROL_POINT_INDICATION_SIZE(_x))
#define CSCS_CONTROL_POINT_MINIMUM_VALUE_SIZE                                                   (sizeof(NonAlignedByte_t))
   
   /* The following define the valid Cycling Speed and Cadence		*/
   /* Measurement Flags bit that may be set in the Flags field of a 	*/
   /* Cycling Speed and Cadence Measurement.				*/
#define CSCS_CYCLING_SPEED_AND_CADENCE_MEASUREMENT_FLAGS_WHEEL_REVOLUTION_DATA_PRESENT          0x01
#define CSCS_CYCLING_SPEED_AND_CADENCE_MEASUREMENT_FLAGS_CRANK_REVOLUTION_DATA_PRESENT          0x02

#define CSCS_DATA_PRESENT_VALID(_x)                                                             ((((Byte_t)(_x)) & CSCS_CYCLING_SPEED_AND_CADENCE_MEASUREMENT_FLAGS_WHEEL_REVOLUTION_DATA_PRESENT) || (((Byte_t)(_x)) & CSCS_CYCLING_SPEED_AND_CADENCE_MEASUREMENT_FLAGS_CRANK_REVOLUTION_DATA_PRESENT))

   /* The following define the valid Sensor Location enumerated		*/
   /* values that may be set as the value for the Sensor Location	*/
   /* characteristic value.                                             */
#define CSC_SENSOR_LOCATION_OTHER                                                               0
#define CSC_SENSOR_LOCATION_TOP_OF_SHOE                                                         1
#define CSC_SENSOR_LOCATION_IN_SHOE                                                             2
#define CSC_SENSOR_LOCATION_HIP                                                                 3
#define CSC_SENSOR_LOCATION_FRONT_WHEEL                                                         4
#define CSC_SENSOR_LOCATION_LEFT_CRANK                                                          5
#define CSC_SENSOR_LOCATION_RIGHT_CRANK                                                         6
#define CSC_SENSOR_LOCATION_LEFT_PEDAL                                                          7
#define CSC_SENSOR_LOCATION_RIGHT_PEDAL                                                         8
#define CSC_SENSOR_LOCATION_FRONT_HUB                                                           9
#define CSC_SENSOR_LOCATION_REAR_DROPOUT                                                        10
#define CSC_SENSOR_LOCATION_CHAINSTAY                                                           11
#define CSC_SENSOR_LOCATION_REAR_WHEEL                                                          12
#define CSC_SENSOR_LOCATION_REAR_HUB                                                            13
#define CSC_SENSOR_LOCATION_CHEST                                                               14

   /* The following define the valid Sensor Location Bitmask		*/
   /* values that may be set as the value for the Sensor Location	*/
   /* characteristic value.                                         */
#define CSC_SENSOR_LOCATION_OTHER_BIT_MASK                                                      0x0001
#define CSC_SENSOR_LOCATION_TOP_OF_SHOE_BIT_MASK                                                0x0002
#define CSC_SENSOR_LOCATION_IN_SHOE_BIT_MASK                                                    0x0004
#define CSC_SENSOR_LOCATION_HIP_BIT_MASK                                                        0x0008
#define CSC_SENSOR_LOCATION_FRONT_WHEEL_BIT_MASK                                                0x0010
#define CSC_SENSOR_LOCATION_LEFT_CRANK_BIT_MASK                                                 0x0020
#define CSC_SENSOR_LOCATION_RIGHT_CRANK_BIT_MASK                                                0x0040
#define CSC_SENSOR_LOCATION_LEFT_PEDAL_BIT_MASK                                                 0x0080
#define CSC_SENSOR_LOCATION_RIGHT_PEDAL_BIT_MASK                                                0x0100
#define CSC_SENSOR_LOCATION_FRONT_HUB_BIT_MASK                                                  0x0200
#define CSC_SENSOR_LOCATION_REAR_DROPOUT_BIT_MASK                                               0x0400
#define CSC_SENSOR_LOCATION_CHAINSTAY_BIT_MASK                                                  0x0800
#define CSC_SENSOR_LOCATION_REAR_WHEEL_BIT_MASK                                                 0x1000
#define CSC_SENSOR_LOCATION_REAR_HUB_BIT_MASK                                                   0x2000
#define CSC_SENSOR_LOCATION_CHEST_BIT_MASK                                                      0x4000


   /* The following MACRO is a utility MACRO that exists to valid that a*/
   /* specified Sensor Location is valid.  The only parameter to this  	*/
   /* function is the Sensor Location to valid.  This MACRO returns	*/
   /* TRUE if the Sensor Location is valid or FALSE otherwise.       	*/
#define CSCS_SENSOR_LOCATION_VALID_TYPE(_x)                                                     ((((Byte_t)(_x)) >= CSC_SENSOR_LOCATION_OTHER) && (((Byte_t)(_x)) <= CSC_SENSOR_LOCATION_CHEST))
#define CSCS_SENSOR_LOCATION_MAX_VALID_LOCATIONS                                                17
#define CSCS_SENSOR_LOCATION_MAX_VALID_LOCATIONS_BITMASK                                        0x7fff
#define CSCS_SENSOR_LOCATION_BITMASK_VALID_TYPE(_x)                                             ((((Word_t)(_x)) > 0) && (((Word_t)(_x)) <= CSCS_SENSOR_LOCATION_MAX_VALID_LOCATIONS_BITMASK))

   /* The following defines the length of the Sensor Location      	*/
   /* characteristic value.                                             */
#define CSCS_SENSOR_LOCATION_VALUE_LENGTH                                                       (BYTE_SIZE)

   /* The following define the valid Cycling Speed and Cadence Control	*/
   /* Point commands that may be written to the Cycling Speed and 	*/
   /* Cadence Control Point characteristic value.			*/
#define CSCS_CONTROL_POINT_OP_CODE_SET_CUMULATIVE_VALUE                                         0x01
#define CSCS_CONTROL_POINT_OP_CODE_START_SENSOR_CALIBRATION                                     0x02
#define CSCS_CONTROL_POINT_OP_CODE_UPDATE_SENSOR_LOCATION                                       0x03
#define CSCS_CONTROL_POINT_OP_CODE_REQUEST_SUPPORTED_SENSOR_LOCATION                            0x04
#define CSCS_CONTROL_POINT_OP_CODE_RESPONSE_CODE                                                0x10
      
   /* The following MACRO is a utility MACRO that exists to determine if*/
   /* the value written to the Cycling Speed and Cadence Control Point  */
   /* is a valid command.                                               */
#define CSCS_CYCLING_SPEED_AND_CADENCE_CONTROL_POINT_VALID_COMMAND(_x)                          (((((Byte_t)(_x)) >= CSCS_CONTROL_POINT_OP_CODE_SET_CUMULATIVE_VALUE) && (((Byte_t)(_x)) <= CSCS_CONTROL_POINT_OP_CODE_REQUEST_SUPPORTED_SENSOR_LOCATION)) || (((Byte_t)(_x)) == CSCS_CONTROL_POINT_OP_CODE_RESPONSE_CODE))

   /* The following define the valid Cycling Speed and Cadence Control	*/
   /* Point Response that may be written to the Cycling Speed and	*/
   /* Cadence Control Point characteristic value.			*/
#define CSCS_CONTROL_POINT_RESPONSE_SUCCESS                                                     0x01
#define CSCS_CONTROL_POINT_RESPONSE_OP_CODE_NOT_SUPPORTED                                       0x02
#define CSCS_CONTROL_POINT_RESPONSE_INVALID_PARAMETER                                           0x03
#define CSCS_CONTROL_POINT_RESPONSE_OPERATION_FAILED                                            0x04

   /* The following MACRO is a utility MACRO that exists to determine if*/
   /* the value written to the Cycling Speed and Cadence Control Point  */
   /* is a valid command.                                               */
#define CSCS_CYCLING_SPEED_AND_CADENCE_CONTROL_POINT_VALID_RESPONSE(_x)                         ((((Byte_t)(_x)) >= CSCS_CONTROL_POINT_RESPONSE_SUCCESS) && (((Byte_t)(_x)) <= CSCS_CONTROL_POINT_RESPONSE_OPERATION_FAILED))

   /* The following defines the length of the Cycling Speed and         */
   /* Cadence Control Point characteristic value.                       */
#define CSCS_CYCLING_SPEED_AND_CADENCE_CONTROL_POINT_VALUE_LENGTH                               (BYTE_SIZE)

   /* The following defines the valid CSC Feature Bit masks that	*/
   /* specifies the supported features of the Server			*/
#define CSCS_CSC_FEATURE_BIT_MASK_WHEEL_REVOLUTION_DATA_SUPPORTED                               0x01
#define CSCS_CSC_FEATURE_BIT_MASK_CRANK_REVOLUTION_DATA_SUPPORTED                               0x02
#define CSCS_CSC_FEATURE_BIT_MASK_MULTIPLE_SENSOR_LOCATION_SUPPORTED                            0x04

   /* The following MACRO is a utility MACRO that exists to valid that a*/
   /* specified Sensor Feature is valid.  The only parameter to this  	*/
   /* function is the Sensor Feature to valid.  This MACRO returns	*/
   /* TRUE if the Sensor Feature is valid or FALSE otherwis.          	*/
#define CSCS_SENSOR_FEATURES_VALID_TYPE(_x)                                                     (((Word_t)(_x)) & (CSCS_CSC_FEATURE_BIT_MASK_WHEEL_REVOLUTION_DATA_SUPPORTED | CSCS_CSC_FEATURE_BIT_MASK_CRANK_REVOLUTION_DATA_SUPPORTED | CSCS_CSC_FEATURE_BIT_MASK_MULTIPLE_SENSOR_LOCATION_SUPPORTED))
#define IS_SC_CONTROL_POINT_SUPPORTED(_x)                                                       ((((Word_t)(_x)) & CSCS_CSC_FEATURE_BIT_MASK_WHEEL_REVOLUTION_DATA_SUPPORTED) || (((Word_t)(_x)) & CSCS_CSC_FEATURE_BIT_MASK_MULTIPLE_SENSOR_LOCATION_SUPPORTED))
#define IS_WHEEL_REVOLUTION_DATA_SUPPORTED(_x)                                                  (((Word_t)(_x)) & CSCS_CSC_FEATURE_BIT_MASK_WHEEL_REVOLUTION_DATA_SUPPORTED)
#define IS_CRANK_REVOLUTION_DATA_SUPPORTED(_x)                                                  (((Word_t)(_x)) & CSCS_CSC_FEATURE_BIT_MASK_CRANK_REVOLUTION_DATA_SUPPORTED)
#define IS_MULTIPLE_SENSOR_LOCATION_SUPPORTED(_x)                                               (((Word_t)(_x)) & CSCS_CSC_FEATURE_BIT_MASK_MULTIPLE_SENSOR_LOCATION_SUPPORTED)

   /* The following MACRO is a utility MACRO that exists to calculate   */
   /* the instantaneous speed, The Calculation is derived from wheel    */
   /* circumference and two successive measurements. The first two      */
   /* Parameters are successice Cumulative Wheel Revolution values,     */
   /* Third Parameter is the wheel circumference, and last to parameters*/
   /* two successive Last Wheel Event Time Values. This MACRO returns   */                      
   /* The value of the calculation in km/h                              */
#define INSTANTANEOUS_SPEED(_Delta_Cumulative,_d,_Delta_Event_Time)                             ((((_Delta_Cumulative)*((double)(_d)))/((double)(_Delta_Event_Time)))*3.6*1024/100) //3.6=m/s->km/h, 1.024 = 1024(1024ticks/sec -> s ) / 100 (wheel circumference cm->m)

   /* The following MACRO is a utility MACRO that exists to calculate   */
   /* the instantaneous cadence, The Calculation is derived from two    */
   /* successive measurements. The first two Parameters are successice  */
   /* Cumulative Crank Revolution values, and last to parameters two    */
   /* successive Last Crank Event Time Values.                          */
   /* This MACRO returns The value of the calculation in rpm            */                      
#define INSTANTANEOUS_CADENCE(_Delta_Cumulative,_Delta_Event_Time)                              (((_Delta_Cumulative)/((double)(_Delta_Event_Time)))*60*1024) // 60 = 60sec -> 1min, 1024 = 1024ticks/sec -> s

   /* The following defines the length of the Sensor Location		*/
   /* characteristic value.                                             */
#define CSCS_CSC_FEATURE_VALUE_LENGTH                                                           (WORD_SIZE)

   /* The following defines the CSCS GATT Service Flags MASK that should*/
   /* be passed into GATT_Register_Service when the CSCS Service is     */
   /* registered.                                                       */
#define CSCS_SERVICE_FLAGS_LE_CONNECTION                                                        (GATT_SERVICE_FLAGS_LE_SERVICE)
#define CSCS_SERVICE_FLAGS_BR_EDR_CONNECTION                                                    (GATT_SERVICE_FLAGS_BR_EDR_SERVICE)

#endif
