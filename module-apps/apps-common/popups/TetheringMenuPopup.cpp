// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TetheringMenuPopup.hpp"
#include "DialogMetadataMessage.hpp"
#include "ApplicationCommon.hpp"

namespace gui
{
    TetheringMenuPopup::TetheringMenuPopup(app::ApplicationCommon *app, const std::string &name)
        : TetheringNotificationPopup{app, name}
    {}

    void TetheringMenuPopup::onBeforeShow(ShowMode mode, [[maybe_unused]] SwitchData *data)
    {
        DialogMetadata metadata = {.title  = "",
                                   .icon   = "info_128px_W_G",
                                   .text   = utils::translate("tethering_menu_access_decline"),
                                   .action = [this]() {
                                       application->returnToPreviousWindow();
                                       return true;
                                   }};

        const auto msg = std::make_unique<DialogMetadataMessage>(std::move(metadata));
        DialogConfirm::onBeforeShow(mode, msg.get());
    }
} // namespace gui
