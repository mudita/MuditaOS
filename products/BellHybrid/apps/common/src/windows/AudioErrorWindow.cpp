// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "windows/AudioErrorWindow.hpp"

#include <Application.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>

namespace
{
    constexpr auto windowTimeout             = std::chrono::seconds{6};
    constexpr auto unsupportedMediaMessage   = "app_bell_audio_error_message";
    constexpr auto exceededFilesLimitMessage = "app_bell_audio_limit_error_message";
    constexpr auto fileDeletedMessage        = "app_bell_audio_file_deleted_message";
    constexpr auto emptyErrorMessage         = "";

    constexpr auto imageMarginTop = 122U;
    constexpr auto textPaddingTop = 30U;
} // namespace

namespace gui
{
    AudioErrorWindow::AudioErrorWindow(app::ApplicationCommon *app,
                                       const std::string &name,
                                       std::unique_ptr<AudioErrorContract::Presenter> &&presenter,
                                       OnExitCallback callback)
        : WindowWithTimer(app, name, windowTimeout), presenter{std::move(presenter)}, onExitCallback{
                                                                                          std::move(callback)}
    {
        if (onExitCallback == nullptr) {
            onExitCallback = [this]() { application->returnToPreviousWindow(); };
        }
        buildInterface();
    }

    void AudioErrorWindow::buildInterface()
    {
        AppWindow::buildInterface();
        buildLayout();
        registerCallbacks();
    }

    void AudioErrorWindow::buildLayout()
    {
        statusBar->setVisible(false);

        icon = new Icon(
            this, 0, 0, style::window_width, style::window_height, "big_information", "", ImageTypeSpecifier::W_G);
        icon->image->setMargins({0, imageMarginTop, 0, 0});
        icon->text->setFont(style::window::font::verybiglight);
        const auto textPadding = icon->text->getPadding();
        icon->text->setPadding({textPadding.left, textPaddingTop, textPadding.right, textPadding.bottom});
        icon->text->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        icon->resizeItems();
    }

    void AudioErrorWindow::registerCallbacks()
    {
        timerCallback = [this](Item &, sys::Timer &timer) {
            onExitCallback();
            return true;
        };
    }

    void AudioErrorWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (data && typeid(*data) == typeid(AudioErrorParams)) {
            auto *errorData = static_cast<AudioErrorParams *>(data);
            errorType       = errorData->getErrorType();
            switch (errorType) {
            case AudioErrorType::UnsupportedMediaType: {
                icon->text->setRichText(utils::translate(unsupportedMediaMessage));
                break;
            }
            case AudioErrorType::FilesLimitExceeded: {
                icon->text->setRichText(utils::translate(exceededFilesLimitMessage));
                break;
            }
            case AudioErrorType::FileDeleted: {
                icon->text->setRichText(utils::translate(fileDeletedMessage));
                break;
            }
            default:
                icon->text->setRichText(utils::translate(emptyErrorMessage));
            }
        }
        WindowWithTimer::onBeforeShow(mode, data);
    }

    bool AudioErrorWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            detachTimerIfExists();
            onExitCallback();
            return true;
        }
        return true;
    }
} // namespace gui
