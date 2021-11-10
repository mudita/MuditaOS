// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallLogDetailsWindow.hpp"
#include <memory>
#include <functional>

#include <OptionsWindow.hpp>
#include "ApplicationCallLog.hpp"
#include "data/CallLogInternals.hpp"
#include "data/CallLogSwitchData.hpp"
#include "windows/CallLogOptionsWindow.hpp"
#include <widgets/TextWithIconsWidget.hpp>
#include <widgets/ActiveIconFactory.hpp>

#include <gsl/assert>
#include <i18n/i18n.hpp>
#include <time/time_conversion_factory.hpp>
#include <Style.hpp>
#include <cassert>

namespace detailsWindow = callLogStyle::detailsWindow;
namespace gui
{

    CallLogDetailsWindow::CallLogDetailsWindow(app::ApplicationCommon *app)
        : AppWindow(app, calllog::settings::DetailsWindowStr)
    {
        buildInterface();
    }

    void CallLogDetailsWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    namespace
    {
        Rect *decorate(Rect *rect)
        {
            Expects(rect != nullptr);
            rect->setEdges(RectangleEdge::None);
            rect->activeItem = false;
            return rect;
        }

        [[nodiscard]] auto createText(const std::string &text, const UTF8 &font)
        {
            auto textWidget = new gui::Text();
            textWidget->setEdges(RectangleEdge::None);
            textWidget->setMaximumSize(detailsWindow::w, detailsWindow::widget::h);
            textWidget->setMinimumSize(detailsWindow::widget::h, Axis::Y);
            textWidget->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
            textWidget->setFont(font);
            textWidget->setText(text);
            return textWidget;
        }

        void addSnippetWithText(gui::Item *parent, const UTF8 &image, const std::string &text, const Margins &margins)
        {
            auto textHBox = decorate(new gui::HBox(parent, 0, 0, parent->getWidth(), detailsWindow::widget::h));
            textHBox->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));

            const auto font  = style::window::font::medium;
            auto imageWidget = new gui::Image(image, ImageTypeSpecifier::W_M);
            imageWidget->setMargins(margins);
            textHBox->addWidget(imageWidget);
            textHBox->addWidget(createText(text, font));
            parent->addWidget(new TextWithSnippet(text, font, image));
        }

        void addNextListHeader(gui::Item *parent, const std::string &text)
        {
            parent->addWidget(decorate(new TextWithSnippet(text, style::window::font::small)));
        }
    } // namespace

    void CallLogDetailsWindow::buildNumberWidget(gui::Item *parent)
    {
        Expects(parent != nullptr);
        addNextListHeader(parent, utils::translate(style::strings::common::information));
        numberHBox = new TextWithIconsWidget(parent);
    }

    void CallLogDetailsWindow::buildCallDataWidget(gui::Item *parent)
    {
        Expects(parent != nullptr);
        auto callBox = decorate(
            new gui::VBox(parent, 0, 0, parent->getWidth() / detailsWindow::callData::columns, parent->getHeight()));
        addNextListHeader(callBox, utils::translate("app_calllog_type"));
        typeHBox = new gui::HBox(callBox, 0, 0, callBox->getWidth(), detailsWindow::widget::h);
        typeHBox->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        decorate(typeHBox);
    }

    void CallLogDetailsWindow::buildCallDurationWidget(gui::Item *parent)
    {
        Expects(parent != nullptr);
        auto durationBox = decorate(
            new gui::VBox(parent, 0, 0, parent->getWidth() / detailsWindow::callData::columns, parent->getHeight()));
        addNextListHeader(durationBox, utils::translate("app_calllog_duration"));
        durationData = createText("", style::window::font::medium);
        durationBox->addWidget(durationData);
    }

    void CallLogDetailsWindow::buildDateWidgets(gui::Item *parent)
    {
        Expects(parent != nullptr);
        addNextListHeader(parent, utils::translate("app_calllog_date"));
        auto dateBox = decorate(new gui::VBox(parent, 0, 0, parent->getWidth(), detailsWindow::date::h));
        dateBox->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Bottom));
        dateDay  = new Text(dateBox, 0, 0, dateBox->getWidth(), detailsWindow::widget::smallH);
        dateDate = new Text(dateBox, 0, 0, dateBox->getWidth(), detailsWindow::widget::smallH);
        dateDay->setFont(style::window::font::medium);
        dateDate->setFont(style::window::font::medium);
    }

    void CallLogDetailsWindow::buildInterface()
    {
        AppWindow::buildInterface();

        navBar->setText(nav_bar::Side::Left, utils::translate(style::strings::common::options));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        auto vBox = new VBox(this, detailsWindow::x + 10, detailsWindow::y, detailsWindow::w, detailsWindow::h);
        vBox->setEdges(RectangleEdge::None);
        buildNumberWidget(vBox);

        auto callDataDurationBox = decorate(new gui::HBox(vBox, 0, 0, vBox->getWidth(), detailsWindow::callData::h));
        buildCallDataWidget(callDataDurationBox);
        buildCallDurationWidget(callDataDurationBox);

        buildDateWidgets(vBox);
    }

    void CallLogDetailsWindow::destroyInterface()
    {
        erase();
    }

    void CallLogDetailsWindow::initNumberWidget()
    {
        Expects(numberHBox != nullptr);
        numberHBox->erase();
        ActiveIconFactory factory(this->application);
        const auto &numberView = record.phoneNumber;
        if (record.presentation == PresentationType::PR_UNKNOWN) {
            numberHBox->addText(utils::translate(callLogStyle::strings::privateNumber),
                                style::window::font::mediumbold);
        }
        else {
            numberHBox->addText(numberView.getFormatted(), style::window::font::mediumbold);
            numberHBox->addIcon(factory.makeCallIcon(numberView));
            numberHBox->addIcon(factory.makeSMSIcon(numberView));
            setFocusItem(numberHBox);
        }
    }

    void CallLogDetailsWindow::initCallDataWidget()
    {
        Expects(typeHBox != nullptr);
        typeHBox->erase();
        UTF8 callIconName, callTypeStr;
        switch (record.type) {
        case CallType::CT_INCOMING:
            callTypeStr  = utils::translate("app_calllog_incoming_call");
            callIconName = "calllog_arrow_in";
            break;
        case CallType::CT_OUTGOING:
            callTypeStr  = utils::translate("app_calllog_outgoing_call");
            callIconName = "calllog_arrow_out";
            break;
        case CallType::CT_MISSED:
            callTypeStr  = utils::translate("app_calllog_missed_call");
            callIconName = "calllog_arrow_den";
            break;
        case CallType::CT_REJECTED:
            callTypeStr  = utils::translate("app_calllog_rejected_call");
            callIconName = "calllog_arrow_den";
            break;
        default:
            break;
        }
        addSnippetWithText(
            typeHBox, callIconName, callTypeStr, Margins(style::margins::big, 0, style::margins::big, 0));
    }

    void CallLogDetailsWindow::initCallDurationWidget()
    {
        Expects(durationData != nullptr);
        durationData->setText(utils::time::Duration(record.duration).str());
    }

    void CallLogDetailsWindow::initDateWidgets()
    {
        Expects(dateDay != nullptr && dateDate != nullptr);
        using namespace utils::time;
        auto date = TimestampFactory().createTimestamp(TimestampType::Date, record.date);
        auto time = TimestampFactory().createTimestamp(TimestampType::Time, record.date);

        dateDay->setText(date->day() + ",");

        dateDate->setText(date->str() + ", " + time->str());
    }

    void CallLogDetailsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_RETURN) {
            return;
        }

        if (auto switchData = dynamic_cast<calllog::CallLogSwitchData *>(data); data != nullptr) {
            record = switchData->getRecord();
            setTitle(record.name);
            initNumberWidget();
            initCallDataWidget();
            initCallDurationWidget();
            initDateWidgets();
        }
    }

    bool CallLogDetailsWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_LF)) {
            auto app = dynamic_cast<app::ApplicationCallLog *>(application);
            assert(app != nullptr);
            app->switchWindow(calllog::settings::CallLogOptionsStr,
                              std::make_unique<calllog::CallLogSwitchData>(record));

            return true;
        }
        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

} /* namespace gui */
