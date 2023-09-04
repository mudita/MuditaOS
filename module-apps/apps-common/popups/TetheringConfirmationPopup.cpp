// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TetheringConfirmationPopup.hpp"
#include "DialogMetadataMessage.hpp"
#include "ApplicationCommon.hpp"

#include <system/messages/TetheringQuestionRequest.hpp>
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
        metadata.icon   = "tethering_128px_W_G";
        metadata.action = [this]() {
            // check if there is any unsaved data in any window in stack
            bool IsDataUnsaved   = false;
            auto windowStackSize = application->getSizeOfWindowsStack();
            for (auto windowOnStackNumber = windowStackSize; windowOnStackNumber > 0; windowOnStackNumber--) {
                if (const auto &previousWindowName = application->getPreviousWindow(windowOnStackNumber);
                    previousWindowName.has_value()) {
                    const auto previousWindow = application->getWindow(previousWindowName.value());
                    IsDataUnsaved             = previousWindow->isAnyUnsavedUserDataInWindow() ? true : IsDataUnsaved;
                }
            }

            // what is the expected action when YES is selected
            auto onYesAction = [this]() {
                application->bus.sendUnicast(std::make_shared<sys::TetheringEnabledResponse>(),
                                             service::name::system_manager);
                app::manager::Controller::sendAction(application, app::manager::actions::Home);
                return true;
            };

            // if there are any unsaved changes, open the appropriate popup, otherwise proceed as usual
            if (IsDataUnsaved) {
                showDialogUnsavedChanges(onYesAction);
                return true;
            }
            onYesAction();
            return true;
        };
        auto msg = std::make_unique<DialogMetadataMessage>(std::move(metadata));
        DialogYesNo::onBeforeShow(mode, msg.get());
    }

    void TetheringConfirmationPopup::showDialogUnsavedChanges(std::function<bool()> whatToDoOnYes)
    {
        // Show a popup warning about possible data loss
        auto metaData = std::make_unique<gui::DialogMetadataMessage>(
            gui::DialogMetadata{utils::translate("unsaved_changes"),
                                "delete_128px_W_G",
                                utils::translate("exit_without_saving"),
                                "",
                                [=]() -> bool {
                                    application->returnToPreviousWindow(); // To exit this popup
                                    return whatToDoOnYes();
                                }});

        application->switchWindow(gui::window::name::dialog_yes_no, std::move(metaData));
    }
} // namespace gui
