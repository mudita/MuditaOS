#include "SMSTemplatesWindow.hpp"

#include "application-messages/ApplicationMessages.hpp"
#include "application-messages/widgets/SMSTemplateItem.hpp"
#include "application-messages/MessagesStyle.hpp"

#include <service-appmgr/ApplicationManager.hpp>
#include <service-db/messages/DBSMSTemplateMessage.hpp>
#include <i18/i18.hpp>
#include <Style.hpp>
#include <log/log.hpp>

#include <cassert>
#include <functional>
#include <memory>

namespace gui
{
    SMSTemplatesWindow::SMSTemplatesWindow(app::Application *app)
        : AppWindow(app, name::window::sms_templates), smsTemplateModel{std::make_unique<SMSTemplateModel>(app)}
    {
        buildInterface();
    }

    SMSTemplatesWindow::~SMSTemplatesWindow()
    {
        destroyInterface();
    }

    void SMSTemplatesWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void SMSTemplatesWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::localize.get("app_messages_templates"));

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::use));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        topBar->setActive(TopBar::Elements::TIME, true);

        namespace style = style::messages::templates::list;

        list = new gui::ListView(this, style::x, style::y, style::w, style::h);
        list->setProvider(smsTemplateModel.get());

        setFocusItem(list);
    }

    void SMSTemplatesWindow::destroyInterface()
    {
        erase();
    }

    void SMSTemplatesWindow::smsTemplateRequestHandler(const SMSTemplateRequest *const switchData)
    {
        auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app != nullptr);

        auto requestingWindow  = switchData->requestingWindow;
        app->templatesCallback = [=](std::shared_ptr<SMSTemplateRecord> templ) {
            LOG_DEBUG("SMS template id = %" PRIu32 "chosen", templ->ID);
            std::unique_ptr<gui::SwitchData> data = std::make_unique<SMSTextData>(templ->text);
            application->switchWindow(requestingWindow, std::move(data));
            return true;
        };
    }

    void SMSTemplatesWindow::smsSendTemplateRequestHandler(const SMSSendTemplateRequest *const switchData)
    {
        auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app != nullptr);

        auto phoneNumber       = switchData->getPhoneNumber().getEntered();
        app->templatesCallback = [=](std::shared_ptr<SMSTemplateRecord> templ) {
            LOG_DEBUG("SMS template id = %" PRIu32 "sent to %s", templ->ID, phoneNumber.c_str());
            app->sendSms(phoneNumber, templ->text);
            sapm::ApplicationManager::messageSwitchPreviousApplication(
                app,
                std::make_unique<sapm::APMSwitchPrevApp>(application->GetName(), std::make_unique<SMSTemplateSent>()));
            return true;
        };
    }

    void SMSTemplatesWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_INIT) {
            smsTemplateModel->clear();
            smsTemplateModel->requestRecordsCount();
            list->clear();
            list->setElementsCount(smsTemplateModel->getItemCount());
        }

        if (auto switchData = dynamic_cast<SMSTemplateRequest *>(data)) {
            smsTemplateRequestHandler(switchData);
        }

        if (auto switchData = dynamic_cast<SMSSendTemplateRequest *>(data); switchData != nullptr) {
            smsSendTemplateRequestHandler(switchData);
        }
    }

    bool SMSTemplatesWindow::onDatabaseMessage(sys::Message *msgl)
    {
        auto msg = dynamic_cast<DBSMSTemplateResponseMessage *>(msgl);
        if (msg && smsTemplateModel->updateRecords(std::move(msg->records), msg->offset, msg->limit, msg->count)) {
            return true;
        }

        return false;
    }

} /* namespace gui */
