/*****< btpsvend.c >***********************************************************/
/*      Copyright 2012 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*      Copyright 2015 Texas Instruments Incorporated.                        */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  BTPSVEND - Vendor specific functions/definitions used to define a set of  */
/*             vendor specific functions supported by the Bluetopia Protocol  */
/*             Stack.  These functions may be unique to a given hardware      */
/*             platform.                                                      */
/*                                                                            */
/*  Author:  Marcus Funk                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   11/08/12  M. Funk        Initial creation.                               */
/*   02/02/15  D. Horowitz    Adding ifdef for CC256X/CC256XB                 */
/*   05/03/12  D. Horowitz    Adding ifdef for CC256X/CC256XB/CC256XC         */
/******************************************************************************/
#include "SS1BTPS.h"          /* Bluetopia API Prototypes/Constants.          */
#include "SS1BTVS.h"          /* TI Vendor Specific Commands.                 */
#include "BTPSKRNL.h"         /* Bluetooth Kernel Prototypes/Constants.       */
#include "BTPSVEND.h"


    /* Add this define to use CC256X									*/
#ifdef __SUPPORT_CC256X_PATCH__
    #include "CC256X.h"
#else
#ifdef __SUPPORT_CC256XC_PATCH__
    #include "CC256XC.h"
#else    
    #include "CC256XB.h"
#endif    
#endif

   /* Miscellaneous Type Declarations.                                  */
#define VENDOR_DEFAULT_BAUDRATE  115200

   /* The following stores the correct driver information for use by    */
   /* functions that need to reconfigure the driver.                    */
static unsigned long       SpecifiedBaudRate;
static HCI_COMM_Protocol_t SpecifiedProtocol;

   /* The following variable is used to track whether or not the Vendor */
   /* Specific Commands (and Patch RAM commands) have already been      */
   /* issued to the device.  This is done so that we do not issue them  */
   /* more than once for a single device (there is no need).  They could*/
   /* be potentially issued because the HCI Reset hooks (defined below) */
   /* are called for every HCI_Reset() that is issued.                  */
static Boolean_t VendorCommandsIssued;

   /* Internal Function Prototypes.                                     */
static Boolean_t DownloadPatch(unsigned int BluetoothStackID, unsigned int PatchLength, BTPSCONST unsigned char *PatchPointer);

   /* The following function is provided to allow a mechanism to        */
   /* download the specified Patch Data to the CC25xx device.  This     */
   /* function does not disable the co-processor.  This function returns*/
   /* TRUE if successful or FALSE if there was an error.                */
static Boolean_t DownloadPatch(unsigned int BluetoothStackID, unsigned int PatchLength, BTPSCONST unsigned char *PatchPointer)
{
   Boolean_t      ret_val;
   int            Result;
   unsigned char  Length;
   unsigned char  Buffer[128];
   unsigned char  OGF;
   unsigned short OCF;
   unsigned char  Status;

   /* First, make sure the input parameters appear to be semi-valid.    */
   if((BluetoothStackID) && (PatchPointer))
   {
      ret_val = TRUE;

      while((PatchLength) && (ret_val))
      {
         /* Confirm that the first parameter is what we expect.         */
         if(PatchPointer[0] == 0x01)
         {
            /* Get the OGF and OCF values an make a call to perform the */
            /* vendor specific function.                                */
            OGF    = PatchPointer[2] >> 2;
            OCF    = (READ_UNALIGNED_WORD_LITTLE_ENDIAN(&PatchPointer[1]) & 0x3FF);
            Length = sizeof(Buffer);
            Result = HCI_Send_Raw_Command(BluetoothStackID, OGF, OCF, PatchPointer[3], (unsigned char*)&PatchPointer[4], &Status, &Length, Buffer, TRUE);

            /* If the function was successful, update the count and     */
            /* pointer for the next command.                            */
            if((!Result) || (!Status))
            {
               /* Advance to the next Patch Entry.                      */
               PatchLength  -= (PatchPointer[3] + 4);
               PatchPointer += (PatchPointer[3] + 4);
            }
            else
               ret_val = FALSE;
         }
         else
            ret_val = FALSE;
      }
   }
   else
      ret_val = FALSE;

   return(ret_val);
}

   /* The following function prototype represents the vendor specific   */
   /* function which is used to implement any needed Bluetooth device   */
   /* vendor specific functionality that needs to be performed before   */
   /* the HCI Communications layer is opened.  This function is called  */
   /* immediately prior to calling the initialization of the HCI        */
   /* Communications layer.  This function should return a BOOLEAN TRUE */
   /* indicating successful completion or should return FALSE to        */
   /* indicate unsuccessful completion.  If an error is returned the    */
   /* stack will fail the initialization process.                       */
   /* * NOTE * The parameter passed to this function is the exact       */
   /*          same parameter that was passed to BSC_Initialize() for   */
   /*          stack initialization.  If this function changes any      */
   /*          members that this pointer points to, it will change the  */
   /*          structure that was originally passed.                    */
   /* * NOTE * No HCI communication calls are possible to be used in    */
   /*          this function because the driver has not been initialized*/
   /*          at the time this function is called.                     */
Boolean_t BTPSAPI HCI_VS_InitializeBeforeHCIOpen(HCI_DriverInformation_t *HCI_DriverInformation)
{
   /* Flag that we have not issued the first Vendor Specific Commands   */
   /* before the first reset.                                           */
   VendorCommandsIssued = FALSE;

   /* Store the driver information for later use.                       */
   SpecifiedBaudRate = HCI_DriverInformation->DriverInformation.COMMDriverInformation.BaudRate;
   SpecifiedProtocol = HCI_DriverInformation->DriverInformation.COMMDriverInformation.Protocol;

   /* Make sure that the driver is initially configured to the default  */
   /* baud rate of the controller.                                      */
   HCI_DriverInformation->DriverInformation.COMMDriverInformation.BaudRate = VENDOR_DEFAULT_BAUDRATE;

   return(TRUE);
}

   /* The following function prototype represents the vendor specific   */
   /* function which is used to implement any needed Bluetooth device   */
   /* vendor specific functionality after the HCI Communications layer  */
   /* is initialized (the driver only).  This function is called        */
   /* immediately after returning from the initialization of the HCI    */
   /* Communications layer (HCI Driver).  This function should return a */
   /* BOOLEAN TRUE indicating successful completion or should return    */
   /* FALSE to indicate unsuccessful completion.  If an error is        */
   /* returned the stack will fail the initialization process.          */
   /* * NOTE * No HCI layer function calls are possible to be used in   */
   /*          this function because the actual stack has not been      */
   /*          initialized at this point.  The only initialization that */
   /*          has occurred is with the HCI Driver (hence the HCI       */
   /*          Driver ID that is passed to this function).              */
Boolean_t BTPSAPI HCI_VS_InitializeAfterHCIOpen(unsigned int HCIDriverID)
{
   return(TRUE);
}

   /* The following function prototype represents the vendor specific   */
   /* function which is used to implement any needed Bluetooth device   */
   /* vendor specific functions after the HCI Communications layer AND  */
   /* the HCI Stack layer has been initialized.  This function is called*/
   /* after all HCI functionality is established, but before the initial*/
   /* HCI Reset is sent to the stack.  The function should return a     */
   /* BOOLEAN TRUE to indicate successful completion or should return   */
   /* FALSE to indicate unsuccessful completion.  If an error is        */
   /* returned the stack will fail the initialization process.          */
   /* * NOTE * At the time this function is called HCI Driver and HCI   */
   /*          layer functions can be called, however no other stack    */
   /*          layer functions are able to be called at this time       */
   /*          (hence the HCI Driver ID and the Bluetooth Stack ID      */
   /*          passed to this function).                                */
Boolean_t BTPSAPI HCI_VS_InitializeBeforeHCIReset(unsigned int HCIDriverID, unsigned int BluetoothStackID)
{
   return(TRUE);
}

   /* The following function prototype represents the vendor specific   */
   /* function which is used to implement any needed Bluetooth device   */
   /* vendor specific functionality after the HCI layer has issued any  */
   /* HCI Reset as part of the initialization.  This function is called */
   /* after all HCI functionality is established, just after the initial*/
   /* HCI Reset is sent to the stack.  The function should return a     */
   /* BOOLEAN TRUE to indicate successful completion or should return   */
   /* FALSE to indicate unsuccessful completion.  If an error is        */
   /* returned the stack will fail the initialization process.          */
   /* * NOTE * At the time this function is called HCI Driver and HCI   */
   /*          layer functions can be called, however no other stack    */
   /*          layer functions are able to be called at this time (hence*/
   /*          the HCI Driver ID and the Bluetooth Stack ID passed to   */
   /*          this function).                                          */
Boolean_t BTPSAPI HCI_VS_InitializeAfterHCIReset(unsigned int HCIDriverID, unsigned int BluetoothStackID)
{
   Boolean_t ret_val;

   /* Verify that the parameters that were passed in appear valid.      */
   if((!VendorCommandsIssued) && (HCIDriverID) && (BluetoothStackID))
   {
      DBG_MSG(DBG_ZONE_VENDOR, ("HCI_VS_InitializeAfterHCIReset\r\n"));

      if(SpecifiedBaudRate != VENDOR_DEFAULT_BAUDRATE)
      {
         /* First, change the baseband's baudrate to what was specified */
         /* in the initialization structure.                            */
         if(!VS_Update_UART_Baud_Rate(BluetoothStackID, SpecifiedBaudRate))
            ret_val = TRUE;
         else
            ret_val = FALSE;

         BTPS_Delay(100);
      }
      else
         ret_val = TRUE;

     if(ret_val)
      {
         /* Next download the patch.                                    */
         ret_val = DownloadPatch(BluetoothStackID, sizeof(BasePatch), BasePatch);

         if((ret_val) && ((SpecifiedProtocol == cpHCILL) || (SpecifiedProtocol == cpHCILL_RTS_CTS)))
         {
            /* Now configure and enable HCILL                           */
            /*    Inactivity Timeout                       = 100ms      */
            /*    WakeUp indication retransmission timeout = 500ms      */
            /*    RTS pulse width                          = 150us      */
            if(!VS_Update_HCILL_Parameters(BluetoothStackID, 80, 400, 150))
            {
               if(VS_Enable_Sleep_Mode(BluetoothStackID, TRUE))
                  ret_val = FALSE;
            }
            else
               ret_val = FALSE;
         }
      }

      VendorCommandsIssued = ret_val;
   }
   else
      ret_val = FALSE;

   /* Print Success/Failure status.                                     */
   DBG_MSG(DBG_ZONE_VENDOR, ("HCI_VS_InitializeAfterHCIReset %s\r\n", (ret_val) ? "Success" : "Failure"));

   return(ret_val);
}

   /* The following function prototype represents the vendor specific   */
   /* function which would is used to implement any needed Bluetooth    */
   /* device vendor specific functionality before the HCI layer is      */
   /* closed.  This function is called at the start of the HCI_Cleanup()*/
   /* function (before the HCI layer is closed), at which time all HCI  */
   /* functions are still operational.  The caller is NOT able to call  */
   /* any other stack functions other than the HCI layer and HCI Driver */
   /* layer functions because the stack is being shutdown (i.e.         */
   /* something has called BSC_Shutdown()).  The caller is free to      */
   /* return either success (TRUE) or failure (FALSE), however, it will */
   /* not circumvent the closing down of the stack or of the HCI layer  */
   /* or HCI Driver (i.e. the stack ignores the return value from this  */
   /* function).                                                        */
   /* * NOTE * At the time this function is called HCI Driver and HCI   */
   /*          layer functions can be called, however no other stack    */
   /*          layer functions are able to be called at this time (hence*/
   /*          the HCI Driver ID and the Bluetooth Stack ID passed to   */
   /*          this function).                                          */
Boolean_t BTPSAPI HCI_VS_InitializeBeforeHCIClose(unsigned int HCIDriverID, unsigned int BluetoothStackID)
{
   Boolean_t ret_val;

   if((SpecifiedProtocol == cpHCILL) || (SpecifiedProtocol == cpHCILL_RTS_CTS))
   {
      /* Disable HCILL.                                                 */
      if(!VS_Enable_Sleep_Mode(BluetoothStackID, FALSE))
         ret_val = TRUE;
      else
         ret_val = FALSE;
   }
   else
      ret_val = TRUE;

   if(ret_val)
   {
      if(SpecifiedBaudRate != VENDOR_DEFAULT_BAUDRATE)
      {
         /* Set the Baudrate back to the baseband's default.            */
         ret_val = VS_Update_UART_Baud_Rate(BluetoothStackID, VENDOR_DEFAULT_BAUDRATE);
      }
   }

   /* Flag that we need to re-download the Patch.                       */
   VendorCommandsIssued = FALSE;

   return(ret_val);
}

   /* The following function prototype represents the vendor specific   */
   /* function which is used to implement any needed Bluetooth device   */
   /* vendor specific functionality after the entire Bluetooth Stack is */
   /* closed.  This function is called during the HCI_Cleanup()         */
   /* function, after the HCI Driver has been closed.  The caller is    */
   /* free return either success (TRUE) or failure (FALSE), however, it */
   /* will not circumvent the closing down of the stack as all layers   */
   /* have already been closed.                                         */
   /* * NOTE * No Stack calls are possible in this function because the */
   /*          entire stack has been closed down at the time this       */
   /*          function is called.                                      */
Boolean_t BTPSAPI HCI_VS_InitializeAfterHCIClose(void)
{
   return(TRUE);
}


 /* Miscellaneous Type Declarations.                                  */

   /* Represents a size large enough to hold the return result from any */
   /* of the patch ram setup commands.                                  */
#define RETURN_BUFFER_SIZE                                        (32)


   /* The following function prototype represents the vendor specific   */
   /* function which is used to enable a specific vendor specific       */
   /* feature.  This can be used to reconfigure the chip for a specific */
   /* feature (i.e. if a special configuration/patch needs to be        */
   /* dynamically loaded it can be done in this function).  This        */
   /* function returns TRUE if the feature was able to be enabled       */
   /* successfully, or FALSE if the feature was unable to be enabled.   */
   /* * NOTE * This functionality is not normally supported by default  */
   /*          (i.e. a custom stack build is required to enable this    */
   /*          functionality).                                          */
#ifndef __SUPPORT_CC256X_PATCH__
Boolean_t BTPSAPI HCI_VS_EnableFeature(unsigned int BluetoothStackID, unsigned long Feature)
{
   int            Result;
   Boolean_t      ret_val = FALSE;
   unsigned long  FeatureMask;

   /* First, make sure that we have a semi-valid Bluetooth Stack ID.    */
   if(BluetoothStackID)
   {
      /* Next, make sure there is a feature to enable.                  */
      if(Feature)
      {
         /* Next, let's see what the currently enabled features   */
         /* are.                                                  */
         if(!BSC_QueryActiveFeatures(BluetoothStackID, &FeatureMask))
         {
            /* Next, let's see if we can enable the feature or if */
            /* another feature is already enabled.                */
            if((FeatureMask) && (FeatureMask != Feature))
               ret_val = FALSE;
            else
            {
               if(FeatureMask & Feature) {
						/* If the feature is already enabled return TRUE */
                  ret_val = TRUE;
               }
               else
               {
                  /* Finally determine the feature that we are    */
                  /* Enabling.                                    */
                  switch(Feature)
                  {
#ifdef __SUPPORT_LOW_ENERGY__
                     case BSC_FEATURE_BLUETOOTH_LOW_ENERGY:
                        ret_val = DownloadPatch(BluetoothStackID, LowEnergyPatchLength, LowEnergyPatch);
                        break; 
#endif
                     case BSC_FEATURE_A3DP_SOURCE:
                     case BSC_FEATURE_WIDE_BAND_SPEECH:
                        ret_val = DownloadPatch(BluetoothStackID, AvprPatchLength, AvprPatch);
						if(FALSE == ret_val) break;
                        Result = VS_AVPR_Enable(BluetoothStackID, TRUE, FALSE, AVRP_A3DP_SOURCE_ROLE);
                        ret_val = Result ? FALSE : TRUE;
                        break;
                     case BSC_FEATURE_A3DP_SINK:
					 	ret_val = DownloadPatch(BluetoothStackID, AvprPatchLength, AvprPatch);
                        if(FALSE == ret_val) break;
                        Result = VS_AVPR_Enable(BluetoothStackID, TRUE, FALSE, AVRP_A3DP_SINK_ROLE);
                        ret_val = Result ? FALSE : TRUE;
                        break;
                     default:
                        /* Unsupported Feature. */
                        ret_val = FALSE;
                        break;
                  }
               }
            }
         }
         else
            ret_val = FALSE;
      }
      else
         ret_val = FALSE;
   }
   else
      ret_val = FALSE;

   return(ret_val);
}

   /* The following function prototype represents the vendor specific   */
   /* function which is used to enable a specific vendor specific       */
   /* feature.  This can be used to reconfigure the chip for a specific */
   /* feature (i.e. if a special configuration/patch needs to be        */
   /* dynamically loaded it can be done in this function).  This        */
   /* function returns TRUE if the feature was able to be disabled      */
   /* successfully, or FALSE if the feature was unable to be disabled.  */
   /* * NOTE * This functionality is not normally supported by default  */
   /*          (i.e. a custom stack build is required to enable this    */
   /*          functionality).                                          */
Boolean_t BTPSAPI HCI_VS_DisableFeature(unsigned int BluetoothStackID, unsigned long Feature)
{
   int            Result;
   Boolean_t      ret_val;
   unsigned long  FeatureMask;  

   /* First, make sure that we have a semi-valid Bluetooth Stack ID.    */
   if(BluetoothStackID)
   {
      /* Next, make sure there is a feature to disable.                 */
      if(Feature)
      {
         /* Next, let's see what the currently enabled features are.    */
         if(!BSC_QueryActiveFeatures(BluetoothStackID, &FeatureMask))
         {
            /* Next, let's see if we can disable the feature or it is   */
            /* already disabled.                                        */
            if((FeatureMask) && (FeatureMask != Feature))
               ret_val = FALSE;
            else
            {
               if(FeatureMask & Feature)
               {
                  /* Initialize success.                                */
                  ret_val = TRUE;

                  /* Finally determine the feature that we are          */
                  /* Disabling.                                         */
                  switch(Feature)
                  {
                     case BSC_FEATURE_BLUETOOTH_LOW_ENERGY:
						Result = VS_DisableBLE(BluetoothStackID);
						ret_val = Result ? FALSE : TRUE;
                     case BSC_FEATURE_WIDE_BAND_SPEECH:
                     case BSC_FEATURE_A3DP_SOURCE:
                     case BSC_FEATURE_A3DP_SINK:
                        /* Simply disable the AVPR Functionality.       */
                        Result = VS_AVPR_Enable(BluetoothStackID, FALSE, FALSE, AVRP_A3DP_SOURCE_ROLE);
                        ret_val = Result ? FALSE : TRUE;
                        break;
                     default:
                        ret_val = FALSE;
                        break;
                  }
               }
               else
                  ret_val = FALSE;
            }
         }
         else
            ret_val = FALSE;
      }
      else
         ret_val = FALSE;
   }
   else
      ret_val = FALSE;

   return(ret_val);
}
#endif

