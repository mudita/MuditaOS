#include "CallLogMainWindow.hpp"
#include "application-calllog/data/CallLogInternals.hpp"
#include "application-calllog/ApplicationCallLog.hpp"
#include "application-calllog/widgets/CalllogItem.hpp"

#include <application-call/ApplicationCall.hpp>
#include <service-appmgr/ApplicationManager.hpp>
#include <service-db/messages/DBCalllogMessage.hpp>
#include <i18/i18.hpp>
#include <Label.hpp>
#include <Margins.hpp>
#include <UiCommonActions.hpp>
#include <Style.hpp>

#include <cassert>
#include <functional>
#include <memory>

using namespace style;
using namespace callLogStyle;

namespace gui
{

    CallLogMainWindow::CallLogMainWindow(app::Application *app)
        : AppWindow(app, calllog::settings::MainWindowStr), calllogModel{std::make_shared<CalllogModel>(app)}
    {

        buildInterface();
    }

    void CallLogMainWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void CallLogMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::localize.get("app_calllog_title_main"));

        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(style::strings::common::call));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        topBar->setActive(TopBar::Elements::TIME, true);

        list = new gui::ListView(this, mainWindow::x, mainWindow::y, mainWindow::w, mainWindow::h, calllogModel);

        setFocusItem(list);
    }

    void CallLogMainWindow::destroyInterface()
    {
        erase();
    }

    void CallLogMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_INIT) {
            calllogModel->clear();
            calllogModel->requestRecordsCount();
            list->clear();
            list->setElementsCount(calllogModel->getItemCount());
        }
        auto app = dynamic_cast<app::ApplicationCallLog *>(application);
        assert(app != nullptr);
        app->setAllEntriesRead();
    }

    bool CallLogMainWindow::onDatabaseMessage(sys::Message *msgl)
    {
        DBCalllogResponseMessage *msg = reinterpret_cast<DBCalllogResponseMessage *>(msgl);
        if (calllogModel->updateRecords(std::move(msg->records), msg->offset, msg->limit, msg->count)) {
            return true;
        }

        return false;
    }

} /* namespace gui */
