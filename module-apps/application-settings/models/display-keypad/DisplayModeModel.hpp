// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>

#include <functional>

namespace display_mode
{
    enum class Mode
    {
        Light, ///< Regular display mode
        Dark   ///< Colors inverted display mode
    };

    class DisplayModeModel : public app::AsyncCallbackReceiver
    {
      public:
        explicit DisplayModeModel(app::ApplicationCommon *application);

        /**
         * Sets the display mode.
         * @param mode          Desired display mode
         * @param onSuccess     Callback called on success
         */
        void setMode(Mode mode, std::function<void(Mode)> &&onSuccess);

        [[nodiscard]] auto isDarkModeEnabled() const noexcept -> bool;

      private:
        app::ApplicationCommon *application{nullptr};
    };
} // namespace display_mode
