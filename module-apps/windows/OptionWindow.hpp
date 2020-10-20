// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "../Application.hpp"
#include "AppWindow.hpp"
#include "PageLayout.hpp"
#include <functional>
#include "OptionsWindowOption.hpp"

namespace gui
{
    class OptionWindow : public AppWindow
    {

      protected:
        PageLayout *body = nullptr;
        std::list<Option> options;
        void addOptions(std::list<Option> &options);
        void addOptions(std::list<Option> &&options);

      public:
        OptionWindow(app::Application *app, const std::string &name);
        OptionWindow(app::Application *app, const std::string &name, std::list<Option> options);
        ~OptionWindow() override;

        void addOptionLabel(const UTF8 &text, std::function<bool(Item &)> activatedCallback, Arrow arrow);
        void setOptions(std::list<Option> options)
        {
            this->options = std::move(options);
        }
        void clearOptions();

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };
}; // namespace gui
