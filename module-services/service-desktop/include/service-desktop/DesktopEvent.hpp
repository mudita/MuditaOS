#pragma once

#include <endpoints/Context.hpp>

namespace sdesktop
{
    class Event
    {
      protected:
        endpoints::Context context;

      public:
        void send();
        virtual ~Event() = default;
    };
} // namespace sdesktop
