#include <bsp/bluetooth/Bluetooth.hpp>
#include "segger/log/log.hpp"

using namespace bsp;

extern "C"
{

#include "HCITRANS.h"
#define TRANSPORT_ID 1

    typedef void(BTPSAPI *HCITR_COMDataCallback_t)(unsigned int HCITransportID,
                                                   unsigned int DataLength,
                                                   unsigned char *DataBuffer,
                                                   unsigned long CallbackParameter);
    void RxThread(void *Param);

    int BTPSAPI HCITR_COMOpen(HCI_COMMDriverInformation_t *COMMDriverInformation,
                              HCITR_COMDataCallback_t COMDataCallback,
                              unsigned long CallbackParameter)
    {

        int ret = TRANSPORT_ID;
        LOG_INFO("COM OPEN!");
        Bluetopia *bt = Bluetopia::getInstance();
        bt->set_reset(true);
        bt->open();
        bt->com_cb       = COMDataCallback;
        bt->com_cb_param = CallbackParameter;
        bt->rx_thread    = xTaskCreate(RxThread, "RxThread", 4096, NULL, 3, &bt->thandle);
        if (bt->rx_thread != pdPASS) {
            ret = ErrorBtGeneric;
            LOG_ERROR("COM OPEN failure: %d", bt->rx_thread);
        }
        LOG_INFO("COM OPEN! done");
        return ret;
    }

    void BTPSAPI HCITR_COMClose(unsigned int HCITransportID)
    {
        LOG_ERROR("COM CLOSE");
        Bluetopia *dev = Bluetopia::getInstance();
        if (dev->thandle != NULL) {
            // TODO check me
            // vTaskDelete(bt->thandle);
        }
        dev->close();
        dev->set_reset(false);
        if (dev->com_cb) {
            dev->com_cb(TRANSPORT_ID, 0, NULL, dev->com_cb_param);
        }
        LOG_ERROR("COM CLOSED");
    }

    void BTPSAPI HCITR_COMReconfigure(unsigned int HCITransportID, HCI_Driver_Reconfigure_Data_t *DriverReconfigureData)
    {
        HCI_COMMReconfigureInformation_t *ReconfigureInformation;
        Bluetopia *dev = Bluetopia::getInstance();
        if ((HCITransportID == TRANSPORT_ID) && dev->is_open && (DriverReconfigureData)) {
            if ((DriverReconfigureData->ReconfigureCommand ==
                 HCI_COMM_DRIVER_RECONFIGURE_DATA_COMMAND_CHANGE_COMM_PARAMETERS) &&
                (DriverReconfigureData->ReconfigureData)) {
                ReconfigureInformation = (HCI_COMMReconfigureInformation_t *)(DriverReconfigureData->ReconfigureData);
                if (ReconfigureInformation->ReconfigureFlags &
                    HCI_COMM_RECONFIGURE_INFORMATION_RECONFIGURE_FLAGS_CHANGE_BAUDRATE) {
                    LOG_INFO("Set baudrate to: %d", ReconfigureInformation->BaudRate);
                    dev->set_irq(false);
                    dev->set_baudrate(ReconfigureInformation->BaudRate);
                    dev->set_irq(true);
                }
            }
        }
    }

    int BTPSAPI HCITR_COMWrite(unsigned int HCITransportID, unsigned int Length, unsigned char *Buffer)
    {
        int ret = 0;
        // LOG_INFO("DATA -> [%d]", Length);
        if (HCITransportID) {
            ret = Bluetopia::getInstance()->write_blocking(reinterpret_cast<char *>(Buffer), Length);
        }
        else {
            ret = -1;
        }
        return ret;
    }

    int BTPSAPI HCITR_COMSuspend(unsigned int HCITransportID)
    {
        LOG_ERROR("Not implemented!");
        int ret = -1;
        if (HCITransportID) {}
        else {
            ret = -1;
        }
        return ret;
    }

    int BTPSAPI HCITR_EnableDebugLogging(Boolean_t Enable)
    {
        return 0;
    }

    void RxThread(void *Param)
    {
        LOG_INFO("BT RxThread created");
        Bluetopia *dev = Bluetopia::getInstance();
        while (dev->is_open) {
            dev->wait_data();
            dev->set_irq(false);
            // LOG_INFO("DATA [%d]<--", dev->in.len);
            // for(int i=0; i<dev->in.len; ++i) {
            //     LOG_PRINTF("0x%X ", dev->in.buff[i]);
            // }
            // LOG_PRINTF("\n");
            if (dev->com_cb) {
                dev->com_cb(TRANSPORT_ID, dev->in.len, (unsigned char *)dev->in.buff, dev->com_cb_param);
            }
            dev->in.flush();
            dev->set_irq(true);
            dev->set_rts(true);
        }
    }
};
