#include <bsp/bluetooth/Bluetooth.hpp>

using namespace bsp;

extern "C" {

#include "HCITRANS.h"

typedef void (BTPSAPI *HCITR_COMDataCallback_t)(unsigned int HCITransportID, unsigned int DataLength, unsigned char *DataBuffer, unsigned long CallbackParameter);

int BTPSAPI HCITR_COMOpen(  HCI_COMMDriverInformation_t *COMMDriverInformation,
                            HCITR_COMDataCallback_t COMDataCallback,
                            unsigned long CallbackParameter)
{
    int ret=0;
    Bluetopia::getInstance()->open();
    return -1;
}

void BTPSAPI HCITR_COMClose(unsigned int HCITransportID)
{
}

void BTPSAPI HCITR_COMReconfigure(unsigned int HCITransportID, HCI_Driver_Reconfigure_Data_t *DriverReconfigureData)
{
}

int BTPSAPI HCITR_COMWrite(unsigned int HCITransportID, unsigned int Length, unsigned char *Buffer)
{
    int ret=0;
    if(HCITransportID) {
        ret=Bluetopia::getInstance()->write_blocking(reinterpret_cast<char*>(Buffer), Length);
    } else {
        ret=-1;
    }
    return ret;
}

int BTPSAPI HCITR_COMSuspend(unsigned int HCITransportID)
{
    int ret=-1;
    if(HCITransportID) {
    } else {
        ret =-1;
    }
    return ret;
}

int BTPSAPI HCITR_EnableDebugLogging(Boolean_t Enable)
{
    Bluetopia::getInstance()->set_logging(BTdev::LogError);
    return 0;
}

};
