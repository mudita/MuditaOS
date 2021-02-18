#pragma once

#include <service-desktop/endpoints/Context.hpp>

namespace sdesktop
{
    class Event
    {
      protected:
        parserFSM::Context context;

      public:
        void send();
        virtual ~Event() = default;
    };
} // namespace sdesktop
