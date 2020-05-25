#pragma once

#include "AppWindow.hpp"
#include <Text.hpp>
#include <functional>

namespace gui
{
    class Dialog : public AppWindow
    {

        Label *yes = nullptr, *no = nullptr;
        Text *text  = nullptr;
        Image *icon = nullptr;

      public:
        enum class Options
        {
            haveChoice, // yes / no
            onlyOk,
            onlyBack
        };

        struct Meta
        {
            std::string title = "";
            UTF8 icon;
            UTF8 text                    = "";
            std::function<bool()> action = []() -> bool { return false; };

            Options options = Options::haveChoice;
        } meta;

        Dialog(app::Application *app, const std::string &name, const Meta &meta);
        ~Dialog() override = default;

        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void update(const Meta &meta);

      private:
        /// build yes / no elements and set focus on no
        void setupChoice();
    };

}; // namespace gui
