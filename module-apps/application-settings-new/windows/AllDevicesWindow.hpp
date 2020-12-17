// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionWindow.hpp"

#include <Device.hpp>

namespace gui
{
    class AllDevicesWindow : public OptionWindow
    {
      public:
        explicit AllDevicesWindow(app::Application *app);

      private:
        auto allDevicesOptionsList(const std::vector<Devicei> &vector) -> std::list<Option>;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto onInput(const InputEvent &inputEvent) -> bool override;

        Image *leftArrowImage = nullptr;
        Image *crossImage     = nullptr;
    };

    class BondedDevicesData : public SwitchData
    {
      public:
        explicit BondedDevicesData(std::vector<Devicei> devices) : SwitchData(), devices(std::move(devices))
        {}
        [[nodiscard]] auto getDevices() const -> const std::vector<Devicei> &
        {
            return devices;
        }

      private:
        const std::vector<Devicei> devices;
    };
} // namespace gui
