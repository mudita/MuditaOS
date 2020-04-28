/*
 * @file CallLogMainWindow.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 19.09.2019
 * @brief Application Call Log Main Window
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "CallLogMainWindow.hpp"
#include <cassert>
#include <functional>
#include <memory>

#include "application-call/ApplicationCall.hpp"
#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationCallLog.hpp"
#include "../widgets/CalllogItem.hpp"

#include "service-db/messages/DBMessage.hpp"
#include "i18/i18.hpp"

#include "../data/CallLogInternals.hpp" // TODO: alek: add easier paths
#include "Label.hpp"
#include "Margins.hpp"
#include "UiCommonActions.hpp"
#include <Style.hpp>

using namespace style;
using namespace callLogStyle;

namespace gui
{

    CallLogMainWindow::CallLogMainWindow(app::Application *app)
        : AppWindow(app, calllog::settings::MainWindowStr), calllogModel{new CalllogModel(app)}
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

        list = new gui::ListView(this, mainWindow::x, mainWindow::y, mainWindow::w, mainWindow::h);
        list->setProvider(calllogModel);

        setFocusItem(list);
    }
    void CallLogMainWindow::destroyInterface()
    {
        AppWindow::destroyInterface();

        if (list) {
            removeWidget(list);
            delete list;
            list = nullptr;
        };

        children.clear();
        delete calllogModel;
    }

    CallLogMainWindow::~CallLogMainWindow()
    {
        destroyInterface();
    }

    void CallLogMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_INIT) {
            calllogModel->clear();
            calllogModel->requestRecordsCount();
            list->clear();
            list->setElementsCount(calllogModel->getItemCount());
        }
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
