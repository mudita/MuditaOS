// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApnListItem.hpp"

#include <application-settings/data/SettingsInternals.hpp>

#include <TextFixedSize.hpp>

namespace gui
{
    class ApnInputWidget : public ApnListItem
    {
        settingsInternals::ListItemName listItemName;

      public:
        explicit ApnInputWidget(
            settingsInternals::ListItemName listItemName,
            const std::function<void(const UTF8 &)> &navBarTemporaryMode                       = nullptr,
            const std::function<void()> &navBarRestoreFromTemporaryMode                        = nullptr,
            const std::function<void()> &selectSpecialCharacter                                = nullptr,
            const std::function<void(std::function<void()> restoreFunction)> &restoreInputMode = nullptr,
            const std::function<void(const std::string &text)> &contentChanged                 = nullptr,
            unsigned int lines                                                                 = 1);

      private:
        VBox *vBox                                                    = nullptr;
        Label *titleLabel                                             = nullptr;
        TextFixedSize *inputText                                      = nullptr;
        std::function<void(const std::string &text)> checkTextContent = nullptr;

        void applyItemNameSpecificSettings();
        void nameHandler();
        void apnHandler();
        void usernameHandler();
        void passwordNumberHandler();
        void authtypeHandler();
        void apntypeHandler();
        void protocolHandler();
    };

} /* namespace gui */
