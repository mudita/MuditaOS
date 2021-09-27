// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Text.hpp>
#include <common/options/BellShortOptionWindow.hpp>

namespace gui
{
    namespace bell_dialog
    {
        enum Options
        {
            YES,
            NO
        };
    } // namespace bell_dialog

    /// @brief Yes/No Dialog class
    ///
    /// Contain same items as Dialog and additionally Yes and No selectable options
    class BellDialogYesNo : public BellShortOptionWindow
    {
      public:
        BellDialogYesNo(app::ApplicationCommon *app, const std::string &name);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        std::list<gui::Option> dialogOptionsList(std::function<bool()> yesAction);
    };
}; // namespace gui
