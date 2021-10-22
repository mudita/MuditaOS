// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AsyncTask.hpp>
#include <apps-common/ApplicationCommon.hpp>

#include <functional>

namespace dark_mode
{
    class DarkModeModel : public app::AsyncCallbackReceiver
    {
      public:
        explicit DarkModeModel(app::ApplicationCommon *application);

        /**
         * Enables the dark mode
         */
        void enable(std::function<void()> &&onSuccess);

        /**
         * Disables the dark mode
         */
        void disable(std::function<void()> &&onSuccess);

        /**
         * @return true if dark mode is enabled, false otherwise.
         */
        bool isEnabled() const noexcept;

      private:
        void switchMode(bool enableDarkMode, std::function<void()> &&onSuccess);

        app::ApplicationCommon *application{nullptr};
        bool enabled{false};
    };
} // namespace dark_mode
