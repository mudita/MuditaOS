// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmWindow.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/TimeSetFmtSpinner.hpp>

#include <data/BellAlarmStyle.hpp>

#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/TextFixedSize.hpp>
#include <module-gui/gui/widgets/ThreeBox.hpp>

#include <common/windows/BellFinishedWindow.hpp>

namespace gui
{
    BellAlarmWindow::BellAlarmWindow(
        app::ApplicationCommon *app,
        std::shared_ptr<app::bell_alarm::BellAlarmWindowContract::Presenter> windowPresenter,
        std::string name)
        : AppWindow(app, std::move(name)), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
        presenter->createData();
    }

    void BellAlarmWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(true);
        bottomBar->setVisible(false);

        body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height);

        topText = new Label(body->firstBox);
        topText->setMinimumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        topText->setText(utils::translate("app_bellmain_alarm"));
        topText->setFont(bell_alarm_style::top_text::font);
        topText->setEdges(RectangleEdge::None);
        topText->activeItem = false;
        topText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        timeSetFmtSpinner = new TimeSetFmtSpinner(body->getCenterBox());
        timeSetFmtSpinner->setFont(bell_alarm_style::time_set_fmt_spinner::focusFont,
                                   bell_alarm_style::time_set_fmt_spinner::noFocusFont);
        timeSetFmtSpinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        timeSetFmtSpinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        body->resize();

        setFocusItem(body);
    }

    bool BellAlarmWindow::onInput(const InputEvent &inputEvent)
    {
        if (body->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            presenter->saveData();
            application->switchWindow(gui::window::name::bellAlarmSet, nullptr);

            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void BellAlarmWindow::onBeforeShow(ShowMode, SwitchData *data)
    {
        presenter->onBeforeShow();
    }

    void BellAlarmWindow::setAlarmTime(time_t time)
    {
        timeSetFmtSpinner->setTime(time);
    }

    time_t BellAlarmWindow::getAlarmTime() const
    {
        return timeSetFmtSpinner->getTime();
    }

    void BellAlarmWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        timeSetFmtSpinner->setTimeFormat(fmt);
        body->resizeItems();
    }

    void BellAlarmWindow::rebuild()
    {
        erase();
        buildInterface();
    }
} /* namespace gui */
