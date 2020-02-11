#include "GPIO.hpp"
#include "../../messages/EVMessages.hpp"
#include <log/log.hpp>
#include <map>

namespace harness::events
{
    using namespace json11;

    GPIO::GPIO(const json11::Json &js)
    {
        state = this->decode(js);
    }

    auto GPIO::encode() -> std::string
    {
        Json::object el;
        el[Type] = (int)evt;
        el[Data] = data;
        return Json(el).dump();
    }

    auto GPIO::decode(const json11::Json &js) -> bool
    {
        auto data = js[Data];
        if (!data.is_object())
        {
            LOG_ERROR("Bad data type");
            return false;
        }
        // load data one by one
        for (auto &el : this->data)
        {
            this->data[el.first] = data[el.first].int_value();
        }
        create_message();
        return true;
    }

    void GPIO::create_message()
    {
        auto msg = std::make_shared<sevm::message::GPIO>();
        msg->num = data["num"];
        msg->port = data["port"];
        msg->state = data["state"];
        this->msg = msg;
    }
} // namespace harness::events
