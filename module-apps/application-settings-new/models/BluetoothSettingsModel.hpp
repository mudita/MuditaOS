// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"

extern "C"
{
#include <module-bluetooth/lib/btstack/src/btstack_util.h>
}

class BluetoothSettingsModel
{
  public:
    explicit BluetoothSettingsModel(app::Application *application);

    void requestStatus();
    void setStatus(bool desiredBluetoothState, bool desiredVisibility);
    void stopScan();
    void setAddrForAudioProfiles(std::string addr);

  private:
    app::Application *application = nullptr;
};
