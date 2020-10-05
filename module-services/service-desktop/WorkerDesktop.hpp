#pragma once

#include <string.h>
#include <module-os/RTOSWrapper/include/ticks.hpp>

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "message_buffer.h"
#include "event_groups.h"
#include "usb.h"
#include "usb_device.h"
#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"
#include "composite.h"
#include "virtual_com_demo.h"
}

#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "Service/Worker.hpp"
#include "parser/ParserFSM.hpp"
#include "bsp/usb/usb.hpp"
#include "vfs.hpp"
#include "timer.hpp"

class WorkerDesktop : public sys::Worker, public Timer
{
  public:
    enum class TransferType : uint8_t
    {
        UploadFile,
        DownloadFile,
        JSONCommands
    };
    WorkerDesktop(sys::Service *ownerServicePtr);

    virtual bool init(std::list<sys::WorkerQueueInfo> queues) override;
    virtual bool deinit() override;
    bool handleMessage(uint32_t queueID) override final;

    sys::Service *ownerService = nullptr;
    parserFSM::StateMachine parser;

    const std::string RECEIVE_QUEUE_BUFFER_NAME = "receiveQueueBuffer";
    const std::string SEND_QUEUE_BUFFER_NAME    = "sendQueueBuffer";

    usb_cdc_vcom_struct_t *getCdcVcomStruct()
    {
        return cdcVcomStruct;
    }
    xQueueHandle getReceiveQueue()
    {
        return receiveQueue;
    }
    static void deviceTask(void *);

    sys::ReturnCodes startDownload(const fs::path &filePath, const uint32_t fileSize);
    sys::ReturnCodes startUpload(const fs::path &filePath, const uint32_t fileSize);
    void stopTransfer();
    void transferDataReceived(const char *data, uint32_t dataLen);
    WorkerDesktop::TransferType getTransferType();
    void Run();

  private:
    xQueueHandle receiveQueue;
    usb_cdc_vcom_struct_t *cdcVcomStruct = nullptr;
    vfs::FILE *fileDes = nullptr;
    uint32_t writeFileSizeExpected = 0;
    uint32_t writeFileDataWritten  = 0;

    volatile TransferType currentTransferType = TransferType::JSONCommands;
    cpp_freertos::MutexStandard mutex;
};
