// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/Application.hpp>

#include <memory>
#include <string>

namespace app
{
    class ApplicationLauncher;
} // namespace app

namespace app::manager
{
    class ApplicationHandle
    {
      public:
        using State = app::Application::State;

        explicit ApplicationHandle(std::unique_ptr<app::ApplicationLauncher> &&_launcher);

        void setState(State state) noexcept;
        void run(sys::phone_modes::PhoneMode phoneMode,
                 sys::bluetooth::BluetoothMode bluetoothMode,
                 sys::Service *caller);
        void runInBackground(sys::phone_modes::PhoneMode phoneMode,
                             sys::bluetooth::BluetoothMode bluetoothMode,
                             sys::Service *caller);
        void close() noexcept;

        auto valid() const noexcept -> bool;
        auto name() const -> ApplicationName;
        auto state() const noexcept -> State;
        auto preventsAutoLocking() const noexcept -> bool;
        auto closeable() const noexcept -> bool;
        auto started() const noexcept -> bool;
        auto handles(actions::ActionId action) const noexcept -> bool;
        auto actionFlag(actions::ActionId action) const noexcept -> actions::ActionFlag;

        std::unique_ptr<app::ApplicationLauncher> launcher; // Handle to the application's start function.
        std::unique_ptr<gui::SwitchData> switchData;
        std::string switchWindow;

        StartupReason startupReason = StartupReason::Launch; // Informs application about startup reason.

      private:
        auto checkBlockClosing() const noexcept -> bool;
        auto getManifest() const -> const ApplicationManifest &;
    };
} // namespace app::manager
