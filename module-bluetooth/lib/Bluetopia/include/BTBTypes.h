/*****< btbtypes.h >***********************************************************/
/*      Copyright 2000 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*      Copyright 2015 Texas Instruments Incorporated.                        */
/*      All Rights Reserved.                                                  */
/*									      									  */	
/*  BTBTYPES - Common Bluetooth Base Defined Types.                           */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   08/20/00  D. Lange       Initial creation.                               */
/*   12/07/07  D. Mason       Changes for BT 2.1                              */
/*   12/10/15  D. Keren       Adding macro for copying BD addresses           */
/******************************************************************************/
#ifndef __BTTYPESH_INC__
#error "BTBTypes.h should never be used directly.  Include BTTypes.h, instead."
#endif

#ifndef __BTBTYPESH__
#define __BTBTYPESH__

   /* The following type declaration represents the structure of a      */
   /* single Bluetooth Board Address.                                   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagBD_ADDR_t
{
   Byte_t BD_ADDR0;
   Byte_t BD_ADDR1;
   Byte_t BD_ADDR2;
   Byte_t BD_ADDR3;
   Byte_t BD_ADDR4;
   Byte_t BD_ADDR5;
} __PACKED_STRUCT_END__ BD_ADDR_t;

#define BD_ADDR_SIZE                            (sizeof(BD_ADDR_t))

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified BD_ADDR variable.   */
   /* The Bytes are NOT in Little Endian Format, however, they are      */
   /* assigned to the BD_ADDR variable in Little Endian Format.  The    */
   /* first parameter is the BD_ADDR variable (of type BD_ADDR_t) to    */
   /* assign, and the next six parameters are the Individual BD_ADDR    */
   /* Byte values to assign to the variable.                            */
#define ASSIGN_BD_ADDR(_dest, _a, _b, _c, _d, _e, _f) \
{                                                     \
   (_dest).BD_ADDR0 = (_f);                           \
   (_dest).BD_ADDR1 = (_e);                           \
   (_dest).BD_ADDR2 = (_d);                           \
   (_dest).BD_ADDR3 = (_c);                           \
   (_dest).BD_ADDR4 = (_b);                           \
   (_dest).BD_ADDR5 = (_a);                           \
}

/* The following MACRO is a utility MACRO that exists to copy       */
/* BD_ADDR variable into the another specified BD_ADDR variable.    */
/* The Bytes are NOT in Little Endian Format, however, they are     */
/* assigned to the BD_ADDR variable in Little Endian Format.  The   */
/* first parameter is the BD_ADDR variable (of type BD_ADDR_t) to   */
/* assign, and the next parameter is the source BD_ADDR variable.   */
#define COPY_BD_ADDR(_dest, _src) \
{                                                     \
(_dest).BD_ADDR0 = (_src).BD_ADDR0;                \
(_dest).BD_ADDR1 = (_src).BD_ADDR1;                \
(_dest).BD_ADDR2 = (_src).BD_ADDR2;                \
(_dest).BD_ADDR3 = (_src).BD_ADDR3;                \
(_dest).BD_ADDR4 = (_src).BD_ADDR4;                \
(_dest).BD_ADDR5 = (_src).BD_ADDR5;                \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two BD_ADDR_t variables.  This MACRO only returns   */
   /* whether the two BD_ADDR_t variables are equal (MACRO returns      */
   /* boolean result) NOT less than/greater than.  The two parameters to*/
   /* this MACRO are both of type BD_ADDR_t and represent the BD_ADDR_t */
   /* variables to compare.                                             */
#define COMPARE_BD_ADDR(_x, _y) (((_x).BD_ADDR0 == (_y).BD_ADDR0) && ((_x).BD_ADDR1 == (_y).BD_ADDR1) && ((_x).BD_ADDR2 == (_y).BD_ADDR2) && ((_x).BD_ADDR3 == (_y).BD_ADDR3) && ((_x).BD_ADDR4 == (_y).BD_ADDR4) && ((_x).BD_ADDR5 == (_y).BD_ADDR5))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of a BD_ADDR_t variables to the NULL BD_ADDR. This     */
   /* MACRO only returns whether the the BD_ADDR_t variable is equal to */
   /* the NULL BD_ADDR (MACRO returns boolean result) NOT less          */
   /* than/greater than.  The parameter to this MACRO is the BD_ADDR_t  */
   /* structure to compare to the NULL BD_ADDR.                         */
#define COMPARE_NULL_BD_ADDR(_x) (((_x).BD_ADDR0 == 0x00) && ((_x).BD_ADDR1 == 0x00) && ((_x).BD_ADDR2 == 0x00) && ((_x).BD_ADDR3 == 0x00) && ((_x).BD_ADDR4 == 0x00) && ((_x).BD_ADDR5 == 0x00))

   /* The following type declaration represents the structure of a      */
   /* single LAP (Lower Address Part).                                  */
typedef __PACKED_STRUCT_BEGIN__ struct _tagLAP_t
{
   Byte_t LAP0;
   Byte_t LAP1;
   Byte_t LAP2;
} __PACKED_STRUCT_END__ LAP_t;

#define LAP_SIZE                                (sizeof(LAP_t))

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified LAP variable.  The  */
   /* Bytes are NOT in Little Endian Format, however, they are assigned */
   /* to the LAP variable in Little Endian Format.  The first parameter */
   /* is the LAP variable (of type LAP_t) to assign, and the next three */
   /* parameters are the Individual LAP values to assign to the         */
   /* variable.                                                         */
#define ASSIGN_LAP(_dest, _x, _y, _z) \
{                                     \
   (_dest).LAP0 = (_z);               \
   (_dest).LAP1 = (_y);               \
   (_dest).LAP2 = (_x);               \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two LAP_t variables.  This MACRO only returns       */
   /* whether the two LAP_t variables are equal (MACRO returns boolean  */
   /* result) NOT less than/greater than.  The two parameters to this   */
   /* MACRO are both of type LAP_t and represent the LAP_t variables to */
   /* compare.                                                          */
#define COMPARE_LAP(_x, _y) (((_x).LAP0 == (_y).LAP0) && ((_x).LAP1 == (_y).LAP1) && ((_x).LAP2 == (_y).LAP2))

   /* The following type declaration represents the structure of a      */
   /* single Link Key.                                                  */
typedef __PACKED_STRUCT_BEGIN__ struct _tagLink_Key_t
{
   Byte_t Link_Key0;
   Byte_t Link_Key1;
   Byte_t Link_Key2;
   Byte_t Link_Key3;
   Byte_t Link_Key4;
   Byte_t Link_Key5;
   Byte_t Link_Key6;
   Byte_t Link_Key7;
   Byte_t Link_Key8;
   Byte_t Link_Key9;
   Byte_t Link_Key10;
   Byte_t Link_Key11;
   Byte_t Link_Key12;
   Byte_t Link_Key13;
   Byte_t Link_Key14;
   Byte_t Link_Key15;
} __PACKED_STRUCT_END__ Link_Key_t;

#define LINK_KEY_SIZE                           (sizeof(Link_Key_t))

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified Link Key variable.  */
   /* The Bytes are NOT in Little Endian Format, however, they are      */
   /* assigned to the Link Key variable in Little Endian Format.  The   */
   /* first parameter is the Link Key variable (of type Link_Key_t) to  */
   /* assign, and the next 16 parameters are the Individual Link Key    */
   /* Byte values to assign to the Link Key variable.                   */
#define ASSIGN_LINK_KEY(_dest, _a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n, _o, _p) \
{                                                                                              \
   (_dest).Link_Key0  = (_p); (_dest).Link_Key1  = (_o); (_dest).Link_Key2  = (_n);            \
   (_dest).Link_Key3  = (_m); (_dest).Link_Key4  = (_l); (_dest).Link_Key5  = (_k);            \
   (_dest).Link_Key6  = (_j); (_dest).Link_Key7  = (_i); (_dest).Link_Key8  = (_h);            \
   (_dest).Link_Key9  = (_g); (_dest).Link_Key10 = (_f); (_dest).Link_Key11 = (_e);            \
   (_dest).Link_Key12 = (_d); (_dest).Link_Key13 = (_c); (_dest).Link_Key14 = (_b);            \
   (_dest).Link_Key15 = (_a);                                                                  \
}

/* The following MACRO is a utility MACRO that exists to Copy        */
/* the individual Byte values into the specified Link Key variable.  */
/* The Bytes are NOT in Little Endian Format, however, they are      */
/* assigned to the Link Key variable in Little Endian Format.  The   */
/* first parameter is the Link Key variable (of type Link_Key_t) to  */
/* assign, and the next 16 parameters are the Individual Link Key    */
/* Byte values to assign to the Link Key variable.                   */
#define COPY_LINK_KEY(_dst, _src)          \
{                                          \
    (_dst).Link_Key0  = (_src).Link_Key0 ; \
    (_dst).Link_Key1  = (_src).Link_Key1 ; \
    (_dst).Link_Key2  = (_src).Link_Key2 ; \
    (_dst).Link_Key3  = (_src).Link_Key3 ; \
    (_dst).Link_Key4  = (_src).Link_Key4 ; \
    (_dst).Link_Key5  = (_src).Link_Key5 ; \
    (_dst).Link_Key6  = (_src).Link_Key6 ; \
    (_dst).Link_Key7  = (_src).Link_Key7 ; \
    (_dst).Link_Key8  = (_src).Link_Key8 ; \
    (_dst).Link_Key9  = (_src).Link_Key9 ; \
    (_dst).Link_Key10 = (_src).Link_Key10; \
    (_dst).Link_Key11 = (_src).Link_Key11; \
    (_dst).Link_Key12 = (_src).Link_Key12; \
    (_dst).Link_Key13 = (_src).Link_Key13; \
    (_dst).Link_Key14 = (_src).Link_Key14; \
    (_dst).Link_Key15 = (_src).Link_Key15; \
}


   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two Link_Key_t variables.  This MACRO only returns  */
   /* whether the two Link_Key_t variables are equal (MACRO returns     */
   /* boolean result) NOT less than/greater than.  The two parameters to*/
   /* this MACRO are both of type Link_Key_t and represent the          */
   /* Link_Key_t variables to compare.                                  */
#define COMPARE_LINK_KEY(_x, _y)                                                                                           \
(                                                                                                                          \
   ((_x).Link_Key0  == (_y).Link_Key0)  && ((_x).Link_Key1  == (_y).Link_Key1)  && ((_x).Link_Key2  == (_y).Link_Key2)  && \
   ((_x).Link_Key3  == (_y).Link_Key3)  && ((_x).Link_Key4  == (_y).Link_Key4)  && ((_x).Link_Key5  == (_y).Link_Key5)  && \
   ((_x).Link_Key6  == (_y).Link_Key6)  && ((_x).Link_Key7  == (_y).Link_Key7)  && ((_x).Link_Key8  == (_y).Link_Key8)  && \
   ((_x).Link_Key9  == (_y).Link_Key9)  && ((_x).Link_Key10 == (_y).Link_Key10) && ((_x).Link_Key11 == (_y).Link_Key11) && \
   ((_x).Link_Key12 == (_y).Link_Key12) && ((_x).Link_Key13 == (_y).Link_Key13) && ((_x).Link_Key14 == (_y).Link_Key14) && \
   ((_x).Link_Key15 == (_y).Link_Key15)                                                                                    \
)

   /* The following type declaration represents the structure of a      */
   /* single PIN Code.                                                  */
typedef __PACKED_STRUCT_BEGIN__ struct _tagPIN_Code_t
{
   Byte_t PIN_Code0;
   Byte_t PIN_Code1;
   Byte_t PIN_Code2;
   Byte_t PIN_Code3;
   Byte_t PIN_Code4;
   Byte_t PIN_Code5;
   Byte_t PIN_Code6;
   Byte_t PIN_Code7;
   Byte_t PIN_Code8;
   Byte_t PIN_Code9;
   Byte_t PIN_Code10;
   Byte_t PIN_Code11;
   Byte_t PIN_Code12;
   Byte_t PIN_Code13;
   Byte_t PIN_Code14;
   Byte_t PIN_Code15;
} __PACKED_STRUCT_END__ PIN_Code_t;

#define PIN_CODE_SIZE                           (sizeof(PIN_Code_t))

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified PIN Code variable.  */
   /* The Bytes are NOT in Little Endian Format, and they are NOT       */
   /* assigned to the PIN Code variable in Little Endian Format.  The   */
   /* first parameter is the PIN Code variable (of type PIN_Code_t) to  */
   /* assign, and the next 16 parameters are the Individual PIN Code    */
   /* Byte values to assign to the PIN Code variable.                   */
#define ASSIGN_PIN_CODE(_dest, _a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n, _o, _p) \
{                                                                                              \
   (_dest).PIN_Code0  = (_a); (_dest).PIN_Code1  = (_b); (_dest).PIN_Code2  = (_c);            \
   (_dest).PIN_Code3  = (_d); (_dest).PIN_Code4  = (_e); (_dest).PIN_Code5  = (_f);            \
   (_dest).PIN_Code6  = (_g); (_dest).PIN_Code7  = (_h); (_dest).PIN_Code8  = (_i);            \
   (_dest).PIN_Code9  = (_j); (_dest).PIN_Code10 = (_k); (_dest).PIN_Code11 = (_l);            \
   (_dest).PIN_Code12 = (_m); (_dest).PIN_Code13 = (_n); (_dest).PIN_Code14 = (_o);            \
   (_dest).PIN_Code15 = (_p);                                                                  \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two PIN_Code_t variables.  This MACRO only returns  */
   /* whether the two PIN_Code_t variables are equal (MACRO returns     */
   /* boolean result) NOT less than/greater than.  The two parameters to*/
   /* this MACRO are both of type PIN_Code_t and represent the          */
   /* PIN_Code_t variables to compare.                                  */
#define COMPARE_PIN_CODE(_x, _y)                                                                                           \
(                                                                                                                          \
   ((_x).PIN_Code0  == (_y).PIN_Code0)  && ((_x).PIN_Code1  == (_y).PIN_Code1)  && ((_x).PIN_Code2  == (_y).PIN_Code2)  && \
   ((_x).PIN_Code3  == (_y).PIN_Code3)  && ((_x).PIN_Code4  == (_y).PIN_Code4)  && ((_x).PIN_Code5  == (_y).PIN_Code5)  && \
   ((_x).PIN_Code6  == (_y).PIN_Code6)  && ((_x).PIN_Code7  == (_y).PIN_Code7)  && ((_x).PIN_Code8  == (_y).PIN_Code8)  && \
   ((_x).PIN_Code9  == (_y).PIN_Code9)  && ((_x).PIN_Code10 == (_y).PIN_Code10) && ((_x).PIN_Code11 == (_y).PIN_Code11) && \
   ((_x).PIN_Code12 == (_y).PIN_Code12) && ((_x).PIN_Code13 == (_y).PIN_Code13) && ((_x).PIN_Code14 == (_y).PIN_Code14) && \
   ((_x).PIN_Code15 == (_y).PIN_Code15)                                                                                    \
)

   /* The following type declaration represents the structure of an     */
   /* Event Mask.                                                       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagEvent_Mask_t
{
   Byte_t Event_Mask0;
   Byte_t Event_Mask1;
   Byte_t Event_Mask2;
   Byte_t Event_Mask3;
   Byte_t Event_Mask4;
   Byte_t Event_Mask5;
   Byte_t Event_Mask6;
   Byte_t Event_Mask7;
} __PACKED_STRUCT_END__ Event_Mask_t;

#define EVENT_MASK_SIZE                         (sizeof(Event_Mask_t))

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified Event Mask variable.*/
   /* The Bytes are NOT in Little Endian Format, however, they are      */
   /* assigned to the Event Mask variable in Little Endian Format.  The */
   /* first parameter is the Event Mask variable (of type Event_Mask_t) */
   /* to assign, and the next eight parameters are the Individual Event */
   /* Mask Byte values to assign to the variable.                       */
#define ASSIGN_EVENT_MASK(_dest, _a, _b, _c, _d, _e, _f, _g, _h) \
{                                                                \
   (_dest).Event_Mask0 = (_h);                                   \
   (_dest).Event_Mask1 = (_g);                                   \
   (_dest).Event_Mask2 = (_f);                                   \
   (_dest).Event_Mask3 = (_e);                                   \
   (_dest).Event_Mask4 = (_d);                                   \
   (_dest).Event_Mask5 = (_c);                                   \
   (_dest).Event_Mask6 = (_b);                                   \
   (_dest).Event_Mask7 = (_a);                                   \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two Event_Mask_t variables.  This MACRO only returns*/
   /* whether the two Event_Mask_t variables are equal (MACRO returns   */
   /* boolean result) NOT less than/greater than.  The two parameters to*/
   /* this MACRO are both of type Event_Mask_t and represent the        */
   /* Event_Mask_t variables to compare.                                */
#define COMPARE_EVENT_MASK(_x, _y)                                                                                                 \
(                                                                                                                                  \
   ((_x).Event_Mask0 == (_y).Event_Mask0) && ((_x).Event_Mask1 == (_y).Event_Mask1) && ((_x).Event_Mask2  == (_y).Event_Mask2)  && \
   ((_x).Event_Mask3 == (_y).Event_Mask3) && ((_x).Event_Mask4 == (_y).Event_Mask4) && ((_x).Event_Mask5  == (_y).Event_Mask5)  && \
   ((_x).Event_Mask6 == (_y).Event_Mask6) && ((_x).Event_Mask7 == (_y).Event_Mask7)                                                \
)

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* setting of Event Bits in the Event Mask. This MACRO accepts as    */
   /* input the Event Mask variable (of type Event_Mask_t) as the first */
   /* parameter, and the bit number to set (LSB is bit number 0).  This */
   /* MACRO sets the bits in Little Endian Format (so bit 0 is          */
   /* physically bit 0 of the first byte of the Event Mask structure).  */
#define SET_EVENT_MASK_BIT(_x, _y)   \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] |= (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* clearing of Event Bits in the Event Mask. This MACRO accepts as   */
   /* input the Event Mask variable (of type Event_Mask_t) as the first */
   /* parameter, and the bit number to clear (LSB is bit number 0).     */
   /* This MACRO clears the bits in Little Endian Format (so bit 0 is   */
   /* physically bit 0 of the first byte of the Event Mask structure).  */
#define RESET_EVENT_MASK_BIT(_x, _y) \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] &= (Byte_t)~(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* testing of Event Bits in the Event Mask. This MACRO accepts as    */
   /* input the Event Mask variable (of type Event_Mask_t) as the first */
   /* parameter, and the bit number to test (LSB is bit number 0).      */
   /* This MACRO tests the bits in Little Endian Format (so bit 0 is    */
   /* physically bit 0 of the first byte of the Event Mask structure).  */
   /* This MACRO returns a boolean result as the test result.           */
#define TEST_EVENT_MASK_BIT(_x, _y)  \
   (((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] & (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8))))

   /* The following type declaration represents the structure of a      */
   /* Device Class Type.                                                */
typedef __PACKED_STRUCT_BEGIN__ struct _tagClass_of_Device_t
{
   Byte_t Class_of_Device0;
   Byte_t Class_of_Device1;
   Byte_t Class_of_Device2;
} __PACKED_STRUCT_END__ Class_of_Device_t;

#define CLASS_OF_DEVICE_SIZE                    (sizeof(Class_of_Device_t))

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified Class_of_Device_t   */
   /* variable.  The Bytes are NOT in Little Endian Format, however,    */
   /* they are assigned to the Class_of_Device variable in Little Endian*/
   /* Format.  The first parameter is the Class_of_Device variable (of  */
   /* type Class_of_Device_t) to assign, and the next three parameters  */
   /* are the Individual Class_of_Device_t values to assign to the      */
   /* variable.                                                         */
#define ASSIGN_CLASS_OF_DEVICE(_dest, _x, _y, _z) \
{                                                 \
   (_dest).Class_of_Device0 = (_z);               \
   (_dest).Class_of_Device1 = (_y);               \
   (_dest).Class_of_Device2 = (_x);               \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two Class_of_Device_t variables.  This MACRO only   */
   /* returns whether the two Class_of_Device_t variables are equal     */
   /* (MACRO returns boolean result) NOT less than/greater than.  The   */
   /* two parameters to this MACRO are both of type Class_of_Device_t   */
   /* and represent the Class_of_Device_t variables to compare.         */
#define COMPARE_CLASS_OF_DEVICE(_x, _y) (((_x).Class_of_Device0 == (_y).Class_of_Device0) && ((_x).Class_of_Device1 == (_y).Class_of_Device1) && ((_x).Class_of_Device2 == (_y).Class_of_Device2))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* setting of Class of Device Bits in the Class of Device Type.  This*/
   /* MACRO accepts as input the Class of Device variable (of type      */
   /* Class_of_Device_t) as the first parameter, and the bit number to  */
   /* set (LSB is bit number 0).  This MACRO sets the bits in Little    */
   /* Endian Format (so bit 0 is physically bit 0 of the first byte of  */
   /* the Class of Device structure).                                   */
#define SET_CLASS_OF_DEVICE_BIT(_x, _y)   \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] |= (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* clearing of Class of Device Bits in the Class of Device Type.     */
   /* This MACRO accepts as input the Class of Device variable (of type */
   /* Class_of_Device_t) as the first parameter, and the bit number to  */
   /* clear (LSB is bit number 0).  This MACRO clears the bits in Little*/
   /* Endian Format (so bit 0 is physically bit 0 of the first byte of  */
   /* the Class of Device structure).                                   */
#define RESET_CLASS_OF_DEVICE_BIT(_x, _y) \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] &= (Byte_t)~(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* testing of Class of Device Bits in the Class of Device Type.      */
   /* This MACRO accepts as input the Class of Device variable (of type */
   /* Class_of_Device_t) as the first parameter, and the bit number to  */
   /* test (LSB is bit number 0).  This MACRO tests the bits in Little  */
   /* Endian Format (so bit 0 is physically bit 0 of the first byte of  */
   /* the Class of Device structure).  This MACRO returns a boolean     */
   /* result ast he test result.                                        */
#define TEST_CLASS_OF_DEVICE_BIT(_x, _y)  \
   (((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] & (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8))))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* reading of the Class of Device Format Type Field.  This MACRO     */
   /* accepts as input the Class of Device variable (of type            */
   /* Class_of_Device_t) as the first parameter.  This MACRO returns    */
   /* the Class of Device Format Type Value which is Right Justisfied   */
   /* (Bit-wise) and is of type Byte_t (Note the value returned will    */
   /* at most only have the lowest two bits set because this is the`    */
   /* size of the Format Type Field.                                    */
#define GET_CLASS_OF_DEVICE_FORMAT_TYPE(_x)                     \
   ((Byte_t)((_x).Class_of_Device0 & 0x03))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* setting of the Class of Device Format Type Field.  This MACRO     */
   /* accepts as input the Class of Device variable (of type            */
   /* Class_of_Device_t) as the first parameter and the Format Type     */
   /* Right Justified (Bit-Wise) to set (Note only the lower two bits   */
   /* of the Format Type are used when setting the Format Type).        */
#define SET_CLASS_OF_DEVICE_FORMAT_TYPE(_x, _y)                 \
   ((_x).Class_of_Device0 = ((_x).Class_of_Device0 & 0xFC) | ((_y) & 0x03))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* reading of the Class of Device Major Service Class Field.  This   */
   /* MACRO accepts as input the Class of Device variable (of type      */
   /* Class_of_Device_t) as the first parameter.  This MACRO returns    */
   /* the Class of Device Major Service Class Field Value which is Right*/
   /* Justisfied (Bit-wise) and is of type Word_t (Note the value       */
   /* returned will at most only have the lowest eleven bits set because*/
   /* this is the size of the Major Service Class Field.                */
#define GET_MAJOR_SERVICE_CLASS(_x)                                   \
  ((Word_t)(((_x).Class_of_Device2 << 0x03) | ((_x).Class_of_Device1 >> 0x05)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* setting of the Class of Device Major Service Class Field.  This   */
   /* MACRO accepts as input the Class of Device variable (of type      */
   /* Class_of_Device_t) as the first parameter and the Major Service   */
   /* Class Right Justified (Bit-Wise) to set (Note only the lower      */
   /* eleven bits of the Major Service Class are used when setting the  */
   /* Major Service Class).                                             */
#define SET_MAJOR_SERVICE_CLASS(_x, _y)                               \
{                                                                     \
   (_x).Class_of_Device2  = (Byte_t)(((_y) & 0x7FF) >> 0x03);         \
   (_x).Class_of_Device1 &= (Byte_t)0x1F;                             \
   (_x).Class_of_Device1 |= (Byte_t)((((_y) & 0xFF) << 0x05) & 0xFF); \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* reading of the Class of Device Major Device Class Field.  This    */
   /* MACRO accepts as input the Class of Device variable (of type      */
   /* Class_of_Device_t) as the first parameter.  This MACRO returns    */
   /* the Class of Device Major Device Class Field Value which is Right */
   /* Justisfied (Bit-wise) and is of type Byte_t (Note the value       */
   /* returned will at most only have the lowest five bits set because  */
   /* this is the size of the Major Device Class Field.                 */
#define GET_MAJOR_DEVICE_CLASS(_x)                                    \
   ((Byte_t)((_x).Class_of_Device1 & 0x1F))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* setting of the Class of Device Major Device Class Field.  This    */
   /* MACRO accepts as input the Class of Device variable (of type      */
   /* Class_of_Device_t) as the first parameter and the Major Device    */
   /* Class Right Justified (Bit-Wise) to set (Note only the lower      */
   /* five bits of the Major Device Class are used when setting the     */
   /* Major Device Class).                                              */
#define SET_MAJOR_DEVICE_CLASS(_x, _y)                                \
{                                                                     \
   (_x).Class_of_Device1 &= 0xE0;                                     \
   (_x).Class_of_Device1 |= (Byte_t)((_y) & 0x1F);                    \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* reading of the Class of Device Minor Device Class Field.  This    */
   /* MACRO accepts as input the Class of Device variable (of type      */
   /* Class_of_Device_t) as the first parameter.  This MACRO returns    */
   /* the Class of Device Minor Device Class Field Value which is Right */
   /* Justisfied (Bit-wise) and is of type Byte_t (Note the value       */
   /* returned will at most only have the lowest six bits set because   */
   /* this is the size of the Minor Device Class Field.                 */
#define GET_MINOR_DEVICE_CLASS(_x)                                    \
   ((Byte_t)((_x).Class_of_Device0 >> 0x02))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* setting of the Class of Device Minor Device Class Field.  This    */
   /* MACRO accepts as input the Class of Device variable (of type      */
   /* Class_of_Device_t) as the first parameter and the Minor Device    */
   /* Class Right Justified (Bit-Wise) to set (Note only the lower      */
   /* six bits of the Minor Device Class are used when setting the      */
   /* Minor Device Class).                                              */
#define SET_MINOR_DEVICE_CLASS(_x, _y)                                \
{                                                                     \
   (_x).Class_of_Device0 &= (Byte_t)0x03;                             \
   (_x).Class_of_Device0 |= (Byte_t)(((_y) << 0x02) & 0xFC);          \
}

   /* The following type declaration represents the structure of a      */
   /* Supported Commands Type (Version 1.2).                            */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSupported_Commands_t
{
   Byte_t Supported_Commands0;
   Byte_t Supported_Commands1;
   Byte_t Supported_Commands2;
   Byte_t Supported_Commands3;
   Byte_t Supported_Commands4;
   Byte_t Supported_Commands5;
   Byte_t Supported_Commands6;
   Byte_t Supported_Commands7;
   Byte_t Supported_Commands8;
   Byte_t Supported_Commands9;
   Byte_t Supported_Commands10;
   Byte_t Supported_Commands11;
   Byte_t Supported_Commands12;
   Byte_t Supported_Commands13;
   Byte_t Supported_Commands14;
   Byte_t Supported_Commands15;
   Byte_t Supported_Commands16;
   Byte_t Supported_Commands17;
   Byte_t Supported_Commands18;
   Byte_t Supported_Commands19;
   Byte_t Supported_Commands20;
   Byte_t Supported_Commands21;
   Byte_t Supported_Commands22;
   Byte_t Supported_Commands23;
   Byte_t Supported_Commands24;
   Byte_t Supported_Commands25;
   Byte_t Supported_Commands26;
   Byte_t Supported_Commands27;
   Byte_t Supported_Commands28;
   Byte_t Supported_Commands29;
   Byte_t Supported_Commands30;
   Byte_t Supported_Commands31;
   Byte_t Supported_Commands32;
   Byte_t Supported_Commands33;
   Byte_t Supported_Commands34;
   Byte_t Supported_Commands35;
   Byte_t Supported_Commands36;
   Byte_t Supported_Commands37;
   Byte_t Supported_Commands38;
   Byte_t Supported_Commands39;
   Byte_t Supported_Commands40;
   Byte_t Supported_Commands41;
   Byte_t Supported_Commands42;
   Byte_t Supported_Commands43;
   Byte_t Supported_Commands44;
   Byte_t Supported_Commands45;
   Byte_t Supported_Commands46;
   Byte_t Supported_Commands47;
   Byte_t Supported_Commands48;
   Byte_t Supported_Commands49;
   Byte_t Supported_Commands50;
   Byte_t Supported_Commands51;
   Byte_t Supported_Commands52;
   Byte_t Supported_Commands53;
   Byte_t Supported_Commands54;
   Byte_t Supported_Commands55;
   Byte_t Supported_Commands56;
   Byte_t Supported_Commands57;
   Byte_t Supported_Commands58;
   Byte_t Supported_Commands59;
   Byte_t Supported_Commands60;
   Byte_t Supported_Commands61;
   Byte_t Supported_Commands62;
   Byte_t Supported_Commands63;
} __PACKED_STRUCT_END__ Supported_Commands_t;

#define SUPPORTED_COMMANDS_SIZE                 (sizeof(Supported_Commands_t))

   /* The following MACRO is a utility MACRO that exists to assign the  */
   /* individual Byte values into the specified Supported_Commands      */
   /* variable.  The Bytes are NOT in Little Endian Format, however,    */
   /* they are assigned to the Supported_Commands variable in Little    */
   /* Endian Format.  The first parameter is the Supported_Commands     */
   /* variable (of type Supported_Commands_t) to assign, and the next   */
   /* sixty-four parameters are the Individual Supported_Commands Byte  */
   /* values to assign to the variable.                                 */
#define ASSIGN_SUPPORTED_COMMANDS(_dest, _a0, _b0, _c0, _d0, _e0, _f0, _g0, _h0, _a1, _b1, _c1, _d1, _e1, _f1, _g1, _h1, _a2, _b2, _c2, _d2, _e2, _f2, _g2, _h2, _a3, _b3, _c3, _d3, _e3, _f3, _g3, _h3, _a4, _b4, _c4, _d4, _e4, _f4, _g4, _h4, _a5, _b5, _c5, _d5, _e5, _f5, _g5, _h5, _a6, _b6, _c6, _d6, _e6, _f6, _g6, _h6, _a7, _b7, _c7, _d7, _e7, _f7, _g7, _h7) \
{                                                                  \
   (_dest).Supported_Commands0  = (_h7);                                   \
   (_dest).Supported_Commands1  = (_g7);                                   \
   (_dest).Supported_Commands2  = (_f7);                                   \
   (_dest).Supported_Commands3  = (_e7);                                   \
   (_dest).Supported_Commands4  = (_d7);                                   \
   (_dest).Supported_Commands5  = (_c7);                                   \
   (_dest).Supported_Commands6  = (_b7);                                   \
   (_dest).Supported_Commands7  = (_a7);                                   \
   (_dest).Supported_Commands8  = (_h6);                                   \
   (_dest).Supported_Commands9  = (_g6);                                   \
   (_dest).Supported_Commands10 = (_f6);                                   \
   (_dest).Supported_Commands11 = (_e6);                                   \
   (_dest).Supported_Commands12 = (_d6);                                   \
   (_dest).Supported_Commands13 = (_c6);                                   \
   (_dest).Supported_Commands14 = (_b6);                                   \
   (_dest).Supported_Commands15 = (_a6);                                   \
   (_dest).Supported_Commands16 = (_h5);                                   \
   (_dest).Supported_Commands17 = (_g5);                                   \
   (_dest).Supported_Commands18 = (_f5);                                   \
   (_dest).Supported_Commands19 = (_e5);                                   \
   (_dest).Supported_Commands20 = (_d5);                                   \
   (_dest).Supported_Commands21 = (_c5);                                   \
   (_dest).Supported_Commands22 = (_b5);                                   \
   (_dest).Supported_Commands23 = (_a5);                                   \
   (_dest).Supported_Commands24 = (_h4);                                   \
   (_dest).Supported_Commands25 = (_g4);                                   \
   (_dest).Supported_Commands26 = (_f4);                                   \
   (_dest).Supported_Commands27 = (_e4);                                   \
   (_dest).Supported_Commands28 = (_d4);                                   \
   (_dest).Supported_Commands29 = (_c4);                                   \
   (_dest).Supported_Commands30 = (_b4);                                   \
   (_dest).Supported_Commands31 = (_a4);                                   \
   (_dest).Supported_Commands32 = (_h3);                                   \
   (_dest).Supported_Commands33 = (_g3);                                   \
   (_dest).Supported_Commands34 = (_f3);                                   \
   (_dest).Supported_Commands35 = (_e3);                                   \
   (_dest).Supported_Commands36 = (_d3);                                   \
   (_dest).Supported_Commands37 = (_c3);                                   \
   (_dest).Supported_Commands38 = (_b3);                                   \
   (_dest).Supported_Commands39 = (_a3);                                   \
   (_dest).Supported_Commands40 = (_h2);                                   \
   (_dest).Supported_Commands41 = (_g2);                                   \
   (_dest).Supported_Commands42 = (_f2);                                   \
   (_dest).Supported_Commands43 = (_e2);                                   \
   (_dest).Supported_Commands44 = (_d2);                                   \
   (_dest).Supported_Commands45 = (_c2);                                   \
   (_dest).Supported_Commands46 = (_b2);                                   \
   (_dest).Supported_Commands47 = (_a2);                                   \
   (_dest).Supported_Commands48 = (_h1);                                   \
   (_dest).Supported_Commands49 = (_g1);                                   \
   (_dest).Supported_Commands50 = (_f1);                                   \
   (_dest).Supported_Commands51 = (_e1);                                   \
   (_dest).Supported_Commands52 = (_d1);                                   \
   (_dest).Supported_Commands53 = (_c1);                                   \
   (_dest).Supported_Commands54 = (_b1);                                   \
   (_dest).Supported_Commands55 = (_a1);                                   \
   (_dest).Supported_Commands56 = (_h0);                                   \
   (_dest).Supported_Commands57 = (_g0);                                   \
   (_dest).Supported_Commands58 = (_f0);                                   \
   (_dest).Supported_Commands59 = (_e0);                                   \
   (_dest).Supported_Commands60 = (_d0);                                   \
   (_dest).Supported_Commands61 = (_c0);                                   \
   (_dest).Supported_Commands62 = (_b0);                                   \
   (_dest).Supported_Commands63 = (_a0);                                   \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two Supported_Commands_t variables.  This MACRO only*/
   /* returns whether the two Supported_Commands_t variables are equal  */
   /* (MACRO returns boolean result) NOT less than/greater than.  The   */
   /* two parameters to this MACRO are both of type Supported_Commands_t*/
   /* and represent the Supported_Commands_t variables to compare.      */
   /* * NOTE * Due to the size of the data types involved, it will      */
   /*          generate far less code to simply perform a Memory        */
   /*          Comparison instead of using this MACRO.                  */
#define COMPARE_SUPPORTED_COMMANDS(_x, _y)                                                                                                                                             \
(                                                                                                                                                                                      \
   ((_x).Supported_Commands0  == (_y).Supported_Commands0)  && ((_x).Supported_Commands1  == (_y).Supported_Commands1)  && ((_x).Supported_Commands2  == (_y).Supported_Commands2)  && \
   ((_x).Supported_Commands3  == (_y).Supported_Commands3)  && ((_x).Supported_Commands4  == (_y).Supported_Commands4)  && ((_x).Supported_Commands5  == (_y).Supported_Commands5)  && \
   ((_x).Supported_Commands6  == (_y).Supported_Commands6)  && ((_x).Supported_Commands7  == (_y).Supported_Commands7)                                                              && \
   ((_x).Supported_Commands8  == (_y).Supported_Commands8)  && ((_x).Supported_Commands9  == (_y).Supported_Commands9)  && ((_x).Supported_Commands10 == (_y).Supported_Commands10) && \
   ((_x).Supported_Commands11 == (_y).Supported_Commands11) && ((_x).Supported_Commands12 == (_y).Supported_Commands12) && ((_x).Supported_Commands13 == (_y).Supported_Commands13) && \
   ((_x).Supported_Commands14 == (_y).Supported_Commands14) && ((_x).Supported_Commands15 == (_y).Supported_Commands15)                                                             && \
   ((_x).Supported_Commands16 == (_y).Supported_Commands16) && ((_x).Supported_Commands17 == (_y).Supported_Commands17) && ((_x).Supported_Commands18 == (_y).Supported_Commands18) && \
   ((_x).Supported_Commands19 == (_y).Supported_Commands19) && ((_x).Supported_Commands20 == (_y).Supported_Commands20) && ((_x).Supported_Commands21 == (_y).Supported_Commands21) && \
   ((_x).Supported_Commands22 == (_y).Supported_Commands22) && ((_x).Supported_Commands23 == (_y).Supported_Commands23)                                                             && \
   ((_x).Supported_Commands24 == (_y).Supported_Commands24) && ((_x).Supported_Commands25 == (_y).Supported_Commands25) && ((_x).Supported_Commands26 == (_y).Supported_Commands26) && \
   ((_x).Supported_Commands27 == (_y).Supported_Commands27) && ((_x).Supported_Commands28 == (_y).Supported_Commands28) && ((_x).Supported_Commands29 == (_y).Supported_Commands29) && \
   ((_x).Supported_Commands30 == (_y).Supported_Commands30) && ((_x).Supported_Commands31 == (_y).Supported_Commands31)                                                             && \
   ((_x).Supported_Commands32 == (_y).Supported_Commands32) && ((_x).Supported_Commands33 == (_y).Supported_Commands33) && ((_x).Supported_Commands34 == (_y).Supported_Commands34) && \
   ((_x).Supported_Commands35 == (_y).Supported_Commands35) && ((_x).Supported_Commands36 == (_y).Supported_Commands36) && ((_x).Supported_Commands37 == (_y).Supported_Commands37) && \
   ((_x).Supported_Commands38 == (_y).Supported_Commands38) && ((_x).Supported_Commands39 == (_y).Supported_Commands39)                                                             && \
   ((_x).Supported_Commands40 == (_y).Supported_Commands40) && ((_x).Supported_Commands41 == (_y).Supported_Commands41) && ((_x).Supported_Commands42 == (_y).Supported_Commands42) && \
   ((_x).Supported_Commands43 == (_y).Supported_Commands43) && ((_x).Supported_Commands44 == (_y).Supported_Commands44) && ((_x).Supported_Commands45 == (_y).Supported_Commands45) && \
   ((_x).Supported_Commands46 == (_y).Supported_Commands46) && ((_x).Supported_Commands47 == (_y).Supported_Commands47)                                                             && \
   ((_x).Supported_Commands48 == (_y).Supported_Commands48) && ((_x).Supported_Commands49 == (_y).Supported_Commands49) && ((_x).Supported_Commands50 == (_y).Supported_Commands50) && \
   ((_x).Supported_Commands51 == (_y).Supported_Commands51) && ((_x).Supported_Commands52 == (_y).Supported_Commands52) && ((_x).Supported_Commands53 == (_y).Supported_Commands53) && \
   ((_x).Supported_Commands54 == (_y).Supported_Commands54) && ((_x).Supported_Commands55 == (_y).Supported_Commands55)                                                             && \
   ((_x).Supported_Commands56 == (_y).Supported_Commands56) && ((_x).Supported_Commands57 == (_y).Supported_Commands57) && ((_x).Supported_Commands58 == (_y).Supported_Commands58) && \
   ((_x).Supported_Commands59 == (_y).Supported_Commands59) && ((_x).Supported_Commands60 == (_y).Supported_Commands60) && ((_x).Supported_Commands61 == (_y).Supported_Commands61) && \
   ((_x).Supported_Commands62 == (_y).Supported_Commands62) && ((_x).Supported_Commands63 == (_y).Supported_Commands63)                                                                \
)

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* setting of Supported Commands Bits in the Supported Commands Mask.*/
   /* This MACRO accepts as input the Supported Commands Mask variable  */
   /* (of type Supported_Commands_t) as the first parameter, and the bit*/
   /* number to set (LSB is bit number 0).  This MACRO sets the bits in */
   /* Little Endian Format (so bit 0 is physically bit 0 of the first   */
   /* byte of the Supported Commands Mask structure).                   */
#define SET_SUPPORTED_COMMANDS_BIT(_x, _y)   \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] |= (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* clearing of Supported Commands Bits in the Supported Commands     */
   /* Mask.  This MACRO accepts as input the Supported Commands Mask    */
   /* variable (of type Supported_Commands_t) as the first parameter,   */
   /* and the bit number to clear (LSB is bit number 0).  This MACRO    */
   /* clears the bits in Little Endian Format (so bit 0 is physically   */
   /* bit 0 of the first byte of the Supported Commands Mask structure).*/
#define RESET_SUPPORTED_COMMANDS_BIT(_x, _y) \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] &= (Byte_t)~(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* testing of Supported Commands Bits in the Supported Commands Mask.*/
   /* This MACRO accepts as input the Supported Commands Mask variable  */
   /* (of type Supported_Commands_t) as the first parameter, and the bit*/
   /* number to test (LSB is bit number 0).  This MACRO tests the bits  */
   /* in Little Endian Format (so bit 0 is physically bit 0 of the first*/
   /* byte of the Supported Commands Mask structure).  This MACRO       */
   /* returns a boolean result as the test result.                      */
#define TEST_SUPPORTED_COMMANDS_BIT(_x, _y)  \
   (((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] & (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8))))

   /* The following type declaration represents the structure of a      */
   /* LMP Feature Type.                                                 */
typedef __PACKED_STRUCT_BEGIN__ struct _tagLMP_Features_t
{
   Byte_t LMP_Features0;
   Byte_t LMP_Features1;
   Byte_t LMP_Features2;
   Byte_t LMP_Features3;
   Byte_t LMP_Features4;
   Byte_t LMP_Features5;
   Byte_t LMP_Features6;
   Byte_t LMP_Features7;
} __PACKED_STRUCT_END__ LMP_Features_t;

#define LMP_FEATURES_SIZE                       (sizeof(LMP_Features_t))

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified LMP_Features        */
   /* variable.  The Bytes are NOT in Little Endian Format, however,    */
   /* they are assigned to the LMP_Features variable in Little Endian   */
   /* Format.  The first parameter is the LMP_Features variable (of type*/
   /* LMP_Features_t) to assign, and the next eight parameters are the  */
   /* Individual LMP_Features Byte values to assign to the variable.    */
#define ASSIGN_LMP_FEATURES(_dest, _a, _b, _c, _d, _e, _f, _g, _h) \
{                                                                  \
   (_dest).LMP_Features0 = (_h);                                   \
   (_dest).LMP_Features1 = (_g);                                   \
   (_dest).LMP_Features2 = (_f);                                   \
   (_dest).LMP_Features3 = (_e);                                   \
   (_dest).LMP_Features4 = (_d);                                   \
   (_dest).LMP_Features5 = (_c);                                   \
   (_dest).LMP_Features6 = (_b);                                   \
   (_dest).LMP_Features7 = (_a);                                   \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two LMP_Features_t variables.  This MACRO only      */
   /* returns whether the two LMP_Features_t variables are equal (MACRO */
   /* returns boolean result) NOT less than/greater than.  The two      */
   /* parameters to this MACRO are both of type LMP_Features_t and      */
   /* represent the LMP_Features_t variables to compare.                */
#define COMPARE_LMP_FEATURES(_x, _y)                                                                                                           \
(                                                                                                                                              \
   ((_x).LMP_Features0 == (_y).LMP_Features0) && ((_x).LMP_Features1 == (_y).LMP_Features1) && ((_x).LMP_Features2  == (_y).LMP_Features2)  && \
   ((_x).LMP_Features3 == (_y).LMP_Features3) && ((_x).LMP_Features4 == (_y).LMP_Features4) && ((_x).LMP_Features5  == (_y).LMP_Features5)  && \
   ((_x).LMP_Features6 == (_y).LMP_Features6) && ((_x).LMP_Features7 == (_y).LMP_Features7)                                                    \
)

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* setting of LMP Features Bits in the LMP Features Mask. This MACRO */
   /* accepts as input the LMP Features Mask variable (of type          */
   /* LMP_Features_t) as the first parameter, and the bit number to set */
   /* (LSB is bit number 0).  This MACRO sets the bits in Little Endian */
   /* Format (so bit 0 is physically bit 0 of the first byte of the     */
   /* LMP Features Mask structure).                                     */
#define SET_FEATURES_BIT(_x, _y)   \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] |= (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* clearing of LMP Features Bits in the LMP Features Mask. This MACRO*/
   /* accepts as input the LMP Features Mask variable (of type          */
   /* LMP_Features_t) as the first parameter, and the bit number to     */
   /* clear (LSB is bit number 0).  This MACRO clears the bits in       */
   /* Little Endian Format (so bit 0 is physically bit 0 of the first   */
   /* byte of the LMP Features Mask structure).                         */
#define RESET_FEATURES_BIT(_x, _y) \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] &= (Byte_t)~(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* testing of LMP Features Bits in the LMP Features Mask. This MACRO */
   /* accepts as input the LMP Features Mask variable (of type          */
   /* LMP_Features_t) as the first parameter, and the bit number to test*/
   /* (LSB is bit number 0).  This MACRO tests the bits in Little       */
   /* Endian Format (so bit 0 is physically bit 0 of the first byte of  */
   /* the LMP Features Mask structure).                                 */
   /* This MACRO returns a boolean result as the test result.           */
#define TEST_FEATURES_BIT(_x, _y)  \
   (((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] & (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8))))

   /* The following structure represents the format of the Inquiry      */
   /* Result Filter Type, Class of Device Condition Condition.  This    */
   /* Type is used with the Condition_t type declared below.            */
typedef __PACKED_STRUCT_BEGIN__ struct _tagInquiry_Result_Filter_Type_Class_of_Device_Condition_t
{
   Class_of_Device_t Class_of_Device;
   Class_of_Device_t Class_of_Device_Mask;
} __PACKED_STRUCT_END__ Inquiry_Result_Filter_Type_Class_of_Device_Condition_t;

#define INQUIRY_RESULT_FILTER_TYPE_CLASS_OF_DEVICE_CONDITION_SIZE       (sizeof(Inquiry_Result_Filter_Type_Class_of_Device_Condition_t))

   /* The following structure represents the format of the Inquiry      */
   /* Result Filter Type, BD_ADDR Condition Condition.  This Type is    */
   /* used with the Condition_t type declared below.                    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagInquiry_Result_Filter_Type_BD_ADDR_Condition_t
{
   BD_ADDR_t BD_ADDR;
} __PACKED_STRUCT_END__ Inquiry_Result_Filter_Type_BD_ADDR_Condition_t;

#define INQUIRY_RESULT_FILTER_TYPE_BD_ADDR_CONDITION_SIZE               (sizeof(Inquiry_Result_Filter_Type_BD_ADDR_Condition_t))

   /* The following structure represents the format of the Connection   */
   /* Setup Filter Type, All Devices Condition Condition.  This Type is */
   /* used with the Condition_t type declared below.                    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagConnection_Setup_Filter_Type_All_Devices_Condition_t
{
   Byte_t  Auto_Accept_Flag;
} __PACKED_STRUCT_END__ Connection_Setup_Filter_Type_All_Devices_Condition_t;

#define CONNECTION_SETUP_FILTER_TYPE_ALL_DEVICES_CONDITION_SIZE         (sizeof(Connection_Setup_Filter_Type_All_Devices_Condition_t))

   /* The following structure represents the format of the Connection   */
   /* Setup Filter Type, Class of Device Condition Condition.  This Type*/
   /* is used with the Condition_t type declared below.                 */
typedef __PACKED_STRUCT_BEGIN__ struct _tagConnection_Setup_Filter_Type_Class_of_Device_Condition_t
{
   Class_of_Device_t Class_of_Device;
   Class_of_Device_t Class_of_Device_Mask;
   Byte_t            Auto_Accept_Flag;
} __PACKED_STRUCT_END__ Connection_Setup_Filter_Type_Class_of_Device_Condition_t;

#define CONNECTION_SETUP_FILTER_TYPE_CLASS_OF_DEVICE_CONDITION_SIZE     (sizeof(Connection_Setup_Filter_Type_Class_of_Device_Condition_t))

   /* The following structure represents the format of the Connection   */
   /* Setup Filter Type, BD_ADDR Condition Condition.  This Type is     */
   /* used with the Condition_t type declared below.                    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagConnection_Setup_Filter_Type_BD_ADDR_Condition_t
{
   BD_ADDR_t BD_ADDR;
   Byte_t    Auto_Accept_Flag;
} __PACKED_STRUCT_END__ Connection_Setup_Filter_Type_BD_ADDR_Condition_t;

#define CONNECTION_SETUP_FILTER_TYPE_BD_ADDR_CONDITION_SIZE             (sizeof(Connection_Setup_Filter_Type_BD_ADDR_Condition_t))

   /* The following structure represents the format of the RAW Data     */
   /* Bytes contained in a Condition_t type variable.                   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagRaw_Condition_Bytes_t
{
   Byte_t Condition0;
   Byte_t Condition1;
   Byte_t Condition2;
   Byte_t Condition3;
   Byte_t Condition4;
   Byte_t Condition5;
   Byte_t Condition6;
} __PACKED_STRUCT_END__ Raw_Condition_Bytes_t;

#define RAW_CONDITION_SIZE                                              (sizeof(Raw_Condition_Bytes_t))

   /* The following type declaration represents the structure of a      */
   /* Filter Condition.                                                 */
typedef __PACKED_STRUCT_BEGIN__ struct _tagCondition_t
{
   __PACKED_STRUCT_BEGIN__ union
   {
      Inquiry_Result_Filter_Type_Class_of_Device_Condition_t   Inquiry_Result_Filter_Type_Class_of_Device_Condition;
      Inquiry_Result_Filter_Type_BD_ADDR_Condition_t           Inquiry_Result_Filter_Type_BD_ADDR_Condition;

      Connection_Setup_Filter_Type_All_Devices_Condition_t     Connection_Setup_Filter_Type_All_Devices_Condition;
      Connection_Setup_Filter_Type_Class_of_Device_Condition_t Connection_Setup_Filter_Type_Class_of_Device_Condition;
      Connection_Setup_Filter_Type_BD_ADDR_Condition_t         Connection_Setup_Filter_Type_BD_ADDR_Condition;

      Raw_Condition_Bytes_t                                    Raw_Condition_Bytes;
   } __PACKED_STRUCT_END__ Condition;
} __PACKED_STRUCT_END__ Condition_t;

#define MAX_CONDITION_SIZE                      (sizeof(Condition_t))

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified Condition variable. */
   /* The Bytes are NOT in Little Endian Format, however, they are      */
   /* assigned to the Condition variable in Little Endian Format.  The  */
   /* first parameter is the Condition variable (of type Condition_t)   */
   /* to assign, and the next seven parameters are the Individual       */
   /* Condition Byte values to assign to the variable.                  */
   /* * NOTE * This MACRO Only assigns RAW Condition Byte values to     */
   /*          the specified Condition.                                 */
#define ASSIGN_RAW_CONDITION(_dest, _a, _b, _c, _d, _e, _f, _g) \
{                                                               \
   (_dest).Condition.Raw_Condition_Bytes.Condition0 = (_g);     \
   (_dest).Condition.Raw_Condition_Bytes.Condition1 = (_f);     \
   (_dest).Condition.Raw_Condition_Bytes.Condition2 = (_e);     \
   (_dest).Condition.Raw_Condition_Bytes.Condition3 = (_d);     \
   (_dest).Condition.Raw_Condition_Bytes.Condition4 = (_c);     \
   (_dest).Condition.Raw_Condition_Bytes.Condition5 = (_b);     \
   (_dest).Condition.Raw_Condition_Bytes.Condition6 = (_a);     \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two Condition_t variables.  This MACRO only returns */
   /* whether the two Condition_t variables are equal (MACRO returns    */
   /* boolean result) NOT less than/greater than.  The two parameters to*/
   /* this MACRO are both of type Condition_t and represent the         */
   /* Condition_t variables to compare.                                 */
   /* * NOTE * This MACRO ONLY Compares the RAW Condition Bytes, so ALL */
   /*          Bytes MUST Match.                                        */
#define COMPARE_RAW_CONDITION(_x, _y)                                                                  \
(                                                                                                      \
   ((_x).Condition.Raw_Condition_Bytes.Condition0 == (_y).Condition.Raw_Condition_Bytes.Condition0) && \
   ((_x).Condition.Raw_Condition_Bytes.Condition1 == (_y).Condition.Raw_Condition_Bytes.Condition1) && \
   ((_x).Condition.Raw_Condition_Bytes.Condition2 == (_y).Condition.Raw_Condition_Bytes.Condition2) && \
   ((_x).Condition.Raw_Condition_Bytes.Condition3 == (_y).Condition.Raw_Condition_Bytes.Condition3) && \
   ((_x).Condition.Raw_Condition_Bytes.Condition4 == (_y).Condition.Raw_Condition_Bytes.Condition4) && \
   ((_x).Condition.Raw_Condition_Bytes.Condition5 == (_y).ondition.Raw_Condition_Bytes.CCondition5) && \
   ((_x).Condition.Raw_Condition_Bytes.Condition6 == (_y).Condition.Raw_Condition_Bytes.Condition6)    \
)

   /* The following type declaration represents the structure of a AFH  */
   /* Channel Map Type (Version 1.2).                                   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAFH_Channel_Map_t
{
   Byte_t AFH_Channel_Map0;
   Byte_t AFH_Channel_Map1;
   Byte_t AFH_Channel_Map2;
   Byte_t AFH_Channel_Map3;
   Byte_t AFH_Channel_Map4;
   Byte_t AFH_Channel_Map5;
   Byte_t AFH_Channel_Map6;
   Byte_t AFH_Channel_Map7;
   Byte_t AFH_Channel_Map8;
   Byte_t AFH_Channel_Map9;
} __PACKED_STRUCT_END__ AFH_Channel_Map_t;

#define AFH_CHANNEL_MAP_SIZE                    (sizeof(AFH_Channel_Map_t))

   /* The following MACRO is a utility MACRO that exists to assign the  */
   /* individual Byte values into the specified AFH_Channel_Map         */
   /* variable.  The Bytes specified are written directly into the AFH  */
   /* Channel Map in the order they are listed.  The first parameter is */
   /* the AFH_Channel_Map variable (of type AFH_Channel_Map_t) to       */
   /* assign, and the next ten parameters are the Individual            */
   /* AFH_Channel_Map Byte values to assign to the variable.            */
#define ASSIGN_AFH_CHANNEL_MAP(_dest, _a, _b, _c, _d, _e, _f, _g, _h, _i, _j) \
{                                                                             \
   (_dest).AFH_Channel_Map0 = (_j);                                           \
   (_dest).AFH_Channel_Map1 = (_i);                                           \
   (_dest).AFH_Channel_Map2 = (_h);                                           \
   (_dest).AFH_Channel_Map3 = (_g);                                           \
   (_dest).AFH_Channel_Map4 = (_f);                                           \
   (_dest).AFH_Channel_Map5 = (_e);                                           \
   (_dest).AFH_Channel_Map6 = (_d);                                           \
   (_dest).AFH_Channel_Map7 = (_c);                                           \
   (_dest).AFH_Channel_Map8 = (_b);                                           \
   (_dest).AFH_Channel_Map9 = (_a);                                           \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two AFH_Channel_Map_t variables.  This MACRO only   */
   /* returns whether the two AFH_Channel_Map_t variables are equal     */
   /* (MACRO returns boolean result) NOT less than/greater than.  The   */
   /* two parameters to this MACRO are both of type AFH_Channel_Map_t   */
   /* and represent the AFH_Channel_Map_t variables to compare.         */
#define COMPARE_AFH_CHANNEL_MAP(_x, _y)                                                                                                                        \
(                                                                                                                                                              \
   ((_x).AFH_Channel_Map0 == (_y).AFH_Channel_Map0) && ((_x).AFH_Channel_Map1 == (_y).AFH_Channel_Map1) && ((_x).AFH_Channel_Map2 == (_y).AFH_Channel_Map2) && \
   ((_x).AFH_Channel_Map3 == (_y).AFH_Channel_Map3) && ((_x).AFH_Channel_Map4 == (_y).AFH_Channel_Map4) && ((_x).AFH_Channel_Map5 == (_y).AFH_Channel_Map5) && \
   ((_x).AFH_Channel_Map6 == (_y).AFH_Channel_Map6) && ((_x).AFH_Channel_Map7 == (_y).AFH_Channel_Map7) && ((_x).AFH_Channel_Map8 == (_y).AFH_Channel_Map8) && \
   ((_x).AFH_Channel_Map9 == (_y).AFH_Channel_Map9)                                                                                                            \
)

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* setting of AFH Channel Map Channel in the AFH Channel Map.  This  */
   /* MACRO accepts as input the AFH Channel Map variable (of type      */
   /* AFH_Channel_Map_t) as the first parameter, and the Channel Number */
   /* to set.                                                           */
#define SET_AFH_CHANNEL_MAP_CHANNEL(_x, _y)   \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] |= (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* clearing of AFH Channel Map Channel in the AFH Channel Map.  This */
   /* MACRO accepts as input the AFH Channel Map variable (of type      */
   /* AFH_Channel_Map_t) as the first parameter, and the Channel Number */
   /* to clear.                                                         */
#define RESET_AFH_CHANNEL_MAP_CHANNEL(_x, _y) \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] &= (Byte_t)~(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* testing of AFH Channel Map Channel in the AFH Channel Map.  This  */
   /* MACRO accepts as input the AFH Channel Map variable (of type      */
   /* AFH_Channel_Map_t) as the first parameter, and the Channel Number */
   /* to test.                                                          */
#define TEST_AFH_CHANNEL_MAP_CHANNEL(_x, _y)  \
   (((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] & (1 << ((_y)%(sizeof(Byte_t)*8))))

   /* The following constant represents the Maximum Number of Bytes     */
   /* that a Name variable an occupy.  It should be noted that for      */
   /* Names that have lengths less than this maximum, a NULL terminating*/
   /* character should be used and IS counted as part of the Length.    */
   /* For a Name of 248 Bytes, there is NO NULL terminating character   */
   /* at the end of the Name Data.                                      */
#define MAX_NAME_LENGTH                                              248

   /* The following type declaration represents the structure of a      */
   /* Bluetooth Name Type.                                              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagBluetooth_Name_t
{
   Byte_t Name[MAX_NAME_LENGTH];
} __PACKED_STRUCT_END__ Bluetooth_Name_t;

#define BLUETOOTH_NAME_SIZE                     (sizeof(Bluetooth_Name_t))

   /* The following define determines the maximum fixed size that is    */
   /* used to define the extended inquiry response returned by the      */
   /* local device.  The size of this buffer is always fixed.           */
#define EXTENDED_INQUIRY_RESPONSE_DATA_MAXIMUM_SIZE                  240

   /* The following type defines the buffer that is used to write or    */
   /* read the extended inquiry response data to be returned by the     */
   /* local device.  The size of this buffer is always fixed.           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagExtended_Inquiry_Response_Data_t
{
   Byte_t Extended_Inquiry_Response_Data[EXTENDED_INQUIRY_RESPONSE_DATA_MAXIMUM_SIZE];
} __PACKED_STRUCT_END__ Extended_Inquiry_Response_Data_t;

#define EXTENDED_INQUIRY_RESPONSE_DATA_SIZE     (sizeof(Extended_Inquiry_Response_Data_t))

   /* This structure defines what each individual element within the    */
   /* Extended Inquiry Response Data looks like.  The first octet is the*/
   /* length of the of the data element, followed by the data type, and */
   /* finally the data itself.  See the Bluetooth Specification 2.1 +   */
   /* EDR, Volume 3, Part C, Section 8 for more information.            */
typedef __PACKED_STRUCT_BEGIN__ struct _tagExtended_Inquiry_Response_Data_Structure_Data_t
{
   Byte_t Length;
   Byte_t Data_Type;
   Byte_t Variable_Data[1];
} __PACKED_STRUCT_END__ Extended_Inquiry_Response_Data_Structure_Data_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an individual     */
   /* Extended Inquiry Response Data Structure Element based upon the   */
   /* lenght (in Bytes) of the variable data portion.  The first        */
   /* parameter to this MACRO is the Length (in Bytes) of the variable  */
   /* data contained in the individual Extended Inquiry Response Data   */
   /* Structure record.                                                 */
#define EXTENDED_INQUIRY_RESPONSE_DATA_STRUCTURE_DATA_SIZE(_x)       ((sizeof(Extended_Inquiry_Response_Data_Structure_Data_t) - sizeof(Byte_t)) + ((unsigned int)(_x)))

   /* The following type declaration represents the structure of an     */
   /* L2CAP Extended Event Mask.  In Memory, the structure will         */
   /* represent a 32 bit (4 octet) value that assigns Bit 0 to Bit 31   */
   /* from left to right.                                               */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Extended_Feature_Mask_t
{
   Byte_t Extended_Feature_Mask0;
   Byte_t Extended_Feature_Mask1;
   Byte_t Extended_Feature_Mask2;
   Byte_t Extended_Feature_Mask3;
} __PACKED_STRUCT_END__ L2CAP_Extended_Feature_Mask_t;

#define L2CAP_EXTENDED_FEATURE_MASK_SIZE        (sizeof(L2CAP_Extended_Feature_Mask_t))

   /* The following MACRO is a utility MACRO that exists to assign the  */
   /* individual Byte values into the specified Extended Feature Mask   */
   /* variable.  Extreme care must be taken when using this function to */
   /* manually assign values to this structure.  The data is saved in a */
   /* format where Mask 0 corresponds to octet 0 and Mask 3 corresponds */
   /* to octet 3.  Each octet is arranged so that the left most bit of  */
   /* the octet contains the LSB of the value.  The first parameter is  */
   /* the Extended Feature Mask variable (of type                       */
   /* L2CAP_Extended_Feature_Mask_t) to assign, and the next four       */
   /* parameters are the Individual Extended Feature Mask Byte values to*/
   /* assign to the variable.                                           */
#define ASSIGN_L2CAP_EXTENDED_FEATURE_MASK(_dest, _a, _b, _c, _d) \
{                                                                 \
   (_dest).Extended_Feature_Mask0 = (_a);                         \
   (_dest).Extended_Feature_Mask1 = (_b);                         \
   (_dest).Extended_Feature_Mask2 = (_c);                         \
   (_dest).Extended_Feature_Mask3 = (_d);                         \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two Extended_Feature_Mask_t variables.  This MACRO  */
   /* only returns whether the two Extended_Feature_Mask_t variables are*/
   /* equal (MACRO returns boolean result) NOT less than/greater than.  */
   /* The two parameters to this MACRO are both of type                 */
   /* L2CAP_Extended_Feature_Mask_t and represent the                   */
   /* Extended_Feature_Mask_t variables to compare.                     */
#define COMPARE_L2CAP_EXTENDED_FEATURE_MASK(_x, _y)                \
(                                                                  \
   ((_x).Extended_Feature_Mask0 == (_y).Extended_Feature_Mask0) && \
   ((_x).Extended_Feature_Mask1 == (_y).Extended_Feature_Mask1) && \
   ((_x).Extended_Feature_Mask2 == (_y).Extended_Feature_Mask2) && \
   ((_x).Extended_Feature_Mask3 == (_y).Extended_Feature_Mask3)    \
)

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* setting of L2CAP Extended Feature Bits in the Extended Feature    */
   /* Mask.  This MACRO accepts as input the Extended Feature Mask      */
   /* variable (of type L2CAP_Extended_Feature_Mask_t) as the first     */
   /* parameter, and the bit number to set.  Valid bit numbers range    */
   /* from 0 to 31 and it is the users responsibility to ensure that the*/
   /* value provided is valid.                                          */
#define SET_L2CAP_EXTENDED_FEATURE_MASK_BIT(_x, _y) \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] |= (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* resetting of L2CAP Extended Feature Bits in the Extended Feature  */
   /* Mask.  This MACRO accepts as input the Extended Feature Mask      */
   /* variable (of type L2CAP_Extended_Feature_Mask_t) as the first     */
   /* parameter, and the bit number to reset.  Valid bit numbers range  */
   /* from 0 to 31 and it is the users responsibility to ensure that the*/
   /* value provided is valid.                                          */
#define RESET_L2CAP_EXTENDED_FEATURE_MASK_BIT(_x, _y) \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] &= (Byte_t)~(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* testing the state of L2CAP Extended Feature Bits in the Extended  */
   /* Feature Mask.  This MACRO accepts as input the Extended Feature   */
   /* Mask variable (of type L2CAP_Extended_Feature_Mask_t) as the first*/
   /* parameter, and the bit number to test.  Valid bit numbers range   */
   /* from 0 to 31 and it is the users responsibility to ensure that the*/
   /* value provided is valid.  This MACRO returns a boolean result as  */
   /* the test result.                                                  */
#define TEST_L2CAP_EXTENDED_FEATURE_MASK_BIT(_x, _y) \
   (((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] & (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8))))

   /* The following type declaration represents the structure of an     */
   /* L2CAP Fixed Channels Supported Event Mask.  In Memory, the        */
   /* structure will represent a 64 bit (8 octet) value that assigns Bit*/
   /* 0 to Bit 63 from left to right.                                   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Fixed_Channels_Supported_Mask_t
{
   Byte_t Fixed_Channels_Supported_Mask0;
   Byte_t Fixed_Channels_Supported_Mask1;
   Byte_t Fixed_Channels_Supported_Mask2;
   Byte_t Fixed_Channels_Supported_Mask3;
   Byte_t Fixed_Channels_Supported_Mask4;
   Byte_t Fixed_Channels_Supported_Mask5;
   Byte_t Fixed_Channels_Supported_Mask6;
   Byte_t Fixed_Channels_Supported_Mask7;
} __PACKED_STRUCT_END__ L2CAP_Fixed_Channels_Supported_Mask_t;

#define L2CAP_FIXED_CHANNELS_SUPPORTED_MASK_SIZE     (sizeof(L2CAP_Fixed_Channels_Supported_Mask_t))

   /* The following MACRO is a utility MACRO that exists to assign the  */
   /* individual Byte values into the specified L2CAP Fixed Channels    */
   /* Supported Mask variable.  Extreme care must be taken when using   */
   /* this function to manually assign values to this structure.  The   */
   /* data is saved in a format where Mask 0 corresponds to octet 0 and */
   /* Mask 7 corresponds to octet 7.  Each octet is arranged so that the*/
   /* left most bit of the octet contains the LSB of the value.  The    */
   /* first parameter is the Supported Channels Mask variable (of type  */
   /* Fixed_Channels_Supported_Mask_t) to assign, and the next eight    */
   /* parameters are the Individual Supported Channel Mask Byte values  */
   /* to assign to the variable.                                        */
#define ASSIGN_L2CAP_FIXED_CHANNELS_SUPPORTED_MASK(_dest, _a, _b, _c, _d, _e, _f, _g, _h)  \
{                                                                                          \
   (_dest).Fixed_Channels_Supported_Mask0 = (_h);                                          \
   (_dest).Fixed_Channels_Supported_Mask1 = (_g);                                          \
   (_dest).Fixed_Channels_Supported_Mask2 = (_f);                                          \
   (_dest).Fixed_Channels_Supported_Mask3 = (_e);                                          \
   (_dest).Fixed_Channels_Supported_Mask4 = (_d);                                          \
   (_dest).Fixed_Channels_Supported_Mask5 = (_c);                                          \
   (_dest).Fixed_Channels_Supported_Mask6 = (_b);                                          \
   (_dest).Fixed_Channels_Supported_Mask7 = (_a);                                          \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two L2CAP_Fixed_Channels_Supported_t variables.     */
   /* This MACRO only returns whether the two variables are equal (MACRO*/
   /* returns boolean result) NOT less than/greater than.               */
#define COMPARE_L2CAP_FIXED_CHANNELS_SUPPORTED_MASK(_x, _y)                        \
(                                                                                  \
   ((_x).Fixed_Channels_Supported_Mask0 == (_y).Fixed_Channels_Supported_Mask0) && \
   ((_x).Fixed_Channels_Supported_Mask1 == (_y).Fixed_Channels_Supported_Mask1) && \
   ((_x).Fixed_Channels_Supported_Mask2 == (_y).Fixed_Channels_Supported_Mask2) && \
   ((_x).Fixed_Channels_Supported_Mask3 == (_y).Fixed_Channels_Supported_Mask3) && \
   ((_x).Fixed_Channels_Supported_Mask4 == (_y).Fixed_Channels_Supported_Mask4) && \
   ((_x).Fixed_Channels_Supported_Mask5 == (_y).Fixed_Channels_Supported_Mask5) && \
   ((_x).Fixed_Channels_Supported_Mask6 == (_y).Fixed_Channels_Supported_Mask6) && \
   ((_x).Fixed_Channels_Supported_Mask7 == (_y).Fixed_Channels_Supported_Mask7)    \
)

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* setting of Bits in the L2CAP_Fixed_Channels_Supported_t Mask.     */
   /* This MACRO accepts as input the Supported Channels Mask variable  */
   /* (of type L2CAP_Fixed_Channels_Supported_t) as the first parameter,*/
   /* and the bit number to set.  Valid bit numbers range from 0 to 63  */
   /* and it is the users responsibility to ensure that the value       */
   /* provided is valid.                                                */
#define SET_L2CAP_FIXED_CHANNELS_SUPPORTED_MASK_BIT(_x, _y) \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] |= (1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* resetting of L2CAP Extended Feature Bits in the L2CAP Extended    */
   /* Feature Mask.  This MACRO accepts as input the L2CAP Extended     */
   /* Feature Mask variable (of type L2CAP_Extended_Feature_Mask_t) as  */
   /* the first parameter, and the bit number to reset.  Valid bit      */
   /* numbers range from 0 to 31 and it is the users responsibility to  */
   /* ensure that the value provided is valid.                          */
#define RESET_L2CAP_FIXED_CHANNELS_SUPPORTED_MASK_BIT(_x, _y) \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] &= ~(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* testing the state of L2CAP Extended Feature Bits in the L2CAP     */
   /* Extended Feature Mask.  This MACRO accepts as input the L2CAP     */
   /* Extended Feature Mask variable (of type                           */
   /* L2CAP_Extended_Feature_Mask_t) as the first parameter, and the bit*/
   /* number to test.  Valid bit numbers range from 0 to 31 and it is   */
   /* the users responsibility to ensure that the value provided is     */
   /* valid.  This MACRO returns a boolean result as the test result.   */
#define TEST_L2CAP_FIXED_CHANNELS_SUPPORTED_MASK_BIT(_x, _y)  \
   (((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] & (1 << ((_y)%(sizeof(Byte_t)*8))))

   /* UUID Types/MACRO definitions.                                     */
   /* * NOTE * Two types of UUID MACRO's exist:                         */
   /*                                                                   */
   /*             - SDP UUID Types                                      */
   /*                                                                   */
   /*             - Bluetooth UUID Types                                */
   /*                                                                   */
   /*          The differences between the two are the way they are     */
   /*          stored in the type itself.  The SDP UUID Type MACRO's    */
   /*          store the actual UUID value in Big Endian format, whereas*/
   /*          the Bluetooth UUID Type MACRO's store the actual UUID    */
   /*          value in Little Endian format.  The same container type  */
   /*          is used for both (UUID_128_t, UUID_16_t, or UUID_32_t),  */
   /*          however.                                                 */
   /* * NOTE * Earlier versions of Bluetopia ONLY stored the UUID types */
   /*          in Big Endian format and used undecorated MACRO names.   */
   /*          These MACRO's were named:                                */
   /*             ASSIGN_UUID_128()                                     */
   /*             ASSIGN_UUID_16()                                      */
   /*             ASSIGN_UUID_32()                                      */
   /*             ASSIGN_UUID_16_TO_UUID_128()                          */
   /*             ASSIGN_UUID_32_TO_UUID_128()                          */
   /*          All of the above MACRO's have been deprecated and        */
   /*          replaced with a MACRO that specified either SDP or       */
   /*          BLUETOOTH in the name (e.g. ASSIGN_UUID_128() has been   */
   /*          replaced with ASSIGN_SDP_UUID_128() or                   */
   /*          ASSIGN_BLUETOOTH_UUID_128()) for consistency.            */

   /* The following type declaration represents the structure of a      */
   /* single 128 Bit Universally Unique Identifier (UUID).              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagUUID_128_t
{
   Byte_t UUID_Byte0;
   Byte_t UUID_Byte1;
   Byte_t UUID_Byte2;
   Byte_t UUID_Byte3;
   Byte_t UUID_Byte4;
   Byte_t UUID_Byte5;
   Byte_t UUID_Byte6;
   Byte_t UUID_Byte7;
   Byte_t UUID_Byte8;
   Byte_t UUID_Byte9;
   Byte_t UUID_Byte10;
   Byte_t UUID_Byte11;
   Byte_t UUID_Byte12;
   Byte_t UUID_Byte13;
   Byte_t UUID_Byte14;
   Byte_t UUID_Byte15;
} __PACKED_STRUCT_END__ UUID_128_t;

#define UUID_128_SIZE                           (sizeof(UUID_128_t))

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified UUID variable.      */
   /* The Bytes are NOT in Little Endian Format, and they are NOT       */
   /* assigned to the UUID variable in Little Endian Format.  The first */
   /* parameter is the UUID variable (of type UUID_128_t) to assign,    */
   /* and the next 16 parameters are the Individual UUID Byte values to */
   /* assign to the UUID variable.                                      */
#define ASSIGN_SDP_UUID_128(_dest, _a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n, _o, _p) \
{                                                                                                  \
   (_dest).UUID_Byte0  = (_a); (_dest).UUID_Byte1  = (_b); (_dest).UUID_Byte2  = (_c);             \
   (_dest).UUID_Byte3  = (_d); (_dest).UUID_Byte4  = (_e); (_dest).UUID_Byte5  = (_f);             \
   (_dest).UUID_Byte6  = (_g); (_dest).UUID_Byte7  = (_h); (_dest).UUID_Byte8  = (_i);             \
   (_dest).UUID_Byte9  = (_j); (_dest).UUID_Byte10 = (_k); (_dest).UUID_Byte11 = (_l);             \
   (_dest).UUID_Byte12 = (_m); (_dest).UUID_Byte13 = (_n); (_dest).UUID_Byte14 = (_o);             \
   (_dest).UUID_Byte15 = (_p);                                                                     \
}

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified UUID variable.      */
   /* The Bytes are NOT in Little Endian Format, however they ARE       */
   /* assigned to the UUID variable in Little Endian Format.  The first */
   /* parameter is the UUID variable (of type UUID_128_t) to assign,    */
   /* and the next 16 parameters are the Individual UUID Byte values to */
   /* assign to the UUID variable.                                      */
#define ASSIGN_BLUETOOTH_UUID_128(_dest, _a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n, _o, _p) \
{                                                                                                        \
   (_dest).UUID_Byte0  = (_p); (_dest).UUID_Byte1  = (_o); (_dest).UUID_Byte2  = (_n);                   \
   (_dest).UUID_Byte3  = (_m); (_dest).UUID_Byte4  = (_l); (_dest).UUID_Byte5  = (_k);                   \
   (_dest).UUID_Byte6  = (_j); (_dest).UUID_Byte7  = (_i); (_dest).UUID_Byte8  = (_h);                   \
   (_dest).UUID_Byte9  = (_g); (_dest).UUID_Byte10 = (_f); (_dest).UUID_Byte11 = (_e);                   \
   (_dest).UUID_Byte12 = (_d); (_dest).UUID_Byte13 = (_c); (_dest).UUID_Byte14 = (_b);                   \
   (_dest).UUID_Byte15 = (_a);                                                                           \
}

   /* The following MACRO is a utility MACRO that exists to convert     */
   /* an SDP 128 bit UUID (of type UUID_128_t) to a Bluetooth 128 bit   */
   /* UUID (of type UUID_128_t).  The first parameter of this MACRO is  */
   /* the 128 bit Bluetooth UUID variable (of type UUID_128_t) that will*/
   /* receive the converted value.  The second parameter is the 128 bit */
   /* SDP UUID variable (of type UUID_128_t) to convert.                */
   /* * NOTE * See Notes above about the difference between SDP and     */
   /*          Bluetooth UUID's (the endian-ness of each type).         */
#define CONVERT_SDP_UUID_128_TO_BLUETOOTH_UUID_128(_dest, _src)                                                                  \
{                                                                                                                                \
   (_dest).UUID_Byte0  = (_src).UUID_Byte15; (_dest).UUID_Byte1  = (_src).UUID_Byte14; (_dest).UUID_Byte2  = (_src).UUID_Byte13; \
   (_dest).UUID_Byte3  = (_src).UUID_Byte12; (_dest).UUID_Byte4  = (_src).UUID_Byte11; (_dest).UUID_Byte5  = (_src).UUID_Byte10; \
   (_dest).UUID_Byte6  = (_src).UUID_Byte9;  (_dest).UUID_Byte7  = (_src).UUID_Byte8;  (_dest).UUID_Byte8  = (_src).UUID_Byte7;  \
   (_dest).UUID_Byte9  = (_src).UUID_Byte6;  (_dest).UUID_Byte10 = (_src).UUID_Byte5;  (_dest).UUID_Byte11 = (_src).UUID_Byte4;  \
   (_dest).UUID_Byte12 = (_src).UUID_Byte3;  (_dest).UUID_Byte13 = (_src).UUID_Byte2;  (_dest).UUID_Byte14 = (_src).UUID_Byte1;  \
   (_dest).UUID_Byte15 = (_src).UUID_Byte0;                                                                                      \
}

   /* The following MACRO is a utility MACRO that exists to convert a   */
   /* Bluetooth 128 bit UUID (of type UUID_128_t) to an SDP 128 bit UUID*/
   /* (of type UUID_128_t).  The first parameter of this MACRO is the   */
   /* 128 bit SDP UUID variable (of type UUID_128_t) that will receive  */
   /* the converted value.  The second parameter is the 128 bit         */
   /* Bluetooth UUID (of type UUID_128_t) to convert.                   */
   /* * NOTE * See Notes above about the difference between SDP and     */
   /*          Bluetooth UUID's (the endian-ness of each type).         */
#define CONVERT_BLUETOOTH_UUID_128_TO_SDP_UUID_128(_dest, _src)                                                                  \
{                                                                                                                                \
   (_dest).UUID_Byte0  = (_src).UUID_Byte15; (_dest).UUID_Byte1  = (_src).UUID_Byte14; (_dest).UUID_Byte2  = (_src).UUID_Byte13; \
   (_dest).UUID_Byte3  = (_src).UUID_Byte12; (_dest).UUID_Byte4  = (_src).UUID_Byte11; (_dest).UUID_Byte5  = (_src).UUID_Byte10; \
   (_dest).UUID_Byte6  = (_src).UUID_Byte9;  (_dest).UUID_Byte7  = (_src).UUID_Byte8;  (_dest).UUID_Byte8  = (_src).UUID_Byte7;  \
   (_dest).UUID_Byte9  = (_src).UUID_Byte6;  (_dest).UUID_Byte10 = (_src).UUID_Byte5;  (_dest).UUID_Byte11 = (_src).UUID_Byte4;  \
   (_dest).UUID_Byte12 = (_src).UUID_Byte3;  (_dest).UUID_Byte13 = (_src).UUID_Byte2;  (_dest).UUID_Byte14 = (_src).UUID_Byte1;  \
   (_dest).UUID_Byte15 = (_src).UUID_Byte0;                                                                                      \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two UUID_128_t variables.  This MACRO only returns  */
   /* whether the two UUID_128_t variables are equal (MACRO returns     */
   /* boolean result) NOT less than/greater than.  The two parameters to*/
   /* this MACRO are both of type UUID_128_t and represent the          */
   /* UUID_128_t variables to compare.                                  */
#define COMPARE_UUID_128(_x, _y)                                                                                                 \
(                                                                                                                                \
   ((_x).UUID_Byte0  == (_y).UUID_Byte0)  && ((_x).UUID_Byte1  == (_y).UUID_Byte1)  && ((_x).UUID_Byte2  == (_y).UUID_Byte2)  && \
   ((_x).UUID_Byte3  == (_y).UUID_Byte3)  && ((_x).UUID_Byte4  == (_y).UUID_Byte4)  && ((_x).UUID_Byte5  == (_y).UUID_Byte5)  && \
   ((_x).UUID_Byte6  == (_y).UUID_Byte6)  && ((_x).UUID_Byte7  == (_y).UUID_Byte7)  && ((_x).UUID_Byte8  == (_y).UUID_Byte8)  && \
   ((_x).UUID_Byte9  == (_y).UUID_Byte9)  && ((_x).UUID_Byte10 == (_y).UUID_Byte10) && ((_x).UUID_Byte11 == (_y).UUID_Byte11) && \
   ((_x).UUID_Byte12 == (_y).UUID_Byte12) && ((_x).UUID_Byte13 == (_y).UUID_Byte13) && ((_x).UUID_Byte14 == (_y).UUID_Byte14) && \
   ((_x).UUID_Byte15 == (_y).UUID_Byte15)                                                                                        \
)

   /* The following type declaration represents the structure of a      */
   /* single 16 Bit Universally Unique Identifier (UUID).               */
typedef __PACKED_STRUCT_BEGIN__ struct _tagUUID_16_t
{
   Byte_t UUID_Byte0;
   Byte_t UUID_Byte1;
} __PACKED_STRUCT_END__ UUID_16_t;

#define UUID_16_SIZE                            (sizeof(UUID_16_t))

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified UUID variable.      */
   /* The Bytes are NOT in Little Endian Format, and they are NOT       */
   /* assigned to the UUID variable in Little Endian Format.  The first */
   /* parameter is the UUID variable (of type UUID_16_t) to assign,     */
   /* and the next 2 parameters are the Individual UUID Byte values to  */
   /* assign to the UUID variable.                                      */
#define ASSIGN_SDP_UUID_16(_dest, _a, _b)                  \
{                                                          \
   (_dest).UUID_Byte0  = (_a); (_dest).UUID_Byte1  = (_b); \
}

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified UUID variable.      */
   /* The Bytes are NOT in Little Endian Format, however they ARE       */
   /* assigned to the UUID variable in Little Endian Format.  The first */
   /* parameter is the UUID variable (of type UUID_16_t) to assign,     */
   /* and the next 2 parameters are the Individual UUID Byte values to  */
   /* assign to the UUID variable.                                      */
#define ASSIGN_BLUETOOTH_UUID_16(_dest, _a, _b)            \
{                                                          \
   (_dest).UUID_Byte0  = (_b); (_dest).UUID_Byte1  = (_a); \
}

   /* The following MACRO is a utility MACRO that exists to assign a    */
   /* 16 Bit UUID to the specified 128 BIT UUID.  The 16 Bit UUID is    */
   /* first expanded to a 32 Bit UUID, then the Most Significant 32 Bits*/
   /* of the specified 128 Bit UUID are changed to the newly defined    */
   /* 32 Bit UUID.  The Bytes in the 128 Bit UUID are NOT in Little     */
   /* Endian Format.  The first parameter is the 128 Bit UUID variable  */
   /* (of type UUID_128_t) to receive the 16 to 32 Bit UUID Value into, */
   /* and the second parameter is the 16 Bit UUID variable (of type     */
   /* UUID_16_t) to extend to a 32 Bit UUID, and then assign to the     */
   /* specified 128 Bit UUID.                                           */
#define ASSIGN_SDP_UUID_16_TO_SDP_UUID_128(_dest, _src)                             \
{                                                                                   \
   (_dest).UUID_Byte0 = 0;                  (_dest).UUID_Byte1 = 0;                 \
   (_dest).UUID_Byte2 = (_src).UUID_Byte0;  (_dest).UUID_Byte3 = (_src).UUID_Byte1; \
}

   /* The following MACRO is a utility MACRO that exists to assign a 16 */
   /* Bit UUID to the specified 128 BIT UUID.  The 16 Bit UUID is first */
   /* expanded to a 32 Bit UUID, then the Least Significant 32 Bits of  */
   /* the specified 128 Bit UUID are changed to the newly defined 32 Bit*/
   /* UUID.  The Bytes in the 128 Bit UUID ARE in Little Endian Format. */
   /* The first parameter is the 128 Bit UUID variable (of type         */
   /* UUID_128_t) to receive the 16 to 32 Bit UUID Value into, and the  */
   /* second parameter is the 16 Bit UUID variable (of type UUID_16_t)  */
   /* to extend to a 32 Bit UUID, and then assign to the specified 128  */
   /* Bit UUID.                                                         */
#define ASSIGN_BLUETOOTH_UUID_16_TO_BLUETOOTH_UUID_128(_dest, _src)                   \
{                                                                                     \
   (_dest).UUID_Byte12 = (_src).UUID_Byte0;  (_dest).UUID_Byte13 = (_src).UUID_Byte1; \
   (_dest).UUID_Byte14 = 0;                  (_dest).UUID_Byte15 = 0;                 \
}

   /* The following MACRO is a utility MACRO that exists to convert an  */
   /* SDP 16 bit UUID (of type UUID_16_t) to a Bluetooth 16 bit UUID (of*/
   /* type UUID_16_t).  The first parameter of this MACRO is the 16 bit */
   /* Bluetooth UUID variable (of type UUID_16_t) that will receive the */
   /* converted value.  The second parameter is the 16 bit SDP UUID (of */
   /* type UUID_16_t) to convert.                                       */
   /* * NOTE * See Notes above about the difference between SDP and     */
   /*          Bluetooth UUID's (the endian-ness of each type).         */
#define CONVERT_SDP_UUID_16_TO_BLUETOOTH_UUID_16(_dest, _src)                      \
{                                                                                  \
   (_dest).UUID_Byte0 = (_src).UUID_Byte1; (_dest).UUID_Byte1 = (_src).UUID_Byte0; \
}

   /* The following MACRO is a utility MACRO that exists to convert a   */
   /* Bluetooth 16 bit UUID (of type UUID_16_t) to an SDP 16 bit UUID   */
   /* (of type UUID_16_t).  The first parameter of this MACRO is the 16 */
   /* bit SDP UUID variable (of type UUID_16_t) that will receive the   */
   /* converted value.  The second parameter is the 16 bit Bluetooth    */
   /* UUID (of type UUID_16_t) to convert.                              */
   /* * NOTE * See Notes above about the difference between SDP and     */
   /*          Bluetooth UUID's (the endian-ness of each type).         */
#define CONVERT_BLUETOOTH_UUID_16_TO_SDP_UUID_16(_dest, _src)                      \
{                                                                                  \
   (_dest).UUID_Byte0 = (_src).UUID_Byte1; (_dest).UUID_Byte1 = (_src).UUID_Byte0; \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two UUID_16_t variables.  This MACRO only returns   */
   /* whether the two UUID_16_t variables are equal (MACRO returns      */
   /* boolean result) NOT less than/greater than.  The two parameters to*/
   /* this MACRO are both of type UUID_16_t and represent the UUID_16_t */
   /* variables to compare.                                             */
#define COMPARE_UUID_16(_x, _y)                                                    \
(                                                                                  \
   ((_x).UUID_Byte0  == (_y).UUID_Byte0)  && ((_x).UUID_Byte1  == (_y).UUID_Byte1) \
)

   /* The following type declaration represents the structure of a      */
   /* single 32 Bit Universally Unique Identifier (UUID).               */
typedef __PACKED_STRUCT_BEGIN__ struct _tagUUID_32_t
{
   Byte_t UUID_Byte0;
   Byte_t UUID_Byte1;
   Byte_t UUID_Byte2;
   Byte_t UUID_Byte3;
} __PACKED_STRUCT_END__ UUID_32_t;

#define UUID_32_SIZE                            (sizeof(UUID_32_t))

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified UUID variable.      */
   /* The Bytes are NOT in Little Endian Format, and they are NOT       */
   /* assigned to the UUID variable in Little Endian Format.  The first */
   /* parameter is the UUID variable (of type UUID_32_t) to assign,     */
   /* and the next 4 parameters are the Individual UUID Byte values to  */
   /* assign to the UUID variable.                                      */
#define ASSIGN_SDP_UUID_32(_dest, _a, _b, _c, _d)         \
{                                                         \
   (_dest).UUID_Byte0  = (_a); (_dest).UUID_Byte1 = (_b); \
   (_dest).UUID_Byte2  = (_c); (_dest).UUID_Byte3 = (_d); \
}

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified UUID variable.      */
   /* The Bytes are NOT in Little Endian Format, however they ARE       */
   /* assigned to the UUID variable in Little Endian Format.  The first */
   /* parameter is the UUID variable (of type UUID_32_t) to assign,     */
   /* and the next 4 parameters are the Individual UUID Byte values to  */
   /* assign to the UUID variable.                                      */
#define ASSIGN_BLUETOOTH_UUID_32(_dest, _a, _b, _c, _d)   \
{                                                         \
   (_dest).UUID_Byte0  = (_d); (_dest).UUID_Byte1 = (_c); \
   (_dest).UUID_Byte2  = (_b); (_dest).UUID_Byte3 = (_a); \
}

   /* The following MACRO is a utility MACRO that exists to assign a    */
   /* 32 Bit UUID to the specified 128 BIT UUID.  The 32 Bit UUID is    */
   /* assigned to the Most Significant 32 Bits of the specified 128 Bit */
   /* UUID.  The Bytes in the 128 Bit UUID are NOT in Little Endian     */
   /* Format.  The first parameter is the 128 Bit UUID variable (of     */
   /* type UUID_128_t) to receive the 32 Bit UUID Value into, and the   */
   /* second parameter is the 32 Bit UUID variable (of type UUID_32_t)  */
   /* to assign to the specified 128 Bit UUID.                          */
#define ASSIGN_SDP_UUID_32_TO_SDP_UUID_128(_dest, _src)                             \
{                                                                                   \
   (_dest).UUID_Byte0 = (_src).UUID_Byte0;  (_dest).UUID_Byte1 = (_src).UUID_Byte1; \
   (_dest).UUID_Byte2 = (_src).UUID_Byte2;  (_dest).UUID_Byte3 = (_src).UUID_Byte3; \
}

   /* The following MACRO is a utility MACRO that exists to assign a 32 */
   /* Bit UUID to the specified 128 BIT UUID.  The 32 Bit UUID is       */
   /* assigned to the Least Significant 32 Bits of the specified 128 Bit*/
   /* UUID.  The Bytes in the 128 Bit UUID ARE in Little Endian Format. */
   /* The first parameter is the 128 Bit UUID variable (of type         */
   /* UUID_128_t) to receive the 32 Bit UUID Value into, and the second */
   /* parameter is the 32 Bit UUID variable (of type UUID_32_t) to      */
   /* assign to the specified 128 Bit UUID.                             */
#define ASSIGN_BLUETOOTH_UUID_32_TO_BLUETOOTH_UUID_128(_dest, _src)                   \
{                                                                                     \
   (_dest).UUID_Byte12 = (_src).UUID_Byte0;  (_dest).UUID_Byte13 = (_src).UUID_Byte1; \
   (_dest).UUID_Byte14 = (_src).UUID_Byte2;  (_dest).UUID_Byte15 = (_src).UUID_Byte3; \
}

   /* The following MACRO is a utility MACRO that exists to convert an  */
   /* SDP 32 bit UUID (of type UUID_32_t) to a Bluetooth 32 bit UUID (of*/
   /* type UUID_32_t).  The first parameter of this MACRO is the 32 bit */
   /* Bluetooth UUID variable (of type UUID_32_t) that will receive the */
   /* converted value.  The second parameter is the 32 bit SDP UUID (of */
   /* type UUID_32_t) to convert.                                       */
   /* * NOTE * See Notes above about the difference between SDP and     */
   /*          Bluetooth UUID's (the endian-ness of each type).         */
#define CONVERT_SDP_UUID_32_TO_BLUETOOTH_UUID_32(_dest, _src)                      \
{                                                                                  \
   (_dest).UUID_Byte0 = (_src).UUID_Byte3; (_dest).UUID_Byte1 = (_src).UUID_Byte2; \
   (_dest).UUID_Byte2 = (_src).UUID_Byte1; (_dest).UUID_Byte3 = (_src).UUID_Byte0; \
}

   /* The following MACRO is a utility MACRO that exists to convert a   */
   /* Bluetooth 32 bit UUID (of type UUID_32_t) to an SDP 32 bit UUID   */
   /* (of type UUID_32_t).  The first parameter of this MACRO is the 32 */
   /* bit SDP UUID (of type UUID_32_t) that will receive the converted  */
   /* value.  The second parameter is the 32 bit Bluetooth UUID variable*/
   /* (of type UUID_32_t) to convert.                                   */
   /* * NOTE * See Notes above about the difference between SDP and     */
   /*          Bluetooth UUID's (the endian-ness of each type).         */
#define CONVERT_BLUETOOTH_UUID_32_TO_SDP_UUID_32(_dest, _src)                      \
{                                                                                  \
   (_dest).UUID_Byte0 = (_src).UUID_Byte3; (_dest).UUID_Byte1 = (_src).UUID_Byte2; \
   (_dest).UUID_Byte2 = (_src).UUID_Byte1; (_dest).UUID_Byte3 = (_src).UUID_Byte0; \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two UUID_32_t variables.  This MACRO only returns   */
   /* whether the two UUID_32_t variables are equal (MACRO returns      */
   /* boolean result) NOT less than/greater than.  The two parameters to*/
   /* this MACRO are both of type UUID_32_t and represent the UUID_32_t */
   /* variables to compare.                                             */
#define COMPARE_UUID_32(_x, _y)                                                       \
(                                                                                     \
   ((_x).UUID_Byte0  == (_y).UUID_Byte0)  && ((_x).UUID_Byte1  == (_y).UUID_Byte1) && \
   ((_x).UUID_Byte2  == (_y).UUID_Byte2)  && ((_x).UUID_Byte3  == (_y).UUID_Byte3)    \
)

   /* The following type declaration represents the structure of a      */
   /* single Simple Pairing Hash, C used for OOB mechanism of Secure    */
   /* Simple Pairing (Version 2.1).                                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSimple_Pairing_Hash_t
{
   Byte_t Pairing_Hash0;
   Byte_t Pairing_Hash1;
   Byte_t Pairing_Hash2;
   Byte_t Pairing_Hash3;
   Byte_t Pairing_Hash4;
   Byte_t Pairing_Hash5;
   Byte_t Pairing_Hash6;
   Byte_t Pairing_Hash7;
   Byte_t Pairing_Hash8;
   Byte_t Pairing_Hash9;
   Byte_t Pairing_Hash10;
   Byte_t Pairing_Hash11;
   Byte_t Pairing_Hash12;
   Byte_t Pairing_Hash13;
   Byte_t Pairing_Hash14;
   Byte_t Pairing_Hash15;
} __PACKED_STRUCT_END__ Simple_Pairing_Hash_t;

#define SIMPLE_PAIRING_HASH_SIZE                (sizeof(Simple_Pairing_Hash_t))

   /* The following MACRO is a utility MACRO that exists to assign the  */
   /* individual Byte values into the specified Pairing Hash, C,        */
   /* variable.  The Bytes are NOT in Little Endian Format, however,    */
   /* they are assigned to the Pairing Hash variable in Little Endian   */
   /* Format.  The first parameter is the Simple Pairing Hash variable  */
   /* (of type Simple_Pairing_Hash_t) to assign, and the next 16        */
   /* parameters are the Individual Pairing Hash Byte values to assign  */
   /* to the Pairing variable.                                          */
#define ASSIGN_PAIRING_HASH(_dest, _a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n, _o, _p) \
{                                                                                              \
   (_dest).Pairing_Hash0  = (_p); (_dest).Pairing_Hash1  = (_o); (_dest).Pairing_Hash2  = (_n);            \
   (_dest).Pairing_Hash3  = (_m); (_dest).Pairing_Hash4  = (_l); (_dest).Pairing_Hash5  = (_k);            \
   (_dest).Pairing_Hash6  = (_j); (_dest).Pairing_Hash7  = (_i); (_dest).Pairing_Hash8  = (_h);            \
   (_dest).Pairing_Hash9  = (_g); (_dest).Pairing_Hash10 = (_f); (_dest).Pairing_Hash11 = (_e);            \
   (_dest).Pairing_Hash12 = (_d); (_dest).Pairing_Hash13 = (_c); (_dest).Pairing_Hash14 = (_b);            \
   (_dest).Pairing_Hash15 = (_a);                                                                  \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two Simple_Pairing_Hash_t variables.  This MACRO    */
   /* only returns whether the two Simple_Pairing_Hash_t variables are  */
   /* equal (MACRO returns boolean result) NOT less than/greater than.  */
   /* The two parameters to this MACRO are both of type                 */
   /* Simple_Pairing_Hash_t and represent the Simple_Pairing_Hash_t     */
   /* variables to compare.                                             */
#define COMPARE_PAIRING_HASH(_x, _y)                                                                                           \
(                                                                                                                          \
   ((_x).Pairing_Hash0  == (_y).Pairing_Hash0)  && ((_x).Pairing_Hash1  == (_y).Pairing_Hash1)  && ((_x).Pairing_Hash2  == (_y).Pairing_Hash2)  && \
   ((_x).Pairing_Hash3  == (_y).Pairing_Hash3)  && ((_x).Pairing_Hash4  == (_y).Pairing_Hash4)  && ((_x).Pairing_Hash5  == (_y).Pairing_Hash5)  && \
   ((_x).Pairing_Hash6  == (_y).Pairing_Hash6)  && ((_x).Pairing_Hash7  == (_y).Pairing_Hash7)  && ((_x).Pairing_Hash8  == (_y).Pairing_Hash8)  && \
   ((_x).Pairing_Hash9  == (_y).Pairing_Hash9)  && ((_x).Pairing_Hash10 == (_y).Pairing_Hash10) && ((_x).Pairing_Hash11 == (_y).Pairing_Hash11) && \
   ((_x).Pairing_Hash12 == (_y).Pairing_Hash12) && ((_x).Pairing_Hash13 == (_y).Pairing_Hash13) && ((_x).Pairing_Hash14 == (_y).Pairing_Hash14) && \
   ((_x).Pairing_Hash15 == (_y).Pairing_Hash15)                                                                                    \
)

   /* The following type declaration represents the structure of a      */
   /* single Simple Pairing Randomizer, R used for OOB mechanism of     */
   /* Secure Simple Pairing (Version 2.1).                              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSimple_Pairing_Randomizer_t
{
   Byte_t Pairing_Randomizer0;
   Byte_t Pairing_Randomizer1;
   Byte_t Pairing_Randomizer2;
   Byte_t Pairing_Randomizer3;
   Byte_t Pairing_Randomizer4;
   Byte_t Pairing_Randomizer5;
   Byte_t Pairing_Randomizer6;
   Byte_t Pairing_Randomizer7;
   Byte_t Pairing_Randomizer8;
   Byte_t Pairing_Randomizer9;
   Byte_t Pairing_Randomizer10;
   Byte_t Pairing_Randomizer11;
   Byte_t Pairing_Randomizer12;
   Byte_t Pairing_Randomizer13;
   Byte_t Pairing_Randomizer14;
   Byte_t Pairing_Randomizer15;
} __PACKED_STRUCT_END__ Simple_Pairing_Randomizer_t;

#define SIMPLE_PAIRING_RANDOMIZER_SIZE          (sizeof(Simple_Pairing_Randomizer_t))

   /* The following MACRO is a utility MACRO that exists to assign the  */
   /* individual Byte values into the specified Pairing Randomizer, R,  */
   /* variable.  The Bytes are NOT in Little Endian Format, however,    */
   /* they are assigned to the Pairing Randomizer Key variable in Little*/
   /* Endian Format.  The first parameter is the Pairing Randomizer     */
   /* variable (of type Simple_Pairing_Randomizer_t) to assign, and the */
   /* next 16 parameters are the Individual Pairing Randomizer Byte     */
   /* values to assign to the Pairing Randomizer variable.              */
#define ASSIGN_PAIRING_RANDOMIZER(_dest, _a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n, _o, _p) \
{                                                                                              \
   (_dest).Pairing_Randomizer0  = (_p); (_dest).Pairing_Randomizer1  = (_o); (_dest).Pairing_Randomizer2  = (_n);            \
   (_dest).Pairing_Randomizer3  = (_m); (_dest).Pairing_Randomizer4  = (_l); (_dest).Pairing_Randomizer5  = (_k);            \
   (_dest).Pairing_Randomizer6  = (_j); (_dest).Pairing_Randomizer7  = (_i); (_dest).Pairing_Randomizer8  = (_h);            \
   (_dest).Pairing_Randomizer9  = (_g); (_dest).Pairing_Randomizer10 = (_f); (_dest).Pairing_Randomizer11 = (_e);            \
   (_dest).Pairing_Randomizer12 = (_d); (_dest).Pairing_Randomizer13 = (_c); (_dest).Pairing_Randomizer14 = (_b);            \
   (_dest).Pairing_Randomizer15 = (_a);                                                                  \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two Simple_Pairing_Randomizer_t variables.  This    */
   /* MACRO only returns whether the two Simple_Pairing_Randomizer_t    */
   /* variables are equal (MACRO returns boolean result) NOT less than/ */
   /* greater than.  The two parameters to this MACRO are both of type  */
   /* Simple_Pairing_Randomizer_t and represent the                     */
   /* Simple_Pairing_Randomizer_t variables to compare.                 */
#define COMPARE_PAIRING_RANDOMIZER(_x, _y)                                                                                           \
(                                                                                                                          \
   ((_x).Pairing_Randomizer0  == (_y).Pairing_Randomizer0)  && ((_x).Pairing_Randomizer1  == (_y).Pairing_Randomizer1)  && ((_x).Pairing_Randomizer2  == (_y).Pairing_Randomizer2)  && \
   ((_x).Pairing_Randomizer3  == (_y).Pairing_Randomizer3)  && ((_x).Pairing_Randomizer4  == (_y).Pairing_Randomizer4)  && ((_x).Pairing_Randomizer5  == (_y).Pairing_Randomizer5)  && \
   ((_x).Pairing_Randomizer6  == (_y).Pairing_Randomizer6)  && ((_x).Pairing_Randomizer7  == (_y).Pairing_Randomizer7)  && ((_x).Pairing_Randomizer8  == (_y).Pairing_Randomizer8)  && \
   ((_x).Pairing_Randomizer9  == (_y).Pairing_Randomizer9)  && ((_x).Pairing_Randomizer10 == (_y).Pairing_Randomizer10) && ((_x).Pairing_Randomizer11 == (_y).Pairing_Randomizer11) && \
   ((_x).Pairing_Randomizer12 == (_y).Pairing_Randomizer12) && ((_x).Pairing_Randomizer13 == (_y).Pairing_Randomizer13) && ((_x).Pairing_Randomizer14 == (_y).Pairing_Randomizer14) && \
   ((_x).Pairing_Randomizer15 == (_y).Pairing_Randomizer15)                                                                                    \
)

   /* The following type declaration represents the structure of a LE   */
   /* Features Type (Version 4.0 + LE).                                 */
typedef __PACKED_STRUCT_BEGIN__ struct _tagLE_Features_t
{
   Byte_t LE_Features0;
   Byte_t LE_Features1;
   Byte_t LE_Features2;
   Byte_t LE_Features3;
   Byte_t LE_Features4;
   Byte_t LE_Features5;
   Byte_t LE_Features6;
   Byte_t LE_Features7;
} __PACKED_STRUCT_END__ LE_Features_t;

#define LE_FEATURES_SIZE                        (sizeof(LE_Features_t))

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified LE_Features         */
   /* variable.  The Bytes are NOT in Little Endian Format, however,    */
   /* they are assigned to the LE_Features variable in Little Endian    */
   /* Format.  The first parameter is the LE_Features variable (of type */
   /* LE_Features_t) to assign, and the next eight parameters are the   */
   /* Individual LE_Features Byte values to assign to the variable.     */
#define ASSIGN_LE_FEATURES(_dest, _a, _b, _c, _d, _e, _f, _g, _h) \
{                                                                 \
   (_dest).LE_Features0 = (_h);                                   \
   (_dest).LE_Features1 = (_g);                                   \
   (_dest).LE_Features2 = (_f);                                   \
   (_dest).LE_Features3 = (_e);                                   \
   (_dest).LE_Features4 = (_d);                                   \
   (_dest).LE_Features5 = (_c);                                   \
   (_dest).LE_Features6 = (_b);                                   \
   (_dest).LE_Features7 = (_a);                                   \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two LE_Features_t variables.  This MACRO only       */
   /* returns whether the two LE_Features_t variables are equal (MACRO  */
   /* returns boolean result) NOT less than/greater than.  The two      */
   /* parameters to this MACRO are both of type LE_Features_t and       */
   /* represent the LE_Features_t variables to compare.                 */
#define COMPARE_LE_FEATURES(_x, _y)                                                                                                      \
(                                                                                                                                        \
   ((_x).LE_Features0 == (_y).LE_Features0) && ((_x).LE_Features1 == (_y).LE_Features1) && ((_x).LE_Features2  == (_y).LE_Features2)  && \
   ((_x).LE_Features3 == (_y).LE_Features3) && ((_x).LE_Features4 == (_y).LE_Features4) && ((_x).LE_Features5  == (_y).LE_Features5)  && \
   ((_x).LE_Features6 == (_y).LE_Features6) && ((_x).LE_Features7 == (_y).LE_Features7)                                                  \
)

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* setting of LE Features Bits in the LE Features Mask. This MACRO   */
   /* accepts as input the LE Features Mask variable (of type           */
   /* LE_Features_t) as the first parameter, and the bit number to set  */
   /* (LSB is bit number 0).  This MACRO sets the bits in Little Endian */
   /* Format (so bit 0 is physically bit 0 of the first byte of the     */
   /* LE Features Mask structure).                                      */
#define SET_LE_FEATURES_BIT(_x, _y)   \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] |= (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* clearing of LE Features Bits in the LE Features Mask. This MACRO  */
   /* accepts as input the LE Features Mask variable (of type           */
   /* LE_Features_t) as the first parameter, and the bit number to      */
   /* clear (LSB is bit number 0).  This MACRO clears the bits in       */
   /* Little Endian Format (so bit 0 is physically bit 0 of the first   */
   /* byte of the LE Features Mask structure).                          */
#define RESET_LE_FEATURES_BIT(_x, _y) \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] &= (Byte_t)~(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* testing of LE Features Bits in the LE Features Mask. This MACRO   */
   /* accepts as input the LE Features Mask variable (of type           */
   /* LE_Features_t) as the first parameter, and the bit number to test */
   /* (LSB is bit number 0).  This MACRO tests the bits in Little       */
   /* Endian Format (so bit 0 is physically bit 0 of the first byte of  */
   /* the LE Features Mask structure).                                  */
   /* This MACRO returns a boolean result as the test result.           */
#define TEST_LE_FEATURES_BIT(_x, _y)  \
   (((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] & (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8))))

   /* The following define determines the maximum fixed size that is    */
   /* used to define the Advertising Data that can be sent/received in  */
   /* advertising packets for a device.                                 */
#define ADVERTISING_DATA_MAXIMUM_SIZE                                31

   /* The following type defines the buffer that is used to write or    */
   /* read the advertising data to be advertised by the local device    */
   /* (Version 4.0 + LE).  The size of this buffer is always fixed.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAdvertising_Data_t
{
   Byte_t Advertising_Data[ADVERTISING_DATA_MAXIMUM_SIZE];
} __PACKED_STRUCT_END__ Advertising_Data_t;

#define ADVERTISING_DATA_SIZE                   (sizeof(Advertising_Data_t))

   /* The following define determines the maximum fixed size that is    */
   /* used to define the Scan Response Data that can be sent/received in*/
   /* scanning packets for a device.                                    */
#define SCAN_RESPONSE_DATA_MAXIMUM_SIZE                              31

   /* The following type defines the buffer that is used to write or    */
   /* read the advertising data to be advertised by the local device    */
   /* (Version 4.0 + LE).  The size of this buffer is always fixed.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagScan_Response_Data_t
{
   Byte_t Scan_Response_Data[SCAN_RESPONSE_DATA_MAXIMUM_SIZE];
} __PACKED_STRUCT_END__ Scan_Response_Data_t;

#define SCAN_RESPONSE_DATA_SIZE                 (sizeof(Scan_Response_Data_t))

   /* The following type declaration represents the structure of an LE  */
   /* Channel Map Type (Version 4.0 + LE).                              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagLE_Channel_Map_t
{
   NonAlignedByte_t LE_Channel_Map0;
   NonAlignedByte_t LE_Channel_Map1;
   NonAlignedByte_t LE_Channel_Map2;
   NonAlignedByte_t LE_Channel_Map3;
   NonAlignedByte_t LE_Channel_Map4;
} __PACKED_STRUCT_END__ LE_Channel_Map_t;

#define LE_CHANNEL_MAP_SIZE                     (sizeof(LE_Channel_Map_t))

   /* The following MACRO is a utility MACRO that exists to assign the  */
   /* individual Byte values into the specified LE_Channel_Map          */
   /* variable.  The Bytes specified are written directly into the AFH  */
   /* Channel Map in the order they are listed.  The first parameter is */
   /* the LE_Channel_Map variable (of type LE_Channel_Map_t) to         */
   /* assign, and the next five parameters are the Individual           */
   /* LE_Channel_Map Byte values to assign to the variable.             */
#define ASSIGN_LE_CHANNEL_MAP(_dest, _a, _b, _c, _d, _e) \
{                                                        \
   (_dest).LE_Channel_Map0 = (_e);                       \
   (_dest).LE_Channel_Map1 = (_d);                       \
   (_dest).LE_Channel_Map2 = (_c);                       \
   (_dest).LE_Channel_Map3 = (_b);                       \
   (_dest).LE_Channel_Map4 = (_a);                       \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two LE_Channel_Map_t variables.  This MACRO only    */
   /* returns whether the two LE_Channel_Map_t variables are equal      */
   /* (MACRO returns boolean result) NOT less than/greater than.  The   */
   /* two parameters to this MACRO are both of type LE_Channel_Map_t    */
   /* and represent the LE_Channel_Map_t variables to compare.          */
#define COMPARE_LE_CHANNEL_MAP(_x, _y)                                                                 \
(                                                                                                      \
   ((_x).LE_Channel_Map0 == (_y).LE_Channel_Map0) && ((_x).LE_Channel_Map1 == (_y).LE_Channel_Map1) && \
   ((_x).LE_Channel_Map2 == (_y).LE_Channel_Map2) && ((_x).LE_Channel_Map3 == (_y).LE_Channel_Map3) && \
   ((_x).LE_Channel_Map4 == (_y).LE_Channel_Map4)                                                      \
)

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* setting of LE Channel Map Channel in the LE Channel Map.  This    */
   /* MACRO accepts as input the LE Channel Map variable (of type       */
   /* LE_Channel_Map_t) as the first parameter, and the Channel Number  */
   /* to set.                                                           */
#define SET_LE_CHANNEL_MAP_CHANNEL(_x, _y)   \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] |= (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* clearing of LE Channel Map Channel in the LE Channel Map.  This   */
   /* MACRO accepts as input the LE Channel Map variable (of type       */
   /* LE_Channel_Map_t) as the first parameter, and the Channel Number  */
   /* to clear.                                                         */
#define RESET_LE_CHANNEL_MAP_CHANNEL(_x, _y) \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] &= (Byte_t)~(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* testing of LE Channel Map Channel in the LE Channel Map.  This    */
   /* MACRO accepts as input the LE Channel Map variable (of type       */
   /* LE_Channel_Map_t) as the first parameter, and the Channel Number  */
   /* to test.                                                          */
#define TEST_LE_CHANNEL_MAP_CHANNEL(_x, _y)  \
   (((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] & (1 << ((_y)%(sizeof(Byte_t)*8))))

   /* The following type declaration represents the structure of a      */
   /* single Encryption Key (Version 4.0 + LE).                         */
typedef __PACKED_STRUCT_BEGIN__ struct _tagEncryption_Key_t
{
   Byte_t Encryption_Key0;
   Byte_t Encryption_Key1;
   Byte_t Encryption_Key2;
   Byte_t Encryption_Key3;
   Byte_t Encryption_Key4;
   Byte_t Encryption_Key5;
   Byte_t Encryption_Key6;
   Byte_t Encryption_Key7;
   Byte_t Encryption_Key8;
   Byte_t Encryption_Key9;
   Byte_t Encryption_Key10;
   Byte_t Encryption_Key11;
   Byte_t Encryption_Key12;
   Byte_t Encryption_Key13;
   Byte_t Encryption_Key14;
   Byte_t Encryption_Key15;
} __PACKED_STRUCT_END__ Encryption_Key_t;

#define ENCRYPTION_KEY_SIZE                     (sizeof(Encryption_Key_t))

   /* The following MACRO is a utility MACRO that exists to assign the  */
   /* individual Byte values into the specified Encryption Key variable.*/
   /* The Bytes are NOT in Little Endian Format, however, they are      */
   /* assigned to the Encryption Key variable in Little Endian Format.  */
   /* The first parameter is the Encryption Key variable (of type       */
   /* Encryption_Key_t) to assign, and the next 16 parameters are the   */
   /* Individual Encryption Key Byte values to assign to the Encryption */
   /* Key variable.                                                     */
#define ASSIGN_ENCRYPTION_KEY(_dest, _a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n, _o, _p)   \
{                                                                                                      \
   (_dest).Encryption_Key0  = (_p); (_dest).Encryption_Key1  = (_o); (_dest).Encryption_Key2  = (_n);  \
   (_dest).Encryption_Key3  = (_m); (_dest).Encryption_Key4  = (_l); (_dest).Encryption_Key5  = (_k);  \
   (_dest).Encryption_Key6  = (_j); (_dest).Encryption_Key7  = (_i); (_dest).Encryption_Key8  = (_h);  \
   (_dest).Encryption_Key9  = (_g); (_dest).Encryption_Key10 = (_f); (_dest).Encryption_Key11 = (_e);  \
   (_dest).Encryption_Key12 = (_d); (_dest).Encryption_Key13 = (_c); (_dest).Encryption_Key14 = (_b);  \
   (_dest).Encryption_Key15 = (_a);                                                                    \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two Encryption_Key_t variables.  This MACRO only    */
   /* returns whether the two Encryption_Key_t variables are equal      */
   /* (MACRO returns boolean result) NOT less than/greater than.  The   */
   /* two parameters to this MACRO are both of type Encryption_Key_t and*/
   /* represent the Encryption_Key_t variables to compare.              */
#define COMPARE_ENCRYPTION_KEY(_x, _y)                                                                                                                         \
(                                                                                                                                                              \
   ((_x).Encryption_Key0  == (_y).Encryption_Key0)  && ((_x).Encryption_Key1  == (_y).Encryption_Key1)  && ((_x).Encryption_Key2  == (_y).Encryption_Key2)  && \
   ((_x).Encryption_Key3  == (_y).Encryption_Key3)  && ((_x).Encryption_Key4  == (_y).Encryption_Key4)  && ((_x).Encryption_Key5  == (_y).Encryption_Key5)  && \
   ((_x).Encryption_Key6  == (_y).Encryption_Key6)  && ((_x).Encryption_Key7  == (_y).Encryption_Key7)  && ((_x).Encryption_Key8  == (_y).Encryption_Key8)  && \
   ((_x).Encryption_Key9  == (_y).Encryption_Key9)  && ((_x).Encryption_Key10 == (_y).Encryption_Key10) && ((_x).Encryption_Key11 == (_y).Encryption_Key11) && \
   ((_x).Encryption_Key12 == (_y).Encryption_Key12) && ((_x).Encryption_Key13 == (_y).Encryption_Key13) && ((_x).Encryption_Key14 == (_y).Encryption_Key14) && \
   ((_x).Encryption_Key15 == (_y).Encryption_Key15)                                                                                                            \
)

   /* The following define determines the maximum fixed size that is    */
   /* used for Plain-text data that used as input to the LE encryption  */
   /* schema.                                                           */
#define PLAIN_TEXT_DATA_MAXIMUM_SIZE                                 	16

   /* The following define determines the size that is    */
   /* used for AES-CMAC input message data for generating the confirmation value */
#define AES_CMAC_CONFIRMATION_MSG_MAXIMUM_SIZE							65

   /* The following define determines the size that is    */
   /* used for AES-CMAC input message data for generating the Mackey and LTK values */
#define AES_CMAC_KEY_GENERATION_MSG_MAXIMUM_SIZE						53
#define AES_CMAC_CHECK_VALUE_GENERATION_MAXIMUM_MSG_SIZE				65
#define AES_CMAC_NUMERIC_COMPARISON_VALUE_GENERATION_MAXIMUM_MSG_SIZE	80

#define KEY_GENERATION_BTLE_SIZE										4
#define KEY_GENERATION_SALT_SIZE										16
#define KEY_GENERATION_T_SIZE											16
#define KEY_GENERATION_LENGTH_SIZE										2

#define AES_CMAC_CHECK_VALUE_GENERATION_R_SIZE							16
#define AES_CMAC_CHECK_VALUE_GENERATION_NON_ZERO_R_PASSKEY_SIZE			3
#define AES_CMAC_CHECK_VALUE_GENERATION_IOCAP_SIZE						3

#define NUMERIC_COMPARISON_VALUE_128_TO_LEAST_SIGNIFICANT_32_SHIFT		12
#define NUMERIC_COMPARISON_VALUE_MODULUS_TO_6_LEASET_DIGITS				1000000


   /* The following type defines the buffer that is used to format      */
   /* Plain-text data that is to be used for input to the LE encryption */
   /* schema (Version 4.0 + LE).  The size of this buffer is always     */
   /* fixed.                                                            */
typedef __PACKED_STRUCT_BEGIN__ struct _tagPlain_Text_Data_t
{
   Byte_t Plain_Text_Data[PLAIN_TEXT_DATA_MAXIMUM_SIZE];
} __PACKED_STRUCT_END__ Plain_Text_Data_t;

#define PLAIN_TEXT_DATA_SIZE                    (sizeof(Plain_Text_Data_t))


   /* The following type defines the buffer that is used to */
   /* format AES-CMAC input message data for generating the confirmation value - used in f4 function */
typedef __PACKED_STRUCT_BEGIN__ struct _tag_AES_CMAC_Confiramation_msg_t
{
   Byte_t AES_CMAC_Confiramation_msg[AES_CMAC_CONFIRMATION_MSG_MAXIMUM_SIZE];
} __PACKED_STRUCT_END__ AES_CMAC_Confiramation_msg_t;

#define AES_CMAC_CONFIRMATION_MSG_SIZE                    (sizeof(AES_CMAC_Confiramation_msg_t))

   /* The following type defines the buffer that is used to */
   /* format AES-CMAC input message data for generating the MacKey and LTK value - used in f5 function */
typedef __PACKED_STRUCT_BEGIN__ struct _tag_AES_CMAC_Key_Generation_msg_t
{
   Byte_t AES_CMAC_Key_Generation_msg[AES_CMAC_KEY_GENERATION_MSG_MAXIMUM_SIZE];
} __PACKED_STRUCT_END__ AES_CMAC_Key_Generation_msg_t;

#define AES_CMAC_KEY_GENERATION_MSG_SIZE                    (sizeof(AES_CMAC_Key_Generation_msg_t))

   /* The following type defines the buffer that is used to */
   /* format AES-CMAC input message data for generating the check value - used in f6 function */
typedef __PACKED_STRUCT_BEGIN__ struct _tag_AES_CMAC_Check_Value_Generation_msg_t
{
   Byte_t AES_CMAC_Check_Value_Generation_msg_t[AES_CMAC_CHECK_VALUE_GENERATION_MAXIMUM_MSG_SIZE];
} __PACKED_STRUCT_END__ AES_CMAC_Check_Value_Generation_msg_t;

#define AES_CMAC_CHECK_VALUE_GENERATION_MSG_SIZE                    (sizeof(AES_CMAC_Check_Value_Generation_msg_t))

   /* The following type defines the buffer that is used to */
   /* format AES-CMAC input message data for generating the Numeric Comparison value - used in g2 function */
typedef __PACKED_STRUCT_BEGIN__ struct _tag_AES_CMAC_Numeric_Comparison_Value_Generation_msg_t
{
   Byte_t AES_CMAC_Numeric_Comparison_Value_Generation_msg_t[AES_CMAC_NUMERIC_COMPARISON_VALUE_GENERATION_MAXIMUM_MSG_SIZE];
} __PACKED_STRUCT_END__ AES_CMAC_Numeric_Comparison_Value_Generation_msg_t;

#define AES_CMAC_NUMERIC_COMPARISON_MSG_SIZE                    (sizeof(AES_CMAC_Numeric_Comparison_Value_Generation_msg_t))


   /* The following define determines the maximum fixed size that is    */
   /* output from the LE encryption schema.                             */
#define ENCRYPTED_DATA_MAXIMUM_SIZE                                  16

   /* The following type defines the buffer that is output from the LE  */
   /* encryption schema (Version 4.0 + LE).  The size of this buffer is */
   /* always fixed.                                                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagEncrypted_Data_t
{
   Byte_t Encrypted_Data[ENCRYPTED_DATA_MAXIMUM_SIZE];
} __PACKED_STRUCT_END__ Encrypted_Data_t;

#define ENCRYPTED_DATA_SIZE                     (sizeof(Encrypted_Data_t))

   /* The following represents the structure of a Random Number that is */
   /* used during Encryption (Version 4.0 + LE).                        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagRandom_Number_t
{
   NonAlignedByte_t Random_Number0;
   NonAlignedByte_t Random_Number1;
   NonAlignedByte_t Random_Number2;
   NonAlignedByte_t Random_Number3;
   NonAlignedByte_t Random_Number4;
   NonAlignedByte_t Random_Number5;
   NonAlignedByte_t Random_Number6;
   NonAlignedByte_t Random_Number7;
} __PACKED_STRUCT_END__ Random_Number_t;

#define RANDOM_NUMBER_DATA_SIZE                 (sizeof(Random_Number_t))

   /* The following MACRO is a utility MACRO that exists to assign the  */
   /* individual Byte values into the specified Random Number variable. */
   /* The Bytes are NOT in Little Endian Format, however, they are      */
   /* assigned to the Random Number variable in Little Endian Format.   */
   /* The first parameter is the Random Number variable (of type        */
   /* Random_Number_t) to assign, and the next eight parameters are the */
   /* Individual Random Number Byte values to assign to the variable.   */
#define ASSIGN_RANDOM_NUMBER(_dest, _a, _b, _c, _d, _e, _f, _g, _h) \
{                                                                   \
   (_dest).Random_Number0 = (_h);                                   \
   (_dest).Random_Number1 = (_g);                                   \
   (_dest).Random_Number2 = (_f);                                   \
   (_dest).Random_Number3 = (_e);                                   \
   (_dest).Random_Number4 = (_d);                                   \
   (_dest).Random_Number5 = (_c);                                   \
   (_dest).Random_Number6 = (_b);                                   \
   (_dest).Random_Number7 = (_a);                                   \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two Random_Number_t variables to each other.  This  */
   /* MACRO only returns whether the the Random_Number_t variables are  */
   /* equal to each other (MACRO returns boolean result) NOT less       */
   /* than/greater than.  The parameters to this MACRO is the           */
   /* Random_Number_t structures to compare.                            */
#define COMPARE_RANDOM_NUMBER(_x, _y)                                                                                                               \
(                                                                                                                                                   \
   ((_x).Random_Number0 == (_y).Random_Number0) && ((_x).Random_Number1 == (_y).Random_Number1) && ((_x).Random_Number2  == (_y).Random_Number2) && \
   ((_x).Random_Number3 == (_y).Random_Number3) && ((_x).Random_Number4 == (_y).Random_Number4) && ((_x).Random_Number5  == (_y).Random_Number5) && \
   ((_x).Random_Number6 == (_y).Random_Number6) && ((_x).Random_Number7 == (_y).Random_Number7)                                                     \
)

   /* The following type declaration represents the structure of a      */
   /* single Long Term Key (Version 4.0 + LE).                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagLong_Term_Key_t
{
   Byte_t Long_Term_Key0;
   Byte_t Long_Term_Key1;
   Byte_t Long_Term_Key2;
   Byte_t Long_Term_Key3;
   Byte_t Long_Term_Key4;
   Byte_t Long_Term_Key5;
   Byte_t Long_Term_Key6;
   Byte_t Long_Term_Key7;
   Byte_t Long_Term_Key8;
   Byte_t Long_Term_Key9;
   Byte_t Long_Term_Key10;
   Byte_t Long_Term_Key11;
   Byte_t Long_Term_Key12;
   Byte_t Long_Term_Key13;
   Byte_t Long_Term_Key14;
   Byte_t Long_Term_Key15;
} __PACKED_STRUCT_END__ Long_Term_Key_t;

#define LONG_TERM_KEY_SIZE                      (sizeof(Long_Term_Key_t))

   /* The following MACRO is a utility MACRO that exists to assign the  */
   /* individual Byte values into the specified Long Term Key variable. */
   /* The Bytes are NOT in Little Endian Format, however, they are      */
   /* assigned to the Long Term Key variable in Little Endian Format.   */
   /* The first parameter is the Long Term Key variable (of type        */
   /* Long_Term_Key_t) to assign, and the next 16 parameters are the    */
   /* Individual Long Term Key Byte values to assign to the Long Term   */
   /* Key variable.                                                     */
#define ASSIGN_LONG_TERM_KEY(_dest, _a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n, _o, _p) \
{                                                                                                   \
   (_dest).Long_Term_Key0  = (_p); (_dest).Long_Term_Key1  = (_o); (_dest).Long_Term_Key2  = (_n);  \
   (_dest).Long_Term_Key3  = (_m); (_dest).Long_Term_Key4  = (_l); (_dest).Long_Term_Key5  = (_k);  \
   (_dest).Long_Term_Key6  = (_j); (_dest).Long_Term_Key7  = (_i); (_dest).Long_Term_Key8  = (_h);  \
   (_dest).Long_Term_Key9  = (_g); (_dest).Long_Term_Key10 = (_f); (_dest).Long_Term_Key11 = (_e);  \
   (_dest).Long_Term_Key12 = (_d); (_dest).Long_Term_Key13 = (_c); (_dest).Long_Term_Key14 = (_b);  \
   (_dest).Long_Term_Key15 = (_a);                                                                  \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two Long_Term_Key_t variables.  This MACRO only     */
   /* returns whether the two Long_Term_Key_t variables are equal (MACRO*/
   /* returns boolean result) NOT less than/greater than.  The two      */
   /* parameters to this MACRO are both of type Long_Term_Key_t and     */
   /* represent the Long_Term_Key_t variables to compare.               */
#define COMPARE_LONG_TERM_KEY(_x, _y)                                                                                                                    \
(                                                                                                                                                        \
   ((_x).Long_Term_Key0  == (_y).Long_Term_Key0)  && ((_x).Long_Term_Key1  == (_y).Long_Term_Key1)  && ((_x).Long_Term_Key2  == (_y).Long_Term_Key2)  && \
   ((_x).Long_Term_Key3  == (_y).Long_Term_Key3)  && ((_x).Long_Term_Key4  == (_y).Long_Term_Key4)  && ((_x).Long_Term_Key5  == (_y).Long_Term_Key5)  && \
   ((_x).Long_Term_Key6  == (_y).Long_Term_Key6)  && ((_x).Long_Term_Key7  == (_y).Long_Term_Key7)  && ((_x).Long_Term_Key8  == (_y).Long_Term_Key8)  && \
   ((_x).Long_Term_Key9  == (_y).Long_Term_Key9)  && ((_x).Long_Term_Key10 == (_y).Long_Term_Key10) && ((_x).Long_Term_Key11 == (_y).Long_Term_Key11) && \
   ((_x).Long_Term_Key12 == (_y).Long_Term_Key12) && ((_x).Long_Term_Key13 == (_y).Long_Term_Key13) && ((_x).Long_Term_Key14 == (_y).Long_Term_Key14) && \
   ((_x).Long_Term_Key15 == (_y).Long_Term_Key15)                                                                                                        \
)

   /* The following type declaration represents the structure of a      */
   /* LE States Type (Version 4.0 + LE).                                */
typedef __PACKED_STRUCT_BEGIN__ struct _tagLE_States_t
{
   Byte_t LE_States0;
   Byte_t LE_States1;
   Byte_t LE_States2;
   Byte_t LE_States3;
   Byte_t LE_States4;
   Byte_t LE_States5;
   Byte_t LE_States6;
   Byte_t LE_States7;
} __PACKED_STRUCT_END__ LE_States_t;

#define LE_STATES_SIZE                          (sizeof(LE_States_t))

   /* The following MACRO is a utility MACRO that exists to assign      */
   /* the individual Byte values into the specified LE States variable. */
   /* The Bytes are NOT in Little Endian Format, however, they are      */
   /* assigned to the LE States variable in Little Endian Format.  The  */
   /* first parameter is the LE States variable (of type LE_States_t)   */
   /* to assign, and the next eight parameters are the Individual LE    */
   /* States Byte values to assign to the variable.                     */
#define ASSIGN_LE_STATES(_dest, _a, _b, _c, _d, _e, _f, _g, _h) \
{                                                               \
   (_dest).LE_States0 = (_h);                                   \
   (_dest).LE_States1 = (_g);                                   \
   (_dest).LE_States2 = (_f);                                   \
   (_dest).LE_States3 = (_e);                                   \
   (_dest).LE_States4 = (_d);                                   \
   (_dest).LE_States5 = (_c);                                   \
   (_dest).LE_States6 = (_b);                                   \
   (_dest).LE_States7 = (_a);                                   \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two LE_States_t variables.  This MACRO only returns */
   /* whether the two LE_States_t variables are equal (MACRO returns    */
   /* boolean result) NOT less than/greater than.  The two parameters to*/
   /* this MACRO are both of type LE_States_t and represent the         */
   /* LE_States_t variables to compare.                                 */
#define COMPARE_LE_STATES(_x, _y)                                                                                           \
(                                                                                                                           \
   ((_x).LE_States0 == (_y).LE_States0) && ((_x).LE_States1 == (_y).LE_States1) && ((_x).LE_States2  == (_y).LE_States2) && \
   ((_x).LE_States3 == (_y).LE_States3) && ((_x).LE_States4 == (_y).LE_States4) && ((_x).LE_States5  == (_y).LE_States5) && \
   ((_x).LE_States6 == (_y).LE_States6) && ((_x).LE_States7 == (_y).LE_States7)                                             \
)

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* setting of LE States Bits in the LE States variable.  This MACRO  */
   /* accepts as input the LE States variable (of type LE_States_t) as  */
   /* the first parameter, and the bit number to set (LSB is bit number */
   /* 0).  This MACRO sets the bits in Little Endian Format (so bit 0 is*/
   /* physically bit 0 of the first byte of the LE States structure).   */
#define SET_LE_STATES_BIT(_x, _y)   \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] |= (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* clearing of LE States Bits in the LE States variable.  This MACRO */
   /* accepts as input the LE States variable (of type LE_States_t) as  */
   /* the first parameter, and the bit number to clear (LSB is bit      */
   /* number 0).  This MACRO clears the bits in Little Endian Format (so*/
   /* bit 0 is physically bit 0 of the first byte of the LE States      */
   /* structure).                                                       */
#define RESET_LE_STATES_BIT(_x, _y)   \
   ((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] &= (Byte_t)~(1 << ((_y)%(sizeof(Byte_t)*8)))

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* testing of LE States Bits in the LE States variable.  This MACRO  */
   /* accepts as input the LE States variable (of type LE_States_t) as  */
   /* the first parameter, and the bit number to test (LSB is bit number*/
   /* 0).  This MACRO tests the bits in Little Endian Format (so bit 0  */
   /* is physically bit 0 of the first byte of the LE States structure).*/
   /* This MACRO returns a boolean result as the test result.           */
#define TEST_LE_STATES_BIT(_x, _y)   \
   (((Byte_t *)&(_x))[(_y)/(sizeof(Byte_t)*8)] & (Byte_t)(1 << ((_y)%(sizeof(Byte_t)*8))))

   /* The following type declaration represents the structure of a      */
   /* Vendor Specific Codec field (Version 4.0 + CSA2).                 */
typedef __PACKED_STRUCT_BEGIN__ struct _tagVendor_Specific_Codec_t
{
   NonAlignedWord_t CompanyID;
   NonAlignedWord_t VendorCodecID;
} __PACKED_STRUCT_END__ Vendor_Specific_Codec_t;

#define VENDOR_SPECIFIC_CODEC_SIZE                             (sizeof(Vendor_Specific_Codec_t))

   /* The following MACRO is a utility MACRO that exists to assign the  */
   /* individual values into the specified Vendor Specific Codec        */
   /* variable.  The values are NOT in Little Endian Format, however,   */
   /* they are assigned to the Vendor Specific Codec variable in Little */
   /* Endian Format.  The first parameter is the Vendor Specific Codec  */
   /* variable (of type Vendor_Specific_Codec_t) to assign, the next    */
   /* parameter is the Company ID and the final parameter is the Vendor */
   /* Specific Codec ID.                                                */
#define ASSIGN_VENDOR_SPECIFIC_CODEC(_dest, _CompanyID, _VendorCodecID)                            \
{                                                                                                  \
   ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&((_dest).CompanyID), (_CompanyID));           \
   ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&((_dest).VendorCodecID), (_VendorCodecID));   \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two Vendor_Specific_Codec_t variables.  This MACRO  */
   /* only returns whether the two Vendor_Specific_Codec_t variables are*/
   /* equal (MACRO returns boolean result) NOT less than/greater than.  */
   /* The two parameters to this MACRO are both of type                 */
   /* Vendor_Specific_Codec_t and represent the Vendor_Specific_Codec_t */
   /* variables to compare.                                             */
#define COMPARE_VENDOR_SPECIFIC_CODEC(_x, _y)                                                                              \
(                                                                                                                          \
   (READ_UNALIGNED_WORD_LITTLE_ENDIAN(&((_x).CompanyID)) == READ_UNALIGNED_WORD_LITTLE_ENDIAN(&((_y).CompanyID))) &&       \
   (READ_UNALIGNED_WORD_LITTLE_ENDIAN(&((_x).VendorCodecID)) == READ_UNALIGNED_WORD_LITTLE_ENDIAN(&((_y).VendorCodecID)))  \
)

   /* The following type declaration represents the structure of a SCO  */
   /* Coding Format field (Version 4.0 + CSA2).                         */
   /* * NOTE * The VendorSpecificCodec field is ignored unless the      */
   /*          Coding_Format is set to                                  */
   /*          HCI_ENHANCED_SYNCHRONOUS_CODING_FORMAT_VENDOR_SPECIFIC.  */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSCO_Coding_Format_t
{
   NonAlignedByte_t        Coding_Format;
   Vendor_Specific_Codec_t VendorSpecificCodec;
} __PACKED_STRUCT_END__ SCO_Coding_Format_t;

#define SCO_CODING_FORMAT_SIZE                                 (sizeof(SCO_Coding_Format_t))

   /* The following MACRO is a utility MACRO that exists to assign the  */
   /* individual values into the specified SCO Coding Format variable.  */
   /* The first parameter is the Coding Format variable (of type        */
   /* Coding_Format_t) to assign, the next parameter is the Coding      */
   /* Format, followed by the Company ID and the final parameter is the */
   /* Vendor Specific Codec ID.                                         */
#define ASSIGN_SCO_CODING_FORMAT(_dest, _CodingFormat, _CompanyID, _VendorCodecID)                    \
{                                                                                                 \
   ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&((_dest).Coding_Format), (_CodingFormat));   \
   ASSIGN_VENDOR_SPECIFIC_CODEC(((_dest).VendorSpecificCodec), (_CompanyID), (_VendorCodecID));   \
}

#endif
