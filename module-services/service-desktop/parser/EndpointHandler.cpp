#include "EndpointHandler.hpp"
#include "json/json11.hpp"
#include "log/log.hpp"
#include "SerialParser.hpp"
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

// json11::Json testPayloadJson = json11::Json::object ({
//     { "level", 75 },
//     { "charging", true },
//     { "maximumCapacity", 100 }
// });

// json11::Json testJson = json11::Json::object({
//     { "endpoint", BATTERY },
//     { "status", 200 },
//     { "payload", testPayloadJson },
// });
// std::string testResponseStr = testJson.dump();

class StateEndpointBattery;
class StateEndpointBackups;
class StateEndpointDeviceInfo;
class StateEndpointNetwork;
class StateEndpointStorage;

// ----------------------------------------------------------------------------
// Endpoint handler states
//

class StateDecodeJson
: public EndpointHandler
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateDecodeJson ***");
    };

    void react(EndpointEvt const &) override
    {
        LOG_DEBUG("EndpointHandler: StateDecodeJson");

        std::string errorString;

        json11::Json msgJson = json11::Json::parse(SerialParser::msgPayload, errorString);
        if (msgJson.is_null())
        {
            LOG_ERROR("Can't parse data as JSON [%s]", errorString.c_str());
        }
        LOG_DEBUG("SerialParser::msgPayload [%s]", SerialParser::msgPayload.c_str());

        method = msgJson["method"].number_value();
        LOG_DEBUG("method [%d]", method);

        switch (method)
        {
        case GET:
            // It's ok, do nothing
            break;
        case POST:
        case DELETE: //not sure if its needed
            if (!msgJson["payload"].is_object() || msgJson["payload"].is_null())
            {
                LOG_ERROR("Invalid payload!");
                return;
            }
            break;
        default:
            LOG_ERROR("Invalid method!");
            return;
        }

        endpoint = msgJson["endpoint"].number_value();
        LOG_DEBUG("Endpoint [%d]", endpoint);

        switch (endpoint)
        {
        case BATTERY:
            LOG_DEBUG("payload [%s]", msgJson["payload"].dump().c_str());
            transit<StateEndpointBattery>();
            break;
        case BACKUPS:
            transit<StateEndpointBackups>();
            break;
        case DEVICE_INFO:
            transit<StateEndpointDeviceInfo>();
            break;
        case NETWORK:
            transit<StateEndpointNetwork>();
            break;
        case STORAGE:
            transit<StateEndpointStorage>();
            break;
        
        default:
            LOG_ERROR("Invalid Endpoint!");
            return;
        }
    };
};

class StateEndpointBattery
: public EndpointHandler
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateEndpointBattery ***");
        //interprete payload and prepare answer if possible
        //othervise, go to StateDecodeJson

        //sample json response
        json11::Json responsePayloadJson = json11::Json::object ({
            { "level", 75 },
            { "charging", true },
            { "maximumCapacity", 100 }
        });

        json11::Json responseJson = json11::Json::object({
            { "endpoint", BATTERY },
            { "status", 200 },
            { "payload", responsePayloadJson },
        });

        std::string responseStr = responseJson.dump();
        LOG_DEBUG("EndpointBattery responseStr [%s]", responseStr.c_str());
        desktopServiceSend(-1, (uint8_t *)responseStr.c_str(), responseStr.length());

        // LOG_DEBUG("CommandBattery testResponseStr [%s]", testResponseStr.c_str());
        // desktopServiceSend(-1, (uint8_t *)testResponseStr.c_str(), testResponseStr.length());

        transit<StateDecodeJson>();
    };
};

class StateEndpointBackups
: public EndpointHandler
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
: public EndpointHandler
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
: public EndpointHandler
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
: public EndpointHandler
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

void EndpointHandler::react(EndpointEvt const &) {
  LOG_DEBUG("EndpointEvt ignored");
}


uint8_t     EndpointHandler::endpoint;
uint8_t     EndpointHandler::method;

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(EndpointHandler, StateDecodeJson)
