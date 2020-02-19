#include "EndpointFsm.hpp"
#include "EndpointHandler.hpp"
#include "json/json11.hpp"
#include "log/log.hpp"
#include "ParserUtils.hpp"
#include "SerialParserFsm.hpp"
#if defined(TARGET_RT1051)
extern "C"
{
#include "../rt1051/include/virtual_com.h"
}
#include "../rt1051/Worker.hpp"
#else
#include "../linux/Worker.hpp"
#endif
extern xQueueHandle USBSendQueue;

class StateEndpointBattery;
class StateEndpointBackups;
class StateEndpointDeviceInfo;
class StateEndpointNetwork;
class StateEndpointStorage;

// ----------------------------------------------------------------------------
// Endpoint FSM states
//

class StateDecodeJson
: public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateDecodeJson ***");
    };

    void react(EndpointEvt const &) override
    {
        std::string errorString;

        json11::Json msgJson = json11::Json::parse(SerialParserFsm::msgPayload, errorString);
        if (msgJson.is_null())
        {
            LOG_ERROR("Can't parse data as JSON [%s]", errorString.c_str());
        }

        method = msgJson[parserutils::json::method].number_value();

        if (!parserutils::http::isMethodValid(method))
        {
            return;
        }

        endpoint = msgJson[parserutils::json::endpoint].number_value();

        switch (endpoint)
        {
        case static_cast<uint8_t>(parserutils::Endpoint::battery):
            transit<StateEndpointBattery>();
            break;
        case static_cast<uint8_t>(parserutils::Endpoint::backups):
            transit<StateEndpointBackups>();
            break;
        case static_cast<uint8_t>(parserutils::Endpoint::deviceInfo):
            transit<StateEndpointDeviceInfo>();
            break;
        case static_cast<uint8_t>(parserutils::Endpoint::network):
            transit<StateEndpointNetwork>();
            break;
        case static_cast<uint8_t>(parserutils::Endpoint::storage):
            transit<StateEndpointStorage>();
            break;
        
        default:
            LOG_ERROR("Invalid Endpoint!");
            return;
        }
    };
};

class StateEndpointBattery
: public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointBattery ***");

        EndpointHandler endpointHandler = EndpointHandler();

        std::string responseStr;
        sys::ReturnCodes retCode = endpointHandler.battery(method, responseStr);
        LOG_DEBUG("endpointHandler.battery retCode %d", retCode);

        // xQueueSend(USBSendQueue, &responseStr, portMAX_DELAY);

        LOG_DEBUG("%s", responseStr.c_str());
        transit<StateDecodeJson>();
    };
};

class StateEndpointBackups
: public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointBackups ***");

        EndpointHandler endpointHandler = EndpointHandler();
        std::string responseStr;
        // sys::ReturnCodes retCode = endpointHandler.backups(method, responseStr);
        // xQueueSend(USBSendQueue, &responseStr, portMAX_DELAY);

        transit<StateDecodeJson>();
    };
};

class StateEndpointDeviceInfo
: public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointDeviceInfo ***");

        EndpointHandler endpointHandler = EndpointHandler();
        std::string responseStr;
        // sys::ReturnCodes retCode = endpointHandler.deviceInfo(method, responseStr);
        // xQueueSend(USBSendQueue, &responseStr, portMAX_DELAY);

        transit<StateDecodeJson>();
    };
};

class StateEndpointNetwork
: public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointNetwork ***");

        EndpointHandler endpointHandler = EndpointHandler();
        std::string responseStr;
        // sys::ReturnCodes retCode = endpointHandler.network(method, responseStr);
        // xQueueSend(USBSendQueue, &responseStr, portMAX_DELAY);

        transit<StateDecodeJson>();
    };
};

class StateEndpointStorage
: public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointStorage ***");

        EndpointHandler endpointHandler = EndpointHandler();
        std::string responseStr;
        // sys::ReturnCodes retCode = endpointHandler.storage(method, responseStr);
        // xQueueSend(USBSendQueue, &responseStr, portMAX_DELAY);

        transit<StateDecodeJson>();
    };
};


// ----------------------------------------------------------------------------
// Base State: default implementations
//

void EndpointFsm::react(EndpointEvt const &)
{
    LOG_DEBUG("EndpointEvt ignored");
}

uint8_t     EndpointFsm::endpoint;
uint8_t     EndpointFsm::method;

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(EndpointFsm, StateDecodeJson)
