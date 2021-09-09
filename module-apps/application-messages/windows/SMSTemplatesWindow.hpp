// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SMSdata.hpp"
#include "SMSTemplateModel.hpp"

#include <AppWindow.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/ListView.hpp>

#include <functional>
#include <string>

namespace gui
{
    class SMSTemplatesWindow : public AppWindow
    {
        std::shared_ptr<SMSTemplateModel> smsTemplateModel;
        gui::ListView *list = nullptr;

        void smsSendTemplateRequestHandler(const SMSSendTemplateRequest *const switchData);
        void smsTemplateRequestHandler(const SMSTemplateRequest *const switchData);

      public:
        SMSTemplatesWindow() = delete;
        SMSTemplatesWindow(app::ApplicationCommon *app);
        virtual ~SMSTemplatesWindow();

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
