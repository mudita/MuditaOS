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

static Json fileItem(const vfs::DirectoryEntry &e)
{
    return (Json::object{{"name", e.fileName}, {"size", (int)e.fileSize}, {"attrs", (int)e.attributes}});
}

static Json contactItem(const ContactRecord &item)
{
    const std::string primaryNumber = item.numbers[0].numberE164.c_str();
    const std::string secondaryNumber = (item.numbers.size() == 2) ? item.numbers[1].numberE164.c_str() : "";

    return (Json::object{{"primaryName", item.primaryName.c_str()},
                         {"alternativeName", item.alternativeName.c_str()},
                         {"mail", item.mail.c_str()},
                         {"primaryNumber", primaryNumber.c_str()},
                         {"secondaryNumber", secondaryNumber.c_str()}});
}

ServiceDesktop::ServiceDesktop() : sys::Service(serviceName), taskHandle(nullptr), ptyDescriptor(-1)
{
    LOG_INFO("ServiceDesktop::ctor");
}

sys::ReturnCodes ServiceDesktop::InitHandler()
{
    if ((ptyDescriptor = desktopServiceInit()) < 0)
    {
        LOG_ERROR("won't start desktop service without serial port");
        return (sys::ReturnCodes::Failure);
    }
    else
    {
        BaseType_t task_error = xTaskCreate(desktopServiceReceive, "USBRead", SERIAL_BUFFER_LEN * 8, this, tskIDLE_PRIORITY, &taskHandle);
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
    // handle database response
    if (resp != nullptr)
    {

        uint32_t msgType = resp->responseTo;
        switch (msgType)
        {
        case static_cast<uint32_t>(MessageType::DBContactGetLimitOffset): {
            DBContactResponseMessage *dbResp = reinterpret_cast<DBContactResponseMessage *>(resp);
            LOG_INFO("DBContactGetLimitOffset count=%d offset=%d favs=%d", dbResp->count, dbResp->offset, dbResp->favourite);
            sendRecorsInResponse(std::move(dbResp->records), dbResp->offset, dbResp->limit, dbResp->count, dbResp->favourite);
            break;
        }
        case static_cast<uint32_t>(MessageType::DBContactGetCount): {
            DBContactResponseMessage *dbResp = reinterpret_cast<DBContactResponseMessage *>(resp);
            LOG_INFO("DataReceivedHandler db count: %d", dbResp->count);
            Json my_json = Json::object{{"count", (int)dbResp->count}};
            std::string responseStr = my_json.dump();
            sendData(responseStr.c_str(), responseStr.length());
            break;
        }
        break;
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}

void ServiceDesktop::sendRecorsInResponse(std::unique_ptr<std::vector<ContactRecord>> records, const uint32_t offset, const uint32_t limit, uint32_t count,
                                          bool favourite)
{
    std::vector<ContactRecord> contacts = *records.get();
    Json::array contactsResponse;
    for (uint32_t i = 0; i < contacts.size(); i++)
    {
        contactsResponse.push_back(contactItem(contacts[i]));
    }
    const std::string responseStr = Json(contactsResponse).dump();
    sendData(responseStr.c_str(), responseStr.length());
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
    return (desktopServiceSend(ptyDescriptor, (uint8_t *)rawDataToSend, rawDataSize));
}

void ServiceDesktop::dataReceived(const uint8_t *data, const ssize_t dataLen)
{
    std::string possibleJsonData((const char *)data, (size_t)dataLen);
    std::string errorString;
    LOG_INFO("dataReceived len: %d data:\"%s\"", dataLen, data);
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

        if (request["command"].string_value() == "contact-count")
        {
            DBServiceAPI::ContactGetCount(this);
        }

        if (request["command"].string_value() == "contact-list")
        {
            int offset = request["offset"].int_value();
            int limit = request["limit"].int_value();
            bool favs = request["favs"].bool_value();
            LOG_INFO("offset:%d limit:%d favs:%d", offset, limit, favs);
            DBServiceAPI::ContactGetLimitOffset(this, offset, limit, favs);
        }
    }
}
