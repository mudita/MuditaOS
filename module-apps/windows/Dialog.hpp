#pragma once

#include "AppWindow.hpp"
#include <Text.hpp>
#include <functional>

namespace gui
{
    class Dialog : public AppWindow
    {
      protected:
        Text *text  = nullptr;
        Image *icon = nullptr;

      public:
        struct Meta
        {
            std::string title            = "No title";
            UTF8 icon                    = "";
            UTF8 text                    = "No text";
            std::function<bool()> action = []() -> bool { return false; };

            Meta() = default;
        } meta;

        Dialog(app::Application *app, const std::string &name, const Meta &meta);
        ~Dialog() override = default;

        virtual void update(const Meta &meta);
    };

    class DialogConfirm : public Dialog
    {
      public:
        DialogConfirm(app::Application *app, const std::string &name, const Dialog::Meta &meta = Dialog::Meta());
        ~DialogConfirm() override = default;

        virtual void update(const Meta &meta) override;
    };

    class DialogYesNo : public Dialog
    {
      protected:
        Label *yes = nullptr, *no = nullptr;

      public:
        DialogYesNo(app::Application *app, const std::string &name, const Meta &meta = Dialog::Meta());
        ~DialogYesNo() override = default;

        void update(const Meta &meta);
    };

}; // namespace gui