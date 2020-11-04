// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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
        static inline constexpr std::string_view InvalidAppName{"NONE"};
        static inline const ApplicationManifest InvalidManifest;

        using State = app::Application::State;

        explicit ApplicationHandle(std::unique_ptr<app::ApplicationLauncher> &&_launcher);

        void setState(State state) noexcept;
        void run(sys::Service *caller);
        void runInBackground(sys::Service *caller);

        auto name() const -> ApplicationName;
        auto manifest() const -> const ApplicationManifest &;
        auto state() const noexcept -> State;
        auto preventsBlocking() const noexcept -> bool;
        auto closeable() const noexcept -> bool;
        auto started() const noexcept -> bool;
        auto handles(actions::ActionId action) const noexcept -> bool;

        std::unique_ptr<app::ApplicationLauncher> launcher; // Handle to the application's start function.
        std::unique_ptr<gui::SwitchData> switchData;
        std::string switchWindow;
        bool blockClosing =
            false; //< Informs the application manager that this application mustn't be closed temporarily.
        //< This flag is used to prevent application closing when application is closeable and there is
        //< incoming call. This flag is also used when closeable application is on front and there is a
        //< timeout to block the application.
    };
} // namespace app::manager
