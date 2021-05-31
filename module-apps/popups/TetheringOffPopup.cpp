// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TetheringOffPopup.hpp"

#include <log/log.hpp>
#include <messages/DialogMetadataMessage.hpp>
#include <module-apps/Application.hpp>
#include <SystemManager/messages/TetheringStateRequest.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    TetheringOffPopup::TetheringOffPopup(app::Application *app, const std::string &name) : DialogYesNo{app, name}
    {
        topBar->configure(configureTopBar(application->getTopBarConfiguration()));
    }

    top_bar::Configuration TetheringOffPopup::configureTopBar(top_bar::Configuration appConfiguration)
    {
        appConfiguration.enable(top_bar::Indicator::Time);
        appConfiguration.disable(top_bar::Indicator::Lock);
        appConfiguration.disable(top_bar::Indicator::Battery);
        appConfiguration.disable(top_bar::Indicator::Signal);
        appConfiguration.disable(top_bar::Indicator::SimCard);
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
