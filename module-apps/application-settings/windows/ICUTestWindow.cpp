#include "ICUTestWindow.hpp"
#include <gui/widgets/Rect.hpp>
#include <i18/i18.hpp>
#include <memory>
#include <unicode/calendar.h>
#include <unicode/datefmt.h>
#include <unicode/errorcode.h>
#include <unicode/utypes.h>

namespace gui
{

    ICUTestWindow::ICUTestWindow(app::Application *parent, std::string window_name) : AppWindow(parent, window_name)
    {
        LOG_INFO("Creating ICU Test");
        buildInterface();
    }

    ICUTestWindow::~ICUTestWindow()
    {
        destroyInterface();
    }

    void ICUTestWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void ICUTestWindow::onBeforeShow(ShowMode /*mode*/, SwitchData * /*data*/)
    {
    }

    void ICUTestWindow::buildInterface()
    {
        //    auto addOrDelete = [](auto *theBox, auto *theItem) {
        //        if (!theBox->addWidget(theItem)) {
        //            LOG_INFO("adding error!");
        //            delete theItem;
        //            return;
        //        }
        //        LOG_INFO("adding success!");
        //    };
        LOG_INFO("Create ICU window");
        AppWindow::buildInterface();
        setTitle("ICU Test");

        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_select"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

        mainBox = new gui::VBox(this, 0, title->offset_h(), style::window_width, style::window_height);
        mainBox->setPenWidth(style::window::default_border_no_focus_w);

        auto rowAdd = [&](std::string labelTxt, gui::HBox **hBox, gui::Text **text, InputMode::Mode inputMode = InputMode::digit) {
            *hBox = new HBox(mainBox, 0, 0, mainBox->getWidth(), style::window::label::default_h);
            gui::Label *label = new gui::Label(*hBox, 0, 0, style::window_width / 3, style::window::label::default_h);
            *text = new gui::Text(*hBox, 0, 0, style::window_width / 2, style::window::label::default_h);
            (*hBox)->setPenWidth(0);
            (*hBox)->setPenFocusWidth(0);
            label->setText(utils::localize.get(std::move(labelTxt)));
            label->setPenWidth(0);
            label->activeItem = false;
            (*text)->setPenWidth(0);
            (*text)->setPenFocusWidth(3);
            (*text)->setInputMode(new InputMode({inputMode}));
            (*text)->contentCallback = [&](Item &) {
                //                LOG_INFO("CALLINGNGGLKA");
                updateDate();
                return true;
            };
        };

        gui::HBox *dayBox;
        rowAdd("Day", &dayBox, &dayText);

        gui::HBox *monthBox;
        rowAdd("Month", &monthBox, &monthText);

        gui::HBox *yearBox;
        rowAdd("Year", &yearBox, &yearText);

        gui::HBox *localeBox;
        rowAdd("Locale", &localeBox, &localeText, InputMode::abc);
        localeText->setText("pl_PL");

        gui::Rect *hr = new gui::Rect(mainBox, 0, 0, style::window_width, 4);
        hr->activeItem = false;

        gui::Label *plShortLable = new gui::Label(mainBox, 0, 0, style::window_width, style::window::label::default_h);
        plShortLable->setText("short date");
        plShortLable->activeItem = false;
        plShortLable->setPenWidth(0);

        plShort = new gui::Label(mainBox, 0, 0, style::window_width, style::window::label::default_h);
        plShort->setText("---------");
        plShort->setPenWidth(0);
        plShort->activeItem = false;

        hr = new gui::Rect(mainBox, 0, 0, style::window_width, 1);
        hr->activeItem = false;

        setFocusItem(mainBox);

        //    addOrDelete(dayBox, dayLabel);
        //    addOrDelete(dayBox, dayText);
    }

    void ICUTestWindow::destroyInterface()
    {
        this->focusItem = nullptr;
        AppWindow::destroyInterface();
    }

    void ICUTestWindow::updateDate()
    {
        if (yearText->getText().length() && monthText->getText().length() && dayText->getText().length())
        {
            icu_66::ErrorCode errorCode;
            std::unique_ptr<icu::DateFormat> df;
            df.reset(icu::DateFormat::createDateInstance(icu::DateFormat::LONG, localeText->getText().c_str()));
            std::unique_ptr<icu::Calendar> calendar;
            // calendar.reset(icu::Calendar::createInstance(icu::Locale(localeText->getText()"pl_PL"), errorCode));
            calendar.reset(icu::Calendar::createInstance(icu::Locale(localeText->getText().c_str()), errorCode));
            if (errorCode.isSuccess())
            {

                calendar->set(std::atoi(yearText->getText().c_str()), std::atoi(monthText->getText().c_str()) - 1, std::atoi(dayText->getText().c_str()));

                icu::UnicodeString dateString;
                dateString = df->format(calendar->getTime(errorCode), dateString);
                std::string convertString;
                dateString.toUTF8String(convertString);
                LOG_INFO("Date: %s", convertString.c_str());
                plShort->setText(std::move(convertString));
                //        plShort->setText(dayText->getText() + "-" + monthText->getText() + "-" + yearText->getText());
            }
            else
            {
                plShort->setText("Wrong locale!");
            }
        }
    }

} // namespace gui
