// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <functional>

#include <AppWindow.hpp>
#include <Text.hpp>
#include <CalllogRecord.hpp>

namespace gui
{
    class TextWithIconsWidget;

    class CallLogDetailsWindow : public AppWindow
    {
        gui::TextWithIconsWidget *numberHBox = nullptr;
        Text *durationData                   = nullptr;
        Text *dateDay                        = nullptr;
        Text *dateDate                       = nullptr;
        gui::HBox *typeHBox                  = nullptr;

        CalllogRecord record;

        void buildNumberWidget(gui::Item *parent);
        void buildCallDataWidget(gui::Item *parent);
        void buildCallDurationWidget(gui::Item *parent);
        void buildDateWidgets(gui::Item *parent);

        void initNumberWidget();
        void initCallDataWidget();
        void initCallDurationWidget();
        void initDateWidgets();

      public:
        explicit CallLogDetailsWindow(app::ApplicationCommon *app);

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onDatabaseMessage(sys::Message *msgl) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
