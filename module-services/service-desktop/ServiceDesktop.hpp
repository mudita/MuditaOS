#pragma once

#include "FreeRTOS.h"
#include "MessageType.hpp"
#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "Service/Worker.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "service-db/messages/DBMessage.hpp"
#include "task.h"
#include "vfs.hpp"
#include <memory>

#define SERIAL_TRANSMISSION_START 0x02
#define SERIAL_TRANSMISSION_END 0x03
#define SERIAL_SHELL_START 0x33
#define SERIAL_BAUDRATE 115200
#define SERIAL_BUFFER_LEN 512 // this matches the buffer length in rt1051 cdc implementaion

class ServiceDesktop : public sys::Service
{
  public:
    ServiceDesktop();
    ~ServiceDesktop() = default;

    virtual sys::Message_t DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    virtual sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;
    int getPtyDescriptor();
    int sendData(const char *rawDataToSend, size_t rawDataSize);
    void dataReceived(const uint8_t *data, const ssize_t dataLen);

  private:
    void sendRecordsInResponse(std::unique_ptr<std::vector<ContactRecord>> records, const uint32_t offset, const uint32_t limit, uint32_t count, bool favourite);

    static const char *serviceName;
    xTaskHandle taskHandle;
    bool isRunning;
    int ptyDescriptor;
};
