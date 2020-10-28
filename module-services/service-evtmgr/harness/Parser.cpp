// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Parser.hpp"

#include <Service/Bus.hpp>              // for Bus
#include <json/json11.hpp>              // for Json, json11
#include <log/log.hpp>                  // for LOG_DEBUG, LOG_ERROR
#include <service-evtmgr/Constants.hpp> // for evt_manager
#include <memory>                       // for shared_ptr

#include "Events.hpp"              // for Events, Type, Data, Events::GPIO, Events::GSMCmd, Events::KeyPress
#include "actions/CellularCMD.hpp" // for gsm_send
#include "events/GPIO.hpp"         // for GPIO
#include "events/KeyPress.hpp"     // for KeyPress

namespace sys
{
    class DataMessage;
    class Service;
} // namespace sys

namespace harness
{

    using namespace json11;

    /// super simple parser - frame is json
    /// format is explained in Events.hpp
    /// todo have them all somehow nice { from , to, what }
    auto parse(const std::string &request, sys::Service *serv) -> Error
    {

        std::string err;
        // TODO measure stack before and after..
        json11::Json el = Json::parse(request, err);
        if (err.size() != 0 || el.is_object() != true) {
            LOG_ERROR(
                "%s >> %s : %d : %d : %d", request.c_str(), err.c_str(), el.is_array(), el.is_object(), el.type());
            return Error::BadRequest;
        }
        else {
            if (el[Type].is_null()) {
                std::shared_ptr<sys::DataMessage> ptr = std::shared_ptr<sys::DataMessage>(nullptr);
                return Error::NoType;
            }
            switch (el[Type].int_value()) {
            case (int)Events::KeyPress: {
                auto evt = events::KeyPress(el);
                if (evt && evt.msg) {
                    sys::Bus::SendUnicast(evt.msg, service::name::evt_manager, serv);
                }
            } break;
            case static_cast<int>(Events::GSMCmd): {
                bool ret = action::gsm_send(serv, el[Data].string_value());
                if (!ret) {
                    return Error::ParserFailed;
                }
            } break;
            case (int)Events::GPIO: {
                auto evt = events::GPIO(el);
                if (!evt) {
                    LOG_DEBUG("GPIO processing request failed");
                }
                else if (evt.msg) {
                    sys::Bus::SendUnicast(evt.msg, service::name::evt_manager, serv);
                }
                else {
                    LOG_DEBUG("> evt: %s", evt.encode().c_str());
                }
            } break;
            default:
                break;
            };
            return Error::Success;
        }
        return Error::Generic;
    }

}; // namespace harness
