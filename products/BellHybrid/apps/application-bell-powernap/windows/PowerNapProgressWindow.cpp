// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapProgressWindow.hpp"
#include "application-bell-powernap/ApplicationBellPowerNap.hpp"
#include "data/PowerNapStyle.hpp"
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/BarGraph.hpp>
#include <apps-common/GuiTimer.hpp>
#include <Text.hpp>

namespace
{
    void decorateProgressItem(gui::Rect *item, gui::Alignment::Vertical alignment)
    {
        item->setEdges(gui::RectangleEdge::None);
        item->activeItem = false;
        item->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, alignment));
    }
    void createTitle(gui::VBox *parent)
    {
        auto title = new gui::Text(parent, 0, 0, parent->getWidth(), parent->getHeight() / 2);
        title->setFont(gui::powerNapStyle::descriptionFont);
        title->setText(utils::translate("app_bellmain_power_nap"));
        decorateProgressItem(title, gui::Alignment::Vertical::Top);
    }
    gui::HBarGraph *createProgress(gui::VBox *parent)
    {
        auto progressBox = new gui::HBox(parent, 0, 0, parent->getWidth(), parent->getHeight() / 2);
        decorateProgressItem(progressBox, gui::Alignment::Vertical::Bottom);
        auto progressBar =
            new gui::HBarGraph(progressBox, 0, 0, gui::powerNapStyle::progress::boxesCount, gui::BarGraphStyle::Heavy);
        decorateProgressItem(progressBar, gui::Alignment::Vertical::Center);
        return progressBar;
    }
    gui::Text *createTimer(gui::Item *parent)
    {
        using namespace style;
        using namespace gui::powerNapStyle;
        auto timer = new gui::Text(
            parent, 0, 0, bell_base_layout::w, bell_base_layout::outer_layouts_h - progress::bottomDescTopMargin);
        timer->setFont(descriptionFont);
        timer->setMargins(gui::Margins(0, progress::bottomDescTopMargin, 0, 0));
        decorateProgressItem(timer, gui::Alignment::Vertical::Top);
        return timer;
    }
} // namespace

namespace gui
{
    PowerNapProgressWindow::PowerNapProgressWindow(
        app::Application *app, std::shared_ptr<app::powernap::PowerNapProgressContract::Presenter> presenter)
        : AppWindow(app, gui::window::name::powernapProgress), presenter{std::move(presenter)}
    {
        buildInterface();
    }

    void PowerNapProgressWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        presenter->activate();
    }

    void PowerNapProgressWindow::buildInterface()
    {
        AppWindow::buildInterface();
        buildLayout();
        configureTimer();
    }
    void PowerNapProgressWindow::buildLayout()
    {
        auto body = new gui::BellBaseLayout(this, 0, 0, style::bell_base_layout::w, style::bell_base_layout::h, false);
        auto vBox =
            new VBox(body->getCenterBox(), 0, 0, style::bell_base_layout::w, style::bell_base_layout::centerbox::h);
        decorateProgressItem(vBox, gui::Alignment::Vertical::Top);
        createTitle(vBox);
        progressBar = createProgress(vBox);
        timerText   = createTimer(body->lastBox);

        dimensionChangedCallback = [&](Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }
    void PowerNapProgressWindow::configureTimer()
    {
        presenter->initTimer(this);
        presenter->getUIConfigurator().attach(progressBar);
        presenter->getUIConfigurator().attach(timerText);
    }
} // namespace gui
