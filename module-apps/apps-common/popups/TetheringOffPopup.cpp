// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TetheringOffPopup.hpp"

#include <log.hpp>
#include <messages/DialogMetadataMessage.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <SystemManager/messages/TetheringStateRequest.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    TetheringOffPopup::TetheringOffPopup(app::ApplicationCommon *app, const std::string &name) : DialogYesNo{app, name}
    {
        statusBar->configure(configureStatusBar(application->getStatusBarConfiguration()));
    }

    status_bar::Configuration TetheringOffPopup::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.enable(status_bar::Indicator::Time);
        appConfiguration.disable(status_bar::Indicator::Lock);
        appConfiguration.disable(status_bar::Indicator::Battery);
        appConfiguration.disable(status_bar::Indicator::Signal);
        appConfiguration.disable(status_bar::Indicator::SimCard);
        return appConfiguration;
    }

    void TetheringOffPopup::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        DialogMetadata metadata;
        metadata.action = [this]() {
            application->bus.sendUnicast(std::make_shared<sys::TetheringStateRequest>(sys::phone_modes::Tethering::Off),
                                         service::name::system_manager);
            app::manager::Controller::sendAction(application, app::manager::actions::Home);
            return true;
        };
        metadata.title = utils::translate("tethering");
        metadata.text  = utils::translate("tethering_turn_off_question");
        metadata.icon  = "tethering_icon";
        auto msg       = std::make_unique<DialogMetadataMessage>(std::move(metadata));
        DialogYesNo::onBeforeShow(mode, msg.get());
    }

} /* namespace gui */
