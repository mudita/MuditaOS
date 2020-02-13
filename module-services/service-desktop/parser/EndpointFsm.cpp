#include "EndpointFsm.hpp"
#include "EndpointHandler.hpp"
#include "json/json11.hpp"
#include "log/log.hpp"
#include "ParserUtils.hpp"
#include "SerialParserFsm.hpp"
#include "../ServiceDesktop.hpp"
#if defined(TARGET_RT1051)
extern "C"
{
#include "../rt1051/include/virtual_com.h"
}
#include "../rt1051/Worker.hpp"
#else
#include "../linux/Worker.hpp"
#endif


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
        LOG_DEBUG("EndpointFsm: StateDecodeJson");

        ParserUtils parserUtils;
        std::string errorString;

        json11::Json msgJson = json11::Json::parse(SerialParserFsm::msgPayload, errorString);
        if (msgJson.is_null())
        {
            LOG_ERROR("Can't parse data as JSON [%s]", errorString.c_str());
        }
        LOG_DEBUG("SerialParser::msgPayload [%s]", SerialParserFsm::msgPayload.c_str());

        method = msgJson["method"].number_value();
        LOG_DEBUG("method [%d]", method);

        if(!parserUtils.isMethodValid(method))
        {
            return;
        }

        // if (!msgJson["payload"].is_object() || msgJson["payload"].is_null())
        //     {
        //         LOG_ERROR("Invalid payload!");
        //         return;
        //     }

        endpoint = msgJson["endpoint"].number_value();
        LOG_DEBUG("Endpoint [%d]", endpoint);

        switch (endpoint)
        {
        case Battery:
            transit<StateEndpointBattery>();
            break;
        case Backups:
            transit<StateEndpointBackups>();
            break;
        case DeviceInfo:
            transit<StateEndpointDeviceInfo>();
            break;
        case Network:
            transit<StateEndpointNetwork>();
            break;
        case Storage:
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
        //interprete payload and prepare answer if possible
        //othervise, go to StateDecodeJson

        EndpointHandler endpointHandler;

        //sample json response
        // json11::Json responseBodyJson = json11::Json::object ({
        //     { "level", 75 },
        //     { "charging", true },
        //     { "maximumCapacity", 100 }
        // });

        // int statusCode = static_cast<int>(Http::Code::OK);

        // json11::Json responsePayloadJson = json11::Json::object({
        //     { "endpoint", Battery },
        //     { "status", statusCode},
        //     { "body", responseBodyJson }
        // });

        // std::string responseSizeStr = EndpointFsm::rspPayloadSizeToStr(responsePayloadJson.dump().size());
        // std::string responseHeaderStr = "#" + responseSizeStr;
        // std::string responseStr = responseHeaderStr + responsePayloadJson.dump();
        // LOG_DEBUG("EndpointBattery responseStr [%s]", responseStr.c_str());
        // desktopServiceSend(-1, (uint8_t *)responseStr.c_str(), responseStr.length());

        endpointHandler.battery(method);

        transit<StateDecodeJson>();
    };
};

class StateEndpointBackups
: public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointBackups ***");
        //interprete payload and prepare answer if possible
        //othervise, go to StateDecodeJson
        transit<StateDecodeJson>();
    };
};

class StateEndpointDeviceInfo
: public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointDeviceInfo ***");
        //interprete payload and prepare answer if possible
        //othervise, go to StateDecodeJson
        transit<StateDecodeJson>();
    };
};

class StateEndpointNetwork
: public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointNetwork ***");
        //interprete payload and prepare answer if possible
        //othervise, go to StateDecodeJson
        transit<StateDecodeJson>();
    };
};

class StateEndpointStorage
: public EndpointFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointStorage ***");
        //interprete payload and prepare answer if possible
        //othervise, go to StateDecodeJson
        transit<StateDecodeJson>();
    };
};


// ----------------------------------------------------------------------------
// Base State: default implementations
//

void EndpointFsm::react(EndpointEvt const &) {
  LOG_DEBUG("EndpointEvt ignored");
}

std::string EndpointFsm::rspPayloadSizeToStr(std::size_t rspSize){
    std::string rspPayloadSizeStr = std::to_string(rspSize);
    while (rspPayloadSizeStr.length() < 9)
    {
        rspPayloadSizeStr.insert(0, 1, '0');
    }
    return rspPayloadSizeStr;
}


uint8_t     EndpointFsm::endpoint;
uint8_t     EndpointFsm::method;

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(EndpointFsm, StateDecodeJson)
