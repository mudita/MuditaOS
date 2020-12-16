// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionWindow.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include <i18n/i18n.hpp>
#include "log/log.hpp"
#include <service-appmgr/model/ApplicationManager.hpp>
#include <Style.hpp>
#include <cassert>
#include <functional>
#include <memory>
#include <utility>
#include <messages/OptionsWindow.hpp>

namespace gui
{

    OptionWindow::OptionWindow(app::Application *app, const std::string &name) : AppWindow(app, name)
    {
        buildInterface();
    }

    OptionWindow::OptionWindow(app::Application *app, const std::string &name, std::list<Option> options)
        : AppWindow(app, name), options(std::move(options))
    {
        buildInterface();
    }

    void OptionWindow::rebuild()
    {}

    void OptionWindow::addOptionLabel(const UTF8 &text, std::function<bool(Item &)> activatedCallback, Arrow arrow)
    {
        body->addWidget(Option(text, activatedCallback, arrow).build());
    }

    void OptionWindow::addOptions(std::list<Option> &optionList)
    {
        for (auto &option : optionList) {
            body->addWidget(option.build());
        }
        body->switchPage(0);
    }

    void OptionWindow::addOptions(std::list<Option> &&optionList)
    {
        options = std::move(optionList);
        addOptions(options);
    }

    void OptionWindow::resetOptions(std::list<Option> &&optionList)
    {
        clearOptions();
        addOptions(std::move(optionList));
    }

    void OptionWindow::clearOptions()
    {
        body->erase();
    }

    void OptionWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);
        setTitle(name);

        // magical offset on designs
        int32_t offset_h = 8;

        body = new gui::PageLayout(this,
                                   {0,
                                    title->offset_h() + offset_h,
                                    this->getWidth(),
                                    this->getHeight() - offset_h - this->title->offset_h() - bottomBar->getHeight()});

        clearOptions();
        addOptions(options);
        setFocusItem(body);
    }

    void OptionWindow::destroyInterface()
    {
        erase();
        body = nullptr;
    }

    OptionWindow::~OptionWindow()
    {
        destroyInterface();
    }

    void OptionWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto message = dynamic_cast<gui::OptionsWindowOptions *>(data)) {
            LOG_DEBUG("Options load!");
            resetOptions(message->takeOptions());
        }
    }
} /* namespace gui */
