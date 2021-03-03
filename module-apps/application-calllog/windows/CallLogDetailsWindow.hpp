// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <functional>

#include "AppWindow.hpp"
#include "gui/widgets/Text.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/ListView.hpp"

#include "../data/CallLogInternals.hpp"

namespace gui
{

    class CallLogDetailsWindow : public AppWindow
    {

        enum FocusRects
        {
            Call,
            Sms,
            NumOfRects
        };

        Label *informationLabel                                                                     = nullptr;
        Label *number                                                                               = nullptr;
        gui::Rect *rects[static_cast<uint32_t>(FocusRects::NumOfRects)]                             = {nullptr};
        Image *callImg                                                                              = nullptr;
        Image *smsImg                                                                               = nullptr;
        gui::Image *callTypeImg[static_cast<uint32_t>(calllog::CallLogCallType::NUM_OF_CALL_TYPES)] = {
            nullptr, nullptr, nullptr};
        Label *typeLabel     = nullptr;
        Label *durationLabel = nullptr;
        Label *typeData      = nullptr;
        Label *durationData  = nullptr;
        Label *dateLabel     = nullptr;
        Label *dateDay       = nullptr;
        Label *dateDate      = nullptr;

        CalllogRecord record;

        Label *decorateLabel(Label *);
        Label *decorateData(Label *);

      public:
        CallLogDetailsWindow(app::Application *app);
        virtual ~CallLogDetailsWindow();

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
