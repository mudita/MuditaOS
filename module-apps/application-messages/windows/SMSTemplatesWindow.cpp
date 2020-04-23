#include "SMSTemplatesWindow.hpp"

#include "application-messages/ApplicationMessages.hpp"
#include "application-messages/widgets/SMSTemplateItem.hpp"
#include "application-messages/MessagesStyle.hpp"

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
        : AppWindow(app, name::window::sms_templates), smsTemplateModel{new SMSTemplateModel(app)}
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
        list->setMaxElements(list::pageSize);
        list->setPageSize(list::pageSize);
        list->setProvider(smsTemplateModel);

        setFocusItem(list);
    }
    void SMSTemplatesWindow::destroyInterface()
    {
        AppWindow::destroyInterface();

        removeWidget(list);
        delete list;
        list = nullptr;

        children.clear();
        delete smsTemplateModel;
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
