#include "EndpointFsm.hpp"
#include "EndpointHandler.hpp"
#include "ParserFsm.hpp"
#include "log/log.hpp"
#include "json/json11.hpp"

class StateEndpointBattery;
class StateEndpointBackups;
class StateEndpointDeviceInfo;
class StateEndpointNetwork;
class StateEndpointStorage;

// ----------------------------------------------------------------------------
// Endpoint FSM states
//

class StateDecodeJson : public EndpointFsm
{
    void react(EndpointEvt const &) override
    {
        std::string errorString;

        json11::Json msgJson = json11::Json::parse(ParserFsm::msgPayload, errorString);
        if (msgJson.is_null()) {
            LOG_ERROR("Can't parse data as JSON [%s]", errorString.c_str());
            return;
        }

        method = msgJson[parserutils::json::method].number_value();

        if (!parserutils::http::isMethodValid(method)) {
            return;
        }

        endpoint = static_cast<parserutils::Endpoint>(msgJson[parserutils::json::endpoint].number_value());

        switch (endpoint) {
        case parserutils::Endpoint::battery:
            transit<StateEndpointBattery>();
            break;
        case parserutils::Endpoint::backups:
            transit<StateEndpointBackups>();
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

class StateEndpointBattery : public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointBattery ***");

        EndpointHandler endpointHandler = EndpointHandler();

        std::string responseStr;
        sys::ReturnCodes retCode = endpointHandler.battery(method, responseStr);
        LOG_DEBUG("endpointHandler.battery retCode %d", retCode);

        putToSendQueue(responseStr);

        transit<StateDecodeJson>();
    };
};

class StateEndpointBackups : public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointBackups ***");
        transit<StateDecodeJson>();
    };
};

class StateEndpointDeviceInfo : public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointDeviceInfo ***");
        transit<StateDecodeJson>();
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

bool EndpointFsm::putToSendQueue(std::string sendMsg)
{
    if (uxQueueSpacesAvailable(sendQueue) != 0) {
        std::string *responseString = new std::string(sendMsg);
        xQueueSend(sendQueue, &responseString, portMAX_DELAY);
        return true;
    }
    else {
        LOG_DEBUG("Endpoint Send Queue full, response lost");
        return false;
    }
}

xQueueHandle EndpointFsm::sendQueue;
parserutils::Endpoint EndpointFsm::endpoint;
uint8_t EndpointFsm::method;

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(EndpointFsm, StateDecodeJson)
