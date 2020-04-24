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
        SMSTemplateModel *smsTemplateModel = nullptr;
        gui::ListView *list                = nullptr;
        std::string requestingWindow;

      public:
        SMSTemplatesWindow(app::Application *app);
        virtual ~SMSTemplatesWindow();

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onDatabaseMessage(sys::Message *msg) override;
    };

} /* namespace gui */
