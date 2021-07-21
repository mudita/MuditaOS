// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        ApnInputWidget(settingsInternals::ListItemName listItemName,
                       std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                       std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr,
                       std::function<void()> selectSpecialCharacter                 = nullptr,
                       std::function<void(const std::string &text)> contentChanged  = nullptr,
                       unsigned int lines                                           = 1);

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
