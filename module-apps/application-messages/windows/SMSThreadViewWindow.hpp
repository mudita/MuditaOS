// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SMSThreadModel.hpp"

#include <AppWindow.hpp>
#include <ListView.hpp>
#include <PhoneNumber.hpp>

#include <functional>
#include <string>

namespace gui
{
    class SMSThreadViewWindow : public AppWindow, public app::AsyncCallbackReceiver
    {
      private:
        std::shared_ptr<SMSThreadModel> smsModel;
        gui::ListView *smsList = nullptr;

        auto requestContact(unsigned int numberID) -> void;
        auto handleContactQueryResponse(db::QueryResult *) -> bool;
        void requestNumber(unsigned int numberID);
        auto handleNumberQueryResponse(db::QueryResult *) -> bool;

      public:
        explicit SMSThreadViewWindow(app::ApplicationCommon *app);
        ~SMSThreadViewWindow() override;

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        bool onDatabaseMessage(sys::Message *msgl) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
