// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TetheringConfirmationPopup.hpp"
#include "DialogMetadataMessage.hpp"
#include "ApplicationCommon.hpp"

#include <SystemManager/messages/TetheringQuestionRequest.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    TetheringConfirmationPopup::TetheringConfirmationPopup(app::ApplicationCommon *app, const std::string &name)
        : DialogYesNo{app, name}
    {}

    void TetheringConfirmationPopup::onBeforeShow(ShowMode mode, [[maybe_unused]] SwitchData *data)
    {
        DialogMetadata metadata;
        metadata.title  = utils::translate("tethering");
        metadata.text   = utils::translate("tethering_enable_question");
        metadata.icon   = "tethering_icon";
        metadata.action = [this]() {
            application->bus.sendUnicast(std::make_shared<sys::TetheringEnabledResponse>(),
                                         service::name::system_manager);
            app::manager::Controller::sendAction(application, app::manager::actions::Home);
            return true;
        };
        auto msg = std::make_unique<DialogMetadataMessage>(std::move(metadata));
        DialogYesNo::onBeforeShow(mode, msg.get());
    }
} // namespace gui
