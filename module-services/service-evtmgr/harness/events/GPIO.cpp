#include "GPIO.hpp"
#include "../../messages/EVMessages.hpp"
#include <log/log.hpp>
#include <map>

#include "bsp/harness/bsp_harness.hpp"

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
        using namespace bsp::harness;
        auto data = js[Data];
        if (!data.is_object())
        {
            LOG_ERROR("Bad data type: %d: %s", data.type(), data.string_value().c_str());
            return false;
        }
        // load data one by one
        for (auto &el : this->data)
        {
            this->data[el.first] = data[el.first].int_value();
        }

        if (this->data[in_s] == 1)
        {
            auto val = gpio::read(gpio::to(this->data[num_s]));
            LOG_DEBUG("Pin %d state: %d", gpio::to(this->data[num_s]), val);
        }
        else
        {
            LOG_DEBUG("Writting gpio: %d state: %d", gpio::to(this->data[num_s]), this->data[state_s]);
            gpio::write(gpio::to(this->data[num_s]), this->data[state_s]);
        }

        create_message();
        return true;
    }

    void GPIO::create_message()
    {
        auto msg = std::make_shared<sevm::message::GPIO>();
        msg->num = data[num_s];
        msg->port = data[state_s];
        msg->state = data[in_s];
        this->msg = msg;
    }
} // namespace harness::events
