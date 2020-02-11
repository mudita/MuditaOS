#pragma once

#include "../Events.hpp"

namespace harness::events
{
    class GPIO : public Event<harness::Events::GPIO>
    {
      public:
        ~GPIO() override = default;
        GPIO(const json11::Json &js);
        auto encode() -> std::string final;
        auto decode(const json11::Json &js) -> bool final;

      private:
        std::map<std::string, int> data = {{"num", 0}, {"port", 0}, {"state", 0}};
        void create_message();
    };
} // namespace harness::events
