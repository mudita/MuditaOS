// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <module-apps/application-messages/models/SMSThreadModel.hpp>

#include <ListView.hpp>
#include <PhoneNumber.hpp>
#include <service-db/api/DBServiceAPI.hpp>

#include <functional>
#include <string>

namespace gui
{
    class SMSThreadViewWindow : public AppWindow
    {
      private:
        std::shared_ptr<SMSThreadModel> smsModel = nullptr;
        gui::ListView *smsList                   = nullptr;

        std::shared_ptr<ContactRecord> contact;

        inline static const std::uint32_t numberIdTimeout = 1000;

      public:
        SMSThreadViewWindow(app::Application *app);

        virtual ~SMSThreadViewWindow();
        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose() override;

        bool onDatabaseMessage(sys::Message *msgl) override;
        void rebuild() override;
        void buildInterface() override;

        void destroyInterface() override;
    };

} /* namespace gui */
