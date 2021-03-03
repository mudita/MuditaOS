// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include <Text.hpp>
#include <functional>
#include <DialogMetadata.hpp>

namespace gui::window::name
{
    inline constexpr auto dialog_confirm = "DialogConfirm";
    inline constexpr auto dialog_retry   = "DialogRetry";
}; // namespace gui::window::name

namespace gui
{
    class Image;

    /// @brief base Dialog class
    ///
    /// Contain icon, text and Back action
    class Dialog : public AppWindow
    {
      protected:
        Text *text  = nullptr;
        Image *icon = nullptr;

      public:
        Dialog(app::Application *app, const std::string &name);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };

    /// @brief Confirm Dialog class
    ///
    /// Contain same items as Dialog but instead of Back there is OK action
    /// @Note it is also showing signal strength, battery on mode indicators
    class DialogConfirm : public Dialog
    {
      public:
        DialogConfirm(app::Application *app, const std::string &name);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };

    /// @brief Yes/No Dialog class
    ///
    /// Contain same items as Dialog and additionally Yes and No selectable options
    class DialogYesNo : public Dialog
    {
      protected:
        Label *yes = nullptr, *no = nullptr;

      public:
        DialogYesNo(app::Application *app, const std::string &name);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };

    /// @brief Yes/No Icon Text  Dialog class
    ///
    /// Contain same items as DialogYesNo and additionally puts text on icon
    class DialogYesNoIconTxt : public DialogYesNo
    {
      protected:
        Text *iconText = nullptr;

      public:
        DialogYesNoIconTxt(app::Application *app, const std::string &name);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };

    /// @brief Retry Dialog class
    ///
    /// Contain same items as Dialog but instead of OK there is a TRY AGAIN button
    class DialogRetry : public Dialog
    {
      public:
        DialogRetry(app::Application *app, const std::string &name);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };

}; // namespace gui
