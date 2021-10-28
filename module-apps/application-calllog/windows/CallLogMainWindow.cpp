// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallLogMainWindow.hpp"
#include "data/CallLogInternals.hpp"
#include "ApplicationCallLog.hpp"
#include "widgets/CalllogItem.hpp"

#include <service-db/DBCalllogMessage.hpp>
#include <i18n/i18n.hpp>
#include <Label.hpp>
#include <Margins.hpp>
#include <Style.hpp>
#include <InputEvent.hpp>

#include <cassert>
#include <functional>
#include <memory>
#include <service-db/DBNotificationMessage.hpp>

using namespace style;
using namespace callLogStyle;

namespace gui
{

    CallLogMainWindow::CallLogMainWindow(app::ApplicationCommon *app)
        : AppWindow(app, calllog::settings::MainWindowStr), calllogModel{std::make_shared<CalllogModel>(app)}
    {

        buildInterface();
    }

    void CallLogMainWindow::rebuild()
    {
        if (list == nullptr) {
            return;
        }
        list->rebuildList(gui::listview::RebuildType::InPlace);
    }

    void CallLogMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_calllog_title_main"));

        navBar->setText(nav_bar::Side::Left, utils::translate(style::strings::common::call));
        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::open));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        list = new gui::ListView(this,
                                 mainWindow::x,
                                 mainWindow::y,
                                 mainWindow::w,
                                 mainWindow::h,
                                 calllogModel,
                                 gui::listview::ScrollBarType::Fixed);

        setFocusItem(list);

        buildInterfaceForEmptyState();

        list->emptyListCallback    = [this]() { onEmptyList(); };
        list->notEmptyListCallback = [this]() { onListFilled(); };
    }

    void CallLogMainWindow::buildInterfaceForEmptyState()
    {
        namespace MyStyle = callLogStyle::detailsWindow::noCalls;
        emptyLayout       = new VBox(this, 0, 0, style::window_width, style::window_height);
        emptyLayout->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));

        Text *noCallsInfo = new Text(emptyLayout, 0, 0, 0, 0);
        noCallsInfo->setMinimumSize(mainWindow::w, MyStyle::infoHeight);
        noCallsInfo->setTextType(TextType::SingleLine);
        noCallsInfo->setEditMode(EditMode::Browse);
        noCallsInfo->setEdges(RectangleEdge::None);
        noCallsInfo->setFont(style::window::font::medium);
        noCallsInfo->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        noCallsInfo->setRichText(utils::translate("app_calllog_no_calls"));
        noCallsInfo->setMargins(gui::Margins(0, MyStyle::infoTopMargin, 0, 0));

        Rect *divLine = new Rect(emptyLayout, 0, 0, style::window_width, 1);
        divLine->setBorderColor(ColorGrey);
        divLine->setEdges(RectangleEdge::Top);
        divLine->setMargins(gui::Margins(0, MyStyle::divLineTopMargin, 0, 0));

        HBox *noCallsBottom = new HBox(emptyLayout);
        noCallsBottom->setMinimumSize(MyStyle::bottomBoxWidth, MyStyle::bottomBoxHeight);
        noCallsBottom->setEdges(RectangleEdge::None);
        noCallsBottom->setMargins(gui::Margins(0, MyStyle::bottomBoxMargin, 0, 0));
        noCallsBottom->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));

        ImageBox *noCallsImg = new ImageBox(noCallsBottom, new Image("callog_empty_W_G"));
        noCallsImg->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        noCallsImg->setMinimumSizeToFitImage();
        noCallsImg->setMargins(Margins(MyStyle::infoIconMargin, 0, 0, 0));

        VThreeBox<VBox, VBox, VBox> *noCallsIcons = new VThreeBox<VBox, VBox, VBox>(noCallsBottom);
        noCallsIcons->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        noCallsIcons->setEdges(RectangleEdge::None);
        noCallsIcons->setMinimumSize(MyStyle::descriptionSize, MyStyle::bottomBoxHeight);
        noCallsIcons->setMargins(Margins(MyStyle::descriptionLeftMargin, 0, 0, 0));
        auto imgLambda = [&](const UTF8 imageName) {
            VBox *box = new VBox(noCallsIcons);
            box->setEdges(RectangleEdge::None);
            box->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
            box->setMinimumSize(MyStyle::descriptionSize, MyStyle::descriptionSize);
            box->setMargins(Margins(0, MyStyle::descriptionInternalMargin, 0, MyStyle::descriptionInternalMargin));

            ImageBox *image = new ImageBox(box, new Image(imageName, gui::ImageTypeSpecifier::W_G));
            image->setMinimumSizeToFitImage();
            return box;
        };
        noCallsIcons->firstBox  = imgLambda("calllog_arrow_in");
        noCallsIcons->centerBox = imgLambda("calllog_arrow_out");
        noCallsIcons->lastBox   = imgLambda("calllog_arrow_den");

        VThreeBox<VBox, VBox, VBox> *noCallsDescriptions = new VThreeBox<VBox, VBox, VBox>(noCallsBottom);
        noCallsDescriptions->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        noCallsDescriptions->setEdges(RectangleEdge::None);
        noCallsDescriptions->setMinimumSize(MyStyle::descriptionTextWidth, MyStyle::bottomBoxHeight);
        auto descLambda = [&](const UTF8 descText) {
            VBox *box = new VBox(noCallsDescriptions);
            box->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
            box->setEdges(RectangleEdge::None);
            box->setMinimumSize(MyStyle::descriptionTextWidth, MyStyle::descriptionSize);
            box->setMargins(Margins(0, MyStyle::descriptionInternalMargin, 0, MyStyle::descriptionInternalMargin));

            Text *desc = new Text(box, 0, 0, 0, 0);
            desc->setMaximumSize(MyStyle::descriptionTextWidth, MyStyle::descriptionSize);
            desc->setEditMode(EditMode::Browse);
            desc->setEdges(RectangleEdge::None);
            desc->setFont(style::window::font::small);
            desc->setRichText(descText);

            return box;
        };
        noCallsDescriptions->firstBox  = descLambda(utils::translate("app_calllog_empty_incoming"));
        noCallsDescriptions->centerBox = descLambda(utils::translate("app_calllog_empty_outgoing"));
        noCallsDescriptions->lastBox   = descLambda(utils::translate("app_calllog_empty_missed"));

        emptyLayout->setVisible(false);
        emptyLayout->resizeItems();
    }

    void CallLogMainWindow::destroyInterface()
    {
        erase();
    }

    void CallLogMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_INIT) {
            list->rebuildList();
        }
        auto app = dynamic_cast<app::ApplicationCallLog *>(application);
        assert(app != nullptr);
        app->setAllEntriesRead();
    }

    bool CallLogMainWindow::onDatabaseMessage(sys::Message *msg)
    {
        auto notification = dynamic_cast<db::NotificationMessage *>(msg);
        if (notification != nullptr) {
            if (notification->interface == db::Interface::Name::Calllog && notification->dataModified()) {
                rebuild();
                return true;
            }
        }
        return false;
    }

    void CallLogMainWindow::onEmptyList()
    {
        navBar->setActive(gui::nav_bar::Side::Left, false);
        navBar->setActive(gui::nav_bar::Side::Center, false);
        emptyLayout->setVisible(true);
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
    }

    void CallLogMainWindow::onListFilled()
    {
        navBar->setActive(gui::nav_bar::Side::Left, true);
        navBar->setActive(gui::nav_bar::Side::Center, true);
        emptyLayout->setVisible(false);
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
    }
} /* namespace gui */
