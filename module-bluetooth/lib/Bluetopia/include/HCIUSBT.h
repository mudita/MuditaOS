/*****< hciusbt.h >************************************************************/
/*      Copyright 2000 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  HCIUSBT - USB HCI Driver Layer Types.                                     */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   08/17/00  D. Lange       Initial creation.                               */
/******************************************************************************/
#ifndef __HCIUSBTH__
#define __HCIUSBTH__

   /* The following constants represent the Minimum, Maximum, and       */
   /* Values that are used with the Initialization Delay member of the  */
   /* HCI_USBDriverInformation_t structure.  These Delays are specified */
   /* in Milliseconds and represent the delay that is to be added       */
   /* between Port Initialization (Open) and the writing of any data to */
   /* the Port.  This functionality was added because it was found that */
   /* some USB Drivers required a delay between the time the Device was */
   /* opened and the time when the Driver was ready to accept data.     */
   /* The default is NO Delay (0 Milliseconds).                         */
#define HCI_USB_INFORMATION_INITIALIZATION_DELAY_MINIMUM       0
#define HCI_USB_INFORMATION_INITIALIZATION_DELAY_MAXIMUM    5000
#define HCI_USB_INFORMATION_INITIALIZATION_DELAY_DEFAULT       0

   /* The following type declaration defines the HCI USB Driver that    */
   /* that will be used as the physical HCI Transport Driver for the USB*/
   /* Device that is to be opened.  This type declaration is used in the*/
   /* HCI_USBDriverInformation_t structure that is required when an HCI */
   /* USB Device is opened.                                             */
typedef enum
{
   dtStonestreetOne,
   dtGarmin
} HCI_USB_Driver_t;

   /* The following type declaration represents the structure of all    */
   /* Data that is needed to open an HCI USB Port.                      */
typedef struct _tagHCI_USBDriverInformation_t
{
   unsigned int     DriverInformationSize;      /* Physical Size of this      */
                                                /* structure.                 */
   HCI_USB_Driver_t DriverType;                 /* HCI USB Driver type that   */
                                                /* will be used for           */
                                                /* communication to the       */
                                                /* Bluetooth Device.          */
   unsigned int     InitializationDelay;        /* Time (In Milliseconds) to  */
                                                /* Delay after the Port is    */
                                                /* opened before any data is  */
                                                /* sent over the Port.  This  */
                                                /* member is present because  */
                                                /* some Drivers may require   */
                                                /* a delay because the device */
                                                /* does not function for some */
                                                /* specified period of time.  */
} HCI_USBDriverInformation_t;

   /* The following constant is used with the                           */
   /* HCI_USB_Driver_Reconfigure_Data_t structure (ReconfigureCommand   */
   /* member) to specify that the Communication parameters are required */
   /* to change.  When specified, the ReconfigureData member will point */
   /* to a valid HCI_USBDriverInformation_t structure which holds the   */
   /* new parameters.                                                   */
   /* * NOTE * The underlying driver may not support changing all of    */
   /*          specified parameters.                                    */
#define HCI_USB_DRIVER_RECONFIGURE_DATA_COMMAND_CHANGE_PARAMETERS  (HCI_DRIVER_RECONFIGURE_DATA_RECONFIGURE_COMMAND_TRANSPORT_START)

#endif
