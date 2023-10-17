// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationErrorWindow.hpp"
#include <data/RelaxationStyle.hpp>
#include <ApplicationBellRelaxation.hpp>

#include <apps-common/widgets/BellBaseLayout.hpp>

namespace
{
    constexpr auto unsupportedMediaMessage   = "app_bell_relaxation_error_message";
    constexpr auto exceededFilesLimitMessage = "app_bell_relaxation_limit_error_message";
    constexpr auto fileDeletedMessage        = "app_bell_relaxation_file_deleted_message";
    constexpr auto emptyErrorMessage         = "";

} // namespace

namespace gui
{
    RelaxationErrorWindow::RelaxationErrorWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::relaxation::RelaxationErrorContract::Presenter> &&presenter)
        : WindowWithTimer(app, gui::window::name::relaxationError, std::chrono::seconds{6}), presenter{
                                                                                                 std::move(presenter)}
    {
        buildInterface();
    }

    void RelaxationErrorWindow::buildInterface()
    {
        AppWindow::buildInterface();
        buildLayout();
        registerCallbacks();
    }

    void RelaxationErrorWindow::buildLayout()
    {
        statusBar->setVisible(false);

        icon = new Icon(
            this, 0, 0, style::window_width, style::window_height, "big_information", "", ImageTypeSpecifier::W_G);
        icon->image->setMargins({0, gui::relaxationStyle::error::imageMarginTop, 0, 0});
        icon->text->setFont(style::window::font::verybiglight);
        const auto textPadding = icon->text->getPadding();
        icon->text->setPadding(
            {textPadding.left, gui::relaxationStyle::error::textPaddingTop, textPadding.right, textPadding.bottom});
        icon->text->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        icon->resizeItems();
    }

    void RelaxationErrorWindow::registerCallbacks()
    {
        timerCallback = [this](Item &, sys::Timer &timer) {
            application->switchWindow(gui::name::window::main_window);
            if (errorType == RelaxationErrorType::FileDeleted) {
                application->getWindow(gui::name::window::main_window)->rebuild();
            }
            return true;
        };
    }

    void RelaxationErrorWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (data && typeid(*data) == typeid(RelaxationErrorData)) {
            auto *errorData = static_cast<RelaxationErrorData *>(data);
            errorType       = errorData->getErrorType();
            switch (errorType) {
            case RelaxationErrorType::UnsupportedMediaType: {
                icon->text->setRichText(utils::translate(unsupportedMediaMessage));
                break;
            }
            case RelaxationErrorType::FilesLimitExceeded: {
                icon->text->setRichText(utils::translate(exceededFilesLimitMessage));
                break;
            }
            case RelaxationErrorType::FileDeleted: {
                icon->text->setRichText(utils::translate(fileDeletedMessage));
                break;
            }
            default:
                icon->text->setRichText(utils::translate(emptyErrorMessage));
            }
        }
        WindowWithTimer::onBeforeShow(mode, data);
    }

    bool RelaxationErrorWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            application->switchWindow(gui::name::window::main_window);
            if (errorType == RelaxationErrorType::FileDeleted) {
                application->getWindow(gui::name::window::main_window)->rebuild();
            }
            return true;
        }
        return true;
    }
} // namespace gui
