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
    /// format is:
    /// "t" : type (of harness::Events type )
    /// "d" : object of data
    /// than this js goes to praser of selected event
    std::pair<Error, std::shared_ptr<sys::Message>> parse(std::string request)
    {
        json11::Json el;
        std::string err;
        std::shared_ptr<sys::Message> msg = nullptr;

        el.parse(request, err);
        if (err.size() != 0 || el.is_object() != true)
        {
            return {Error::BadRequest, nullptr};
        }
        else
        {
            Json val = el["t"];
            if (val.is_null())
            {
                return {Error::NoType, nullptr};
            }
            switch (el["t"].int_value())
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
