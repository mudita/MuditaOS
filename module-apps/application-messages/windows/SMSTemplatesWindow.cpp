#include "SMSTemplatesWindow.hpp"

#include "application-messages/ApplicationMessages.hpp"
#include "application-messages/widgets/SMSTemplateItem.hpp"
#include "application-messages/MessagesStyle.hpp"
#include "application-messages/data/SMSdata.hpp"

#include <service-appmgr/ApplicationManager.hpp>
#include <service-db/messages/DBMessage.hpp>
#include <i18/i18.hpp>
#include <Style.hpp>

#include <cassert>
#include <functional>
#include <memory>

using namespace style;
using namespace messages::templates;

namespace gui
{
    SMSTemplatesWindow::SMSTemplatesWindow(app::Application *app)
        : AppWindow(app, name::window::sms_templates), smsTemplateModel{std::make_unique<SMSTemplateModel>(app)}
    {
        buildInterface();
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

        list = new gui::ListView(this, list::x, list::y, list::w, list::h);
        list->setProvider(smsTemplateModel.get());

        setFocusItem(list);

        if (auto app = dynamic_cast<app::ApplicationMessages *>(application)) {
            app->templatesCallback = [=](std::shared_ptr<SMSTemplateRecord> templ) {
                std::unique_ptr<gui::SwitchData> data = std::make_unique<SMSTemplateData>(templ);
                application->switchWindow(requestingWindow, std::move(data));
                return true;
            };
        }
    }

    void SMSTemplatesWindow::destroyInterface()
    {
        erase();
    }

    SMSTemplatesWindow::~SMSTemplatesWindow()
    {
        destroyInterface();
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
            requestingWindow = switchData->requestingWindow;
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
