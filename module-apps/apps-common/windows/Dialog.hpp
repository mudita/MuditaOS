// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include <Text.hpp>
#include <functional>
#include <DialogMetadata.hpp>
#include <module-gui/gui/widgets/Icon.hpp>

namespace gui::window::name
{
    inline constexpr auto dialog_confirm = "DialogConfirm";
    inline constexpr auto dialog_retry   = "DialogRetry";
}; // namespace gui::window::name

namespace gui
{
    namespace dialog
    {
        enum Option
        {
            YES,
            NO
        };

        namespace style
        {
            namespace option
            {
                inline constexpr auto w         = 150;
                inline constexpr auto h         = 75;
                inline constexpr auto margin    = 30;
                inline constexpr auto iconTextH = 99;
            } // namespace option

            namespace iconTextLabel
            {
                inline constexpr auto h = 188;
            } // namespace iconTextLabel

        } // namespace style
    }     // namespace dialog

    /// @brief base Dialog class
    ///
    /// Contain icon, text and Back action
    class Dialog : public AppWindow
    {
      protected:
        Icon *icon = nullptr;

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
        Label *yes = nullptr;
        Label *no  = nullptr;
        HBox *hBox = nullptr;

      public:
        DialogYesNo(app::Application *app, const std::string &name);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        Label *createYesNoOption(Item *parent, const gui::dialog::Option &optionName);
    };

    /// @brief Yes/No Icon Text  Dialog class
    ///
    /// Contain same items as DialogYesNo and additionally puts text on icon
    class DialogYesNoIconTxt : public DialogYesNo
    {
      protected:
        Label *iconText = nullptr;

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
