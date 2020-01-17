#include "Parser.hpp"
#include "Events.hpp"
#include "events/KeyPress.hpp"
#include <Service/Bus.hpp>
#include <json/json11.hpp>
#include <log/log.hpp>

namespace harness
{

    using namespace json11;

    /// super simple parser - frame is json
    /// format is explained in Events.hpp
    std::pair<Error, std::shared_ptr<sys::DataMessage>> parse(const std::string &request)
    {

        std::string err;
        std::shared_ptr<sys::DataMessage> msg = nullptr;
        // TODO measure stack before and after..
        json11::Json el = Json::parse(request, err);
        if (err.size() != 0 || el.is_object() != true)
        {
            LOG_ERROR("%s : %d : %d : %d", err.c_str(), el.is_array(), el.is_object(), el.type());
            return {Error::BadRequest, nullptr};
        }
        else
        {
            if (el[Type].is_null())
            {
                std::shared_ptr<sys::DataMessage> ptr = std::shared_ptr<sys::DataMessage>(nullptr);
                std::pair<Error, std::shared_ptr<sys::DataMessage>> ret = {Error::NoType, ptr};
                return ret;
            }
            switch (el[Type].int_value())
            {
            case (int)Events::KeyPress: {
                msg = parseKeyPress(el);
            }
            break;
            default:
                return {Error::NotHandled, nullptr};
            };
            return {Error::Success, msg};
        }
        return {Error::Generic, nullptr};
    }

}; // namespace harness
