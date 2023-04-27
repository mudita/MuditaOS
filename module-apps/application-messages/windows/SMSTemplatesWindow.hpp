// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SMSdata.hpp"
#include "SMSTemplateModel.hpp"

#include <AppWindow.hpp>
#include <Constants.hpp>

#include <Icon.hpp>
#include <gui/widgets/ListView.hpp>

namespace gui
{
    class SMSTemplatesWindow : public AppWindow, public InfoAboutPreviousAppWhereWeComeFrom
    {
        std::shared_ptr<SMSTemplateModel> smsTemplateModel;
        gui::ListView *list      = nullptr;
        gui::Icon *emptyListIcon = nullptr;

        void smsSendTemplateRequestHandler(const SMSSendTemplateRequest *const switchData);
        void smsTemplateRequestHandler(const SMSTemplateRequest *const switchData);

      public:
        SMSTemplatesWindow() = delete;
        explicit SMSTemplatesWindow(app::ApplicationCommon *app,
                                    const std::string &windowName = name::window::sms_templates);
        virtual ~SMSTemplatesWindow();

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        bool onDatabaseMessage(sys::Message *msgl) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
