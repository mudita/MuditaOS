// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-apps/Application.hpp>
#include <module-apps/ApplicationLauncher.hpp>

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
        void run(sys::phone_modes::PhoneMode mode, sys::Service *caller);
        void runInBackground(sys::phone_modes::PhoneMode mode, sys::Service *caller);
        void close() noexcept;

        auto valid() const noexcept -> bool;
        auto name() const -> ApplicationName;
        auto state() const noexcept -> State;
        auto preventsAutoLocking() const noexcept -> bool;
        auto closeable() const noexcept -> bool;
        auto started() const noexcept -> bool;
        auto handles(actions::ActionId action) const noexcept -> bool;

        std::unique_ptr<app::ApplicationLauncher> launcher; // Handle to the application's start function.
        std::unique_ptr<gui::SwitchData> switchData;
        std::string switchWindow;
        bool blockClosing = false; //< Informs the application manager that this application mustn't be closed
                                   // temporarily. This flag is used to prevent application closing when application
                                   // is closeable and there is incoming call. This flag is also used when closeable
                                   // application is on front and there is a timeout to block the application.

        StartupReason startupReason = StartupReason::Launch; // Informs application about startup reason.

      private:
        auto getManifest() const -> const ApplicationManifest &;
    };
} // namespace app::manager
