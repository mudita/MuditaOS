// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellMainVolumeWindow.hpp"
#include <application-bell-main/ApplicationBellMain.hpp>

#include <popups/data/PopupData.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>

namespace style
{
    inline constexpr auto volumeValueFont = style::window::font::supersizemelight;

    namespace progressVolume
    {
        inline constexpr auto radius                   = 192U;
        inline constexpr auto penWidth                 = 3U;
        inline constexpr auto verticalDeviationDegrees = 68U;
    } // namespace progressVolume
} // namespace style

namespace gui
{
    BellMainVolumeWindow::BellMainVolumeWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::home_screen::AbstractBellMainVolumePresenter> &&presenter)
        : WindowWithTimer(app, gui::popup::window::volume_window), presenter{std::move(presenter)}
    {
        buildInterface();
    }

    void BellMainVolumeWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height);

        auto topMessage = new TextFixedSize(body->firstBox);
        topMessage->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        topMessage->setFont(style::window::font::largelight);
        topMessage->setEdges(gui::RectangleEdge::None);
        topMessage->activeItem = false;
        topMessage->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        topMessage->setText(utils::translate("app_settings_volume"));
        topMessage->drawUnderline(false);

        const auto progressArcRadius = style::progressVolume::radius;
        const auto progressArcWidth  = style::progressVolume::penWidth;
        const auto arcStartAngle     = -90 - style::progressVolume::verticalDeviationDegrees;
        const auto arcSweepAngle     = 360 - (2 * (style::progressVolume::verticalDeviationDegrees));
        const auto arcProgressSteps  = app::AbstractAudioModel::maxVolume;

        Arc::ShapeParams arcParams;
        arcParams.setCenterPoint(Point(getWidth() / 2, getHeight() / 2))
            .setRadius(progressArcRadius)
            .setStartAngle(arcStartAngle)
            .setSweepAngle(arcSweepAngle)
            .setPenWidth(progressArcWidth)
            .setBorderColor(ColorFullBlack);

        progress = new ArcProgressBar(this, arcParams, ArcProgressBar::ProgressDirection::CounterClockwise);
        progress->setMaximum(arcProgressSteps);
        progress->setValue(presenter->getVolume());

        const auto data = presenter->getVolumeData();
        spinner         = new U8IntegerSpinner({static_cast<U8IntegerSpinner::value_type>(data.min),
                                        static_cast<U8IntegerSpinner::value_type>(data.max),
                                        static_cast<U8IntegerSpinner::value_type>(data.step)},
                                       Boundaries::Fixed);
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::center_layout_h);
        spinner->setFont(style::volumeValueFont);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setFocusEdges(RectangleEdge::None);
        spinner->set_value(static_cast<U8IntegerSpinner::value_type>(presenter->getVolume()));
        body->getCenterBox()->addWidget(spinner);

        spinner->onValueChanged = [this](const auto &value) { presenter->setVolume(value); };
        body->setMinMaxArrowsVisibility(spinner->value() == data.min, spinner->value() == data.max);

        setFocusItem(body);
        body->resize();

        setWindowTimerActionCallback([this]() { presenter->saveVolume(spinner->value()); });
    }

    bool BellMainVolumeWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            detachTimerIfExists();
            presenter->saveVolume(spinner->value());
            application->returnToPreviousWindow();
            return true;
        }

        resetTimer();
        const auto data          = presenter->getVolumeData();
        const auto ret           = body->onInput(inputEvent);
        const auto selectedValue = spinner->value();
        progress->setValue(selectedValue);
        body->setMinMaxArrowsVisibility(selectedValue == data.min, selectedValue == data.max);
        return ret;
    }
} // namespace gui
