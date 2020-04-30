#pragma once

#include "application-messages/widgets/SMSTemplateModel.hpp"

#include <AppWindow.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/ListView.hpp>

#include <string>
#include <functional>

namespace gui
{
    class SMSTemplatesWindow : public AppWindow
    {
        std::unique_ptr<SMSTemplateModel> smsTemplateModel;
        gui::ListView *list = nullptr;
        std::string requestingWindow;

      public:
        SMSTemplatesWindow() = delete;
        SMSTemplatesWindow(app::Application *app);
        virtual ~SMSTemplatesWindow();

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onDatabaseMessage(sys::Message *msg) override;
    };

} /* namespace gui */
