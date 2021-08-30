// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-evtmgr/EventManagerCommon.hpp>

namespace hal::temperature
{
    class AbstractTemperatureSource;
}

class EventManager : public EventManagerCommon
{
  public:
    explicit EventManager(const std::string &name = service::name::evt_manager);

  private:
    void handleKeyEvent(sys::Message *msg) override;

  private:
    std::shared_ptr<hal::temperature::AbstractTemperatureSource> temperatureSource;
};

namespace sys
{
    template <> struct ManifestTraits<EventManager>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::evt_manager;
            manifest.dependencies = {service::name::db};
            return manifest;
        }
    };
} // namespace sys
