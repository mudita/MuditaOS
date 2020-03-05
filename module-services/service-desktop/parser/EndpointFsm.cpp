#include "EndpointFsm.hpp"
#include "EndpointHandler.hpp"
#include "ParserFsm.hpp"
#include "log/log.hpp"

class StateEndpointBattery;
class StateEndpointUpdate;
class StateEndpointDeviceInfo;
class StateEndpointNetwork;
class StateEndpointStorage;

// ----------------------------------------------------------------------------
// Endpoint FSM states
//

class StateDecodeJson : public EndpointFsm
{
    void react(EndpointEvt const &msg) override
    {
        std::string errorString;

        json11::Json msgJson = json11::Json::parse(ParserFsm::msgPayload, errorString);
        if (msgJson.is_null())
        {
            LOG_ERROR("Can't parse data as JSON [%s]", errorString.c_str());
            return;
        }

        method       = msgJson[parserutils::json::method].number_value();
        uuid         = msgJson[parserutils::json::uuid].number_value();
        body         = msgJson[parserutils::json::body];
        ownerService = msg.ownerService;

        if (!parserutils::http::isMethodValid(method))
        {
            return;
        }

        endpoint = static_cast<parserutils::Endpoint>(msgJson[parserutils::json::endpoint].number_value());

        switch (endpoint)
        {
        case parserutils::Endpoint::battery:
            transit<StateEndpointBattery>();
            break;
        case parserutils::Endpoint::update:
            transit<StateEndpointUpdate>();
            break;
        case parserutils::Endpoint::deviceInfo:
            transit<StateEndpointDeviceInfo>();
            break;
        case parserutils::Endpoint::network:
            transit<StateEndpointNetwork>();
            break;
        case parserutils::Endpoint::storage:
            transit<StateEndpointStorage>();
            break;
        }
    };
};

class StateEndpointDeviceInfo : public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointDeviceInfo *** uuid:%d", uuid);

        EndpointHandler endpointHandler = EndpointHandler();

        std::string responseStr;
        sys::ReturnCodes retCode = endpointHandler.deviceInfo(method, uuid, body, responseStr, ownerService);
        LOG_DEBUG("endpointHandler.deviceInfo retCode %d uuid %d", retCode, uuid);

        putToSendQueue(responseStr);
        transit<StateDecodeJson>();
    };
};

class StateEndpointBattery : public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointBattery ***");
        transit<StateDecodeJson>();
    };
};

class StateEndpointUpdate : public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointUpdate ***");

        EndpointHandler endpointHandler = EndpointHandler();

        std::string responseStr;
        sys::ReturnCodes retCode = endpointHandler.update(method, uuid, body, responseStr, ownerService);
        LOG_DEBUG("endpointHandler.deviceInfo retCode %d uuid %d", retCode, uuid);

        putToSendQueue(responseStr);
        transit<StateDecodeJson>();

        LOG_DEBUG("*** entry StateEndpointUpdate ***");
    };
};

class StateEndpointNetwork : public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointNetwork ***");
        transit<StateDecodeJson>();
    };
};

class StateEndpointStorage : public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointStorage ***");

        EndpointHandler endpointHandler = EndpointHandler();

        std::string responseStr;
        sys::ReturnCodes retCode = endpointHandler.storage(method, uuid, body, responseStr, ownerService);
        LOG_DEBUG("endpointHandler.deviceInfo retCode %d uuid %d", retCode, uuid);

        putToSendQueue(responseStr);
        transit<StateDecodeJson>();

        LOG_DEBUG("*** entry StateEndpointStorage ***");
    };
};

// ----------------------------------------------------------------------------
// Base State: default implementations
//

void EndpointFsm::react(EndpointEvt const &)
{
    LOG_DEBUG("EndpointEvt ignored");
}

bool EndpointFsm::putToSendQueue(std::string sendMsg)
{
    if (uxQueueSpacesAvailable(sendQueue) != 0)
    {
        std::string *responseString = new std::string(sendMsg);
        xQueueSend(sendQueue, &responseString, portMAX_DELAY);
        return true;
    }
    else
    {
        LOG_DEBUG("Endpoint Send Queue full, response lost");
        return false;
    }
}

xQueueHandle EndpointFsm::sendQueue;
parserutils::Endpoint EndpointFsm::endpoint;
uint8_t EndpointFsm::method;
uint32_t EndpointFsm::uuid;
json11::Json EndpointFsm::body;
sys::Service *EndpointFsm::ownerService;

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(EndpointFsm, StateDecodeJson)
