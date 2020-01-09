#include "ServiceDesktop.hpp"
#include "MessageType.hpp"
#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "json/json11.hpp"
#include <errno.h>
#include <log/log.hpp>
#include <string.h>

const char *ServiceDesktop::serviceName = "ServiceDesktop";

#if defined(TARGET_RT1051)
extern "C"
{
#include "rt1051/include/virtual_com.h"
}
#include "rt1051/Worker.hpp"
#else
#include "linux/Worker.hpp"
#endif
using namespace json11;

ServiceDesktop::ServiceDesktop() : sys::Service(serviceName), taskHandle(nullptr), ptyDescriptor(-1)
{
    LOG_INFO("ServiceDesktop::ctor");
}

sys::ReturnCodes ServiceDesktop::InitHandler()
{
    if ((ptyDescriptor = desktop_init()) < 0)
    {
        LOG_ERROR("won't start desktop service without serial port");
        return (sys::ReturnCodes::Failure);
    }
    else
    {
        BaseType_t task_error = xTaskCreate(desktop_receive, "USB data reception task", SERIAL_BUFFER_LEN * 8, this, tskIDLE_PRIORITY, &taskHandle);
        if (task_error != pdPASS)
        {
            LOG_ERROR("failed to start freertos core task");
            return (sys::ReturnCodes::Failure);
        }
    }
    return (sys::ReturnCodes::Success);
}

sys::ReturnCodes ServiceDesktop::DeinitHandler()
{
    return sys::ReturnCodes::Success;
}

sys::Message_t ServiceDesktop::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
{
    return std::make_shared<sys::ResponseMessage>();
}

sys::ReturnCodes ServiceDesktop::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    return sys::ReturnCodes::Success;
}

int ServiceDesktop::getPtyDescriptor()
{
    return (ptyDescriptor);
}

/*
 * send data to the application
 */
int ServiceDesktop::sendData(const char *rawDataToSend, size_t rawDataSize)
{
    return (desktop_send(ptyDescriptor, (uint8_t *)rawDataToSend, rawDataSize));
}

Json fileItem(const vfs::DirectoryEntry &e)
{
    return (Json::object{{"name", e.fileName}, {"size", (int)e.fileSize}, {"attrs", (int)e.attributes}});
}

void ServiceDesktop::dataReceived(const uint8_t *data, const ssize_t dataLen)
{
    std::string possibleJsonData((const char *)data, (size_t)dataLen);
    std::string errorString;
    LOG_INFO("dataReceived \"%s\"", data);
    json11::Json request = json11::Json::parse(possibleJsonData, errorString);
    if (request.is_null())
    {
        LOG_ERROR("Can't parse data as JSON [%s]", errorString.c_str());
    }
    else
    {
        LOG_INFO("got serial command: %s", request["command"].string_value().c_str());
        if (request["command"].string_value() == "running_tasks")
        {
            Json my_json = Json::object{{"tasks", (int)uxTaskGetNumberOfTasks()}};
            std::string responseStr = my_json.dump();
            sendData(responseStr.c_str(), responseStr.length());
        }

        if (request["command"].string_value() == "kernel_version")
        {
            Json my_json = Json::object{{"version", tskKERNEL_VERSION_NUMBER}};
            std::string responseStr = my_json.dump();
            sendData(responseStr.c_str(), responseStr.length());
        }

        if (request["command"].string_value() == "list_root")
        {
            Json::array fileListAsJsonObjects;
            std::vector<vfs::DirectoryEntry> root_files = vfs.listdir("sys");

            for (auto &f : root_files)
            {
                fileListAsJsonObjects.push_back(fileItem(f));
            }
            const std::string responseStr = Json(fileListAsJsonObjects).dump();
            sendData(responseStr.c_str(), responseStr.length());
        }
    }
}
