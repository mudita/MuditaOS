#include "Parser.hpp"
#include "Events.hpp"
#include "actions/CellularCMD.hpp"
#include "events/KeyPress.hpp"
#include <Service/Bus.hpp>
#include <json/json11.hpp>
#include <log/log.hpp>

namespace harness
{

    using namespace json11;

    /// super simple parser - frame is json
    /// format is explained in Events.hpp
    /// todo have them all somehow nice { from , to, what }
    Error parse(const std::string &request, sys::Service *serv)
    {

        std::string err;
        // TODO measure stack before and after..
        json11::Json el = Json::parse(request, err);
        if (err.size() != 0 || el.is_object() != true)
        {
            LOG_ERROR("%s >> %s : %d : %d : %d", request.c_str(), err.c_str(), el.is_array(), el.is_object(), el.type());
            return Error::BadRequest;
        }
        else
        {
            if (el[Type].is_null())
            {
                std::shared_ptr<sys::DataMessage> ptr = std::shared_ptr<sys::DataMessage>(nullptr);
                return Error::NoType;
            }
            switch (el[Type].int_value())
            {
            case static_cast<int>(Events::KeyPress): {
                auto msg = parseKeyPress(el);
                auto ret = sys::Bus::SendUnicast(msg, "EventManager", serv);
                if (!ret)
                {
                    LOG_ERROR("Message not sent!");
                }
            }
            break;
            case static_cast<int>(Events::GSMCmd): {
                bool ret = action::gsm_send(serv, el[Data].string_value());
                if (!ret)
                {
                    return Error::ParserFailed;
                }
            }
            break;
            default:
                return Error::NotHandled;
            };
            return Error::Success;
        }
        return Error::Generic;
    }

}; // namespace harness
