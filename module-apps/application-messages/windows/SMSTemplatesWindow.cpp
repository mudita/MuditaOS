// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"
#include "MessagesStyle.hpp"
#include "SMSTemplateItem.hpp"
#include "SMSTemplatesWindow.hpp"

#include <service-db/DBNotificationMessage.hpp>
#include <i18n/i18n.hpp>
#include <log/log.hpp>
#include <service-appmgr/Controller.hpp>
#include <Style.hpp>

#include <cassert>
#include <memory>

namespace gui
{
    SMSTemplatesWindow::SMSTemplatesWindow(app::ApplicationCommon *app, const std::string &windowName)
        : AppWindow(app, windowName), smsTemplateModel{std::make_shared<SMSTemplateModel>(app)}
    {
        buildInterface();
    }

    SMSTemplatesWindow::~SMSTemplatesWindow()
    {
        destroyInterface();
    }

    void SMSTemplatesWindow::rebuild()
    {
        if (list == nullptr) {
            return;
        }
        list->rebuildList(gui::listview::RebuildType::InPlace);
    }

    void SMSTemplatesWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_messages_templates"));

        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::use));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
        navBar->setActive(nav_bar::Side::Right, true);

        namespace style = style::messages::templates::list;

        list = new gui::ListView(
            this, style::x, style::y, style::w, style::h, smsTemplateModel, listview::ScrollBarType::Fixed);
        list->setBoundaries(Boundaries::Continuous);

        using namespace ::style;
        emptyListIcon = new Icon(this,
                                 0,
                                 window::default_vertical_pos,
                                 window_width,
                                 window_height - window::default_vertical_pos - ::style::nav_bar::height,
                                 "info_128px_W_G",
                                 utils::translate("app_messages_no_templates"));
        emptyListIcon->setVisible(false);

        list->setVisible(true);

        list->emptyListCallback = [this]() {
            emptyListIcon->setVisible(true);
            navBar->setActive(nav_bar::Side::Center, false);
            application->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
        };

        list->notEmptyListCallback = [this]() {
            emptyListIcon->setVisible(false);
            navBar->setActive(nav_bar::Side::Center, true);
            application->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
        };
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
            LOG_DEBUG("SMS template id = %" PRIu32 " chosen", templ->ID);
            auto data = std::make_unique<SMSTextData>(templ->text, SMSTextData::Concatenate::True);
            application->switchWindow(requestingWindow, std::move(data));
            return true;
        };
    }

    void SMSTemplatesWindow::smsSendTemplateRequestHandler(const SMSSendTemplateRequest *const switchData)
    {
        preventsAutoLock = (switchData->getAutolockBehavior() == SMSSendTemplateRequest::AutolockBehavior::Prevent);

        auto app         = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app != nullptr);

        app->templatesCallback = [=](std::shared_ptr<SMSTemplateRecord> templ) {
            LOG_DEBUG("SMS template id = %" PRIu32 " sent", templ->ID);
            app::manager::Controller::switchBack(
                app,
                std::make_unique<app::manager::SwitchBackRequest>(application->GetName(),
                                                                  std::make_unique<SMSTemplateSent>(templ->text)));
            app->popCurrentWindow();
            return true;
        };
    }

    void SMSTemplatesWindow::onBeforeShow([[maybe_unused]] ShowMode mode, SwitchData *data)
    {
        if (auto switchData = dynamic_cast<SMSTemplateRequest *>(data)) {
            smsTemplateRequestHandler(switchData);
        }

        if (auto switchData = dynamic_cast<SMSSendTemplateRequest *>(data); switchData != nullptr) {
            saveInfoAboutPreviousAppForProperSwitchBack(data);
            smsSendTemplateRequestHandler(switchData);
        }

        list->rebuildList(listview::RebuildType::InPlace);
    }

    bool SMSTemplatesWindow::onInput(const InputEvent &inputEvent)
    {
        if (!inputEvent.isShortRelease(KeyCode::KEY_RF) || !shouldCurrentAppBeIgnoredOnSwitchBack()) {
            return AppWindow::onInput(inputEvent);
        }

        return app::manager::Controller::switchBack(
            application,
            std::make_unique<app::manager::SwitchBackRequest>(nameOfPreviousApplication.value(), nullptr, true));
    }

    bool SMSTemplatesWindow::onDatabaseMessage(sys::Message *msgl)
    {
        const auto msgNotification = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msgNotification != nullptr && msgNotification->interface == db::Interface::Name::SMSTemplate &&
            msgNotification->dataModified()) {
            rebuild();
            return true;
        }
        return false;
    }
} /* namespace gui */
