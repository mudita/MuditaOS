// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/PacketDataTypes.hpp>
#include <module-apps/Application.hpp>

class ApnSettingsModel
{
  public:
    ApnSettingsModel(app::Application *application);

    void requestAPNList();
    void saveAPN(std::shared_ptr<packet_data::APN::Config> apn);
    void removeAPN(std::shared_ptr<packet_data::APN::Config> apn);
    void setAsDefaultAPN(std::shared_ptr<packet_data::APN::Config> apn);

  private:
    app::Application *application = nullptr;
};
