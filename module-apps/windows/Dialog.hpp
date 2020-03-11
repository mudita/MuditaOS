#pragma once

#include "AppWindow.hpp"
#include <Text.hpp>
#include <functional>

namespace gui
{
    class Dialog : public AppWindow
    {

        Label *yes = nullptr, *no = nullptr;
        Text *text = nullptr;
        Image *icon = nullptr;

      public:
        struct Meta
        {
            std::string title;
            UTF8 icon;
            UTF8 text;
            std::function<bool()> action;
        } meta;

        Dialog(app::Application *app, const std::string &name, const Meta &meta);
        ~Dialog() override = default;

        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool handleSwitchData(SwitchData *data) override;

        void update(const Meta &meta);
    };

}; // namespace gui
