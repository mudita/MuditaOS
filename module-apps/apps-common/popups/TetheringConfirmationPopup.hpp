// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include "Dialog.hpp"
namespace gui::window::name
{
    inline constexpr auto dialog_yes_no = "DialogYesNo";
} // namespace gui::window::name

namespace gui
{
    class TetheringConfirmationPopup : public DialogYesNo
    {
      public:
        TetheringConfirmationPopup(app::ApplicationCommon *app, const std::string &name);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        void showDialogUnsavedChanges(std::function<bool()> whatToDoOnYes);
    };
}; // namespace gui
