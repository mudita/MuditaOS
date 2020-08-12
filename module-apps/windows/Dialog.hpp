#pragma once

#include "AppWindow.hpp"
#include <Text.hpp>
#include <functional>

namespace gui
{
    /// @brief base Dialog class
    ///
    /// Contain icon, text and Back action
    class Dialog : public AppWindow
    {
      protected:
        Text *text  = nullptr;
        Image *icon = nullptr;

      public:
        struct Meta
        {
            std::string title;
            UTF8 icon                    = "";
            UTF8 text                    = "No text";
            std::function<bool()> action = []() -> bool { return false; };

            Meta() = default;
        } meta;

        Dialog(app::Application *app, const std::string &name, const Meta &meta);
        ~Dialog() override = default;

        virtual void update(const Meta &meta);
    };

    /// @brief Confirm Dialog class
    ///
    /// Contain same items as Dialog but instead of Back there is OK action
    /// @Note it is also showing signal strength, battery on mode indicators
    class DialogConfirm : public Dialog
    {
      public:
        DialogConfirm(app::Application *app, const std::string &name, const Dialog::Meta &meta = Dialog::Meta());
        ~DialogConfirm() override = default;

        virtual void update(const Meta &meta) override;
    };

    /// @brief Yes/No Dialog class
    ///
    /// Contain same items as Dialog and additionally Yes and No selectable options
    class DialogYesNo : public Dialog
    {
      protected:
        Label *yes = nullptr, *no = nullptr;

      public:
        DialogYesNo(app::Application *app, const std::string &name, const Meta &meta = Dialog::Meta());
        ~DialogYesNo() override = default;

        void update(const Meta &meta);
    };

    /// @brief Yes/No Icon Text  Dialog class
    ///
    /// Contain same items as DialogYesNo and additionally puts text on icon
    class DialogYesNoIconTxt : public DialogYesNo
    {
      protected:
        Text *iconText = nullptr;
        UTF8 textStr   = "";

      public:
        DialogYesNoIconTxt(app::Application *app, const std::string &name, const Meta &meta = Dialog::Meta());
        ~DialogYesNoIconTxt() override = default;

        void SetIconText(const std::string &text);
        void update(const Meta &meta);
    };

}; // namespace gui
