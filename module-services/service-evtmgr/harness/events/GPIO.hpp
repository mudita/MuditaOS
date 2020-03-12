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
        static inline const std::string num_s   = "num";
        static inline const std::string state_s = "state";
        static inline const std::string in_s    = "in";
        std::map<std::string, int> data         = {{num_s, 0}, {state_s, 0}, {in_s, 0}};
        void create_message();
    };
} // namespace harness::events
