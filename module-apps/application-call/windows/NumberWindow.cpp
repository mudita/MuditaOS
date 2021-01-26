// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NumberWindow.hpp"

#include "application-call/ApplicationCall.hpp"
#include "application-call/data/CallAppStyle.hpp"
#include "application-call/data/CallSwitchData.hpp"

#include <ContactRecord.hpp>
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/Image.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/TopBar.hpp>
#include <gui/widgets/Window.hpp>
#include <i18n/i18n.hpp>
#include <InputMode.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-cellular/CellularServiceAPI.hpp>

#include <cassert>

namespace gui
{
    NumberWindow::NumberWindow(app::Application *app,
                               app::EnterNumberWindowInterface *interface,
                               std::string windowName)
        : AppWindow(app, std::move(windowName)), interface(interface), currentCountry(defaultCountry),
          numberUtil(*PhoneNumberUtil::GetInstance())
    {
        assert(interface != nullptr);
        assert(app != nullptr);
        switchFormatter(utils::country::getAlpha2Code(currentCountry));
    }

    void NumberWindow::setNumberLabel(const std::string &num)
    {
        numberLabel->setText(num);

        if (num.empty()) {
            bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
            return;
        }
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_call_clear"));
    }

    void NumberWindow::buildInterface()
    {
        using namespace callAppStyle;
        using namespace callAppStyle::enterNumberWindow;
        AppWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::LEFT, true);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);

        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(callAppStyle::strings::call));

        numberLabel = new gui::Label(this, numberLabel::x, numberLabel::y, numberLabel::w, numberLabel::h);
        numberLabel->setPenWidth(numberLabel::borderW);
        numberLabel->setFont(style::window::font::largelight);
        numberLabel->setEdges(RectangleEdge::Bottom);
        numberLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        numberLabel->setEllipsis(Ellipsis::Left);

        numberDescriptionLabel = new gui::Label(this,
                                                numberDescriptionLabel::x,
                                                numberDescriptionLabel::y,
                                                numberDescriptionLabel::w,
                                                numberDescriptionLabel::h);
        numberDescriptionLabel->setFont(style::window::font::mediumlight);
        numberDescriptionLabel->setAlignment(
            gui::Alignment{gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center});
        numberDescriptionLabel->setEdges(RectangleEdge::None);
    }

    void NumberWindow::destroyInterface()
    {
        erase();
    }

    bool NumberWindow::onInput(const InputEvent &inputEvent)
    {
        auto code = translator.handle(inputEvent.key, InputMode({InputMode::phone}).get());
        if (inputEvent.isShortPress()) {
            // Call function
            if (inputEvent.is(KeyCode::KEY_LF)) {
                interface->handleCallEvent(enteredNumber);
                return true;
            }
            // Clear/back function
            else if (inputEvent.is(KeyCode::KEY_RF)) {
                // if there isn't any char in phone number field return to previous application
                if (enteredNumber.empty()) {
                    formatter->Clear();
                    app::manager::Controller::switchBack(application);
                }
                // if there is the last char just clear input
                else if (enteredNumber.size() == 1) {
                    clearInput();
                }
                else {
                    // remove last digit and reformat entered number otherwise
                    enteredNumber = enteredNumber.substr(0, enteredNumber.size() - 1);
                    initFormatterInput(enteredNumber);
                    setNumberLabel(formattedNumber);

                    application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
                }
                return true;
            }
            else if (code != 0) {
                addDigit(code);
                return true;
            }
        }
        else if (inputEvent.isLongPress()) {
            // erase all characters from phone number
            if (inputEvent.is(KeyCode::KEY_RF)) {
                // if there isn't any char in phone number field return to previous application
                if (enteredNumber.empty()) {
                    app::manager::Controller::switchBack(application);
                    return true;
                }
                clearInput();

                return true;
            }
            // long press of '0' key is translated to '+'
            else if (inputEvent.is(KeyCode::KEY_0)) {
                addDigit('+');
                return true;
            }
        }

        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    void NumberWindow::switchFormatter(const std::string &regionCode)
    {
        auto newFormatter = std::unique_ptr<Formatter>(numberUtil.GetAsYouTypeFormatter(regionCode));
        formatter.swap(newFormatter);
        LOG_INFO("Switched formatter to region: %s", regionCode.c_str());
    }

    void NumberWindow::initFormatterInput(const std::string &number)
    {
        enteredNumber = number;
        formatter->Clear();
        for (auto c : number) {
            formatter->InputDigit(c, &formattedNumber);
        }
    }

    void NumberWindow::addDigit(const std::string::value_type &digit)
    {
        enteredNumber += digit;
        formatter->InputDigit(digit, &formattedNumber);
        setNumberLabel(formattedNumber);
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }

    const std::string &NumberWindow::getEnteredNumber() const noexcept
    {
        return enteredNumber;
    }

    void NumberWindow::clearInput()
    {
        enteredNumber.clear();
        formattedNumber.clear();
        formatter->Clear();
        setNumberLabel("");
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }
} /* namespace gui */
