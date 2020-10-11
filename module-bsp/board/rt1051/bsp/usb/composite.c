#include "board.h"
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"
#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"
#include "composite.h"
#include "usb_phy.h"

#include "virtual_com_demo.h"

extern usb_device_class_struct_t g_UsbDeviceCdcVcomConfig;
extern usb_device_class_struct_t g_MtpClass;

/* Composite device structure. */
static usb_device_composite_struct_t composite;
static usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param);

/* USB device class information */
static usb_device_class_config_struct_t g_CompositeClassConfig[2] = {
    {
        VirtualComUSBCallback, &composite.cdcVcom, (class_handle_t)NULL, &g_UsbDeviceCdcVcomConfig,
    },
    {
        MtpUSBCallback, &composite.mtpApp, (class_handle_t)NULL, &g_MtpClass,
    }
};

/* USB device class configuration information */
static usb_device_class_config_list_struct_t g_UsbDeviceCompositeConfigList = {
    g_CompositeClassConfig, USB_DeviceCallback, sizeof(g_CompositeClassConfig)/sizeof(usb_device_class_config_struct_t),
};

void USB_OTG1_IRQHandler(void)
{
    USB_DeviceEhciIsrFunction(composite.deviceHandle);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
    exception return operation might vector to incorrect interrupt */
    __DSB();
}

void USB_OTG2_IRQHandler(void)
{
    USB_DeviceEhciIsrFunction(composite.deviceHandle);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
    exception return operation might vector to incorrect interrupt */
    __DSB();
}

static uint32_t USB_DeviceClockInit(void)
{
    usb_phy_config_struct_t phyConfig = {
        BOARD_USB_PHY_D_CAL,
        BOARD_USB_PHY_TXCAL45DP,
        BOARD_USB_PHY_TXCAL45DM,
    };

    if (CONTROLLER_ID == kUSB_ControllerEhci0)
    {
        CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
        CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M, 480000000U);
    }
    else
    {
        CLOCK_EnableUsbhs1PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
        CLOCK_EnableUsbhs1Clock(kCLOCK_Usb480M, 480000000U);
    }
    return USB_EhciPhyInit(CONTROLLER_ID, BOARD_XTAL0_CLK_HZ, &phyConfig);
}

void USB_DeviceIsrEnable(void)
{
    uint8_t irqNumber;

    uint8_t usbDeviceEhciIrq[] = USBHS_IRQS;
    irqNumber                  = usbDeviceEhciIrq[CONTROLLER_ID - kUSB_ControllerEhci0];

    /* Install isr, set priority, and enable IRQ. */
    NVIC_SetPriority((IRQn_Type)irqNumber, USB_DEVICE_INTERRUPT_PRIORITY);
    EnableIRQ((IRQn_Type)irqNumber);
}

static usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;
    uint16_t *temp16 = (uint16_t *)param;
    uint8_t *temp8 = (uint8_t *)param;

    switch (event)
    {
        case kUSB_DeviceEventBusReset:
        {
            composite.attach = 0;
            composite.currentConfiguration = 0U;
            error = kStatus_USB_Success;
            VirtualComReset(&composite.cdcVcom);
#if (defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U))
            /* Get USB speed to configure the device, including max packet size and interval of the endpoints. */
            if (kStatus_USB_Success == USB_DeviceClassGetSpeed(CONTROLLER_ID, &composite.speed))
            {
                USB_DeviceSetSpeed(handle, composite.speed);
            }
#endif
        }
        break;
        case kUSB_DeviceEventSetConfiguration:
            if (0U ==(*temp8))
            {
                composite.attach = 0;
                composite.currentConfiguration = 0U;
            }
            else if (USB_COMPOSITE_CONFIGURE_INDEX == (*temp8))
            {
                composite.attach = 1;
                composite.currentConfiguration = *temp8;
                VirtualComUSBSetConfiguration(&composite.cdcVcom, *temp8);
                error = kStatus_USB_Success;
            }
            else
            {
                error = kStatus_USB_InvalidRequest;
            }
            break;
        case kUSB_DeviceEventSetInterface:
            if (composite.attach)
            {
                uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
                uint8_t alternateSetting = (uint8_t)(*temp16 & 0x00FFU);
                if (interface < USB_INTERFACE_COUNT)
                {
                    composite.currentInterfaceAlternateSetting[interface] = alternateSetting;
                    error = kStatus_USB_Success;
                }
            }
            break;
        case kUSB_DeviceEventGetConfiguration:
            if (param)
            {
                *temp8 = composite.currentConfiguration;
                error = kStatus_USB_Success;
            }
            break;
        case kUSB_DeviceEventGetInterface:
            if (param)
            {
                uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
                if (interface < USB_INTERFACE_COUNT)
                {
                    *temp16 = (*temp16 & 0xFF00U) | composite.currentInterfaceAlternateSetting[interface];
                    error = kStatus_USB_Success;
                }
                else
                {
                    error = kStatus_USB_InvalidRequest;
                }
            }
            break;
        case kUSB_DeviceEventGetDeviceDescriptor:
            if (param)
            {
                error = USB_DeviceGetDeviceDescriptor(handle, (usb_device_get_device_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetConfigurationDescriptor:
            if (param)
            {
                error = USB_DeviceGetConfigurationDescriptor(handle,
                                                             (usb_device_get_configuration_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetStringDescriptor:
            if (param)
            {
                error = USB_DeviceGetStringDescriptor(handle, (usb_device_get_string_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventDetach:
            VirtualComDetached(&composite.cdcVcom);
            MtpDetached(&composite.mtpApp);
            break;
        default:
            break;
    }

    return error;
}


int composite_init(void)
{
    if (USB_DeviceClockInit() != kStatus_USB_Success) {
        LOG_ERROR("[Composite] USB Device Clock init failed");
    }

    composite.speed = USB_SPEED_FULL;
    composite.attach = 0;
    composite.cdcVcom.cdcAcmHandle = (class_handle_t)NULL;
    composite.deviceHandle = NULL;

    if (kStatus_USB_Success !=
        USB_DeviceClassInit(CONTROLLER_ID, &g_UsbDeviceCompositeConfigList, &composite.deviceHandle))
    {
        LOG_ERROR("[Composite] USB Device init failed");
        return -1;
    }
    else
    {
        /* TODO: pass event handling function here */
        if (VirtualComInit(&composite.cdcVcom, g_CompositeClassConfig[0].classHandle, NULL, NULL) != kStatus_USB_Success)
            LOG_ERROR("[Composite] VirtualCom initialization failed");

        if (MtpInit(&composite.mtpApp, g_CompositeClassConfig[1].classHandle) != kStatus_USB_Success)
            LOG_ERROR("[Composite] MTP initialization failed");
    }

    VirtualComDemoInit(&composite.cdcVcom);

    USB_DeviceIsrEnable();

    if (USB_DeviceRun(composite.deviceHandle) != kStatus_USB_Success) {
        LOG_ERROR("[Composite] USB Device run failed");
    }

    LOG_DEBUG("[Composite] USB initialized");
    return 0;

}
