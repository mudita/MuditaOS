#include "SeveralOptionsItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <Style.hpp>
#include <Utils.hpp>

namespace gui
{

    SeveralOptionsItem::SeveralOptionsItem(app::Application *app,
                                           const std::string &description,
                                           std::function<void(const UTF8 &)> bottomBarTemporaryMode,
                                           std::function<void()> bottomBarRestoreFromTemporaryMode)
        : bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode))
    {
        application = app;
        assert(app != nullptr);

        setMinimumSize(style::window::default_body_width, style::window::calendar::item::severalOptions::height);
        setMaximumSize(style::window::default_body_width, style::window::calendar::item::severalOptions::height);

        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        setPenWidth(style::window::default_border_rect_no_focus);
        setMargins(gui::Margins(0, style::window::calendar::item::severalOptions::margin, 0, 0));

        hBox = new gui::HBox(this, 0, 0, style::window::default_body_width, 0);
        hBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        hBox->setPenFocusWidth(style::window::default_border_focus_w);
        hBox->setPenWidth(style::window::default_border_rect_no_focus);
        hBox->activeItem = false;

        descriptionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        descriptionLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        descriptionLabel->setFont(style::window::font::small);
        descriptionLabel->activeItem = false;
        descriptionLabel->setText(description);

        optionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        optionLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        optionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        optionLabel->setFont(style::window::font::small);
        optionLabel->activeItem = false;

        leftArrow             = new gui::Image(hBox, 0, 0, 0, 0);
        leftArrow->activeItem = false;
        leftArrow->set("arrow_left");

        rightArrow             = new gui::Image(hBox, 0, 0, 0, 0);
        rightArrow->activeItem = false;
        rightArrow->set("arrow_right");

        prepareOptionsNames();
        setNavigation();
    }

    void SeveralOptionsItem::prepareOptionsNames()
    {
        optionsNames.clear();
        if (descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_reminder")) {
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_never"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_event_time"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_5_min_before"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_15_min_before"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_30_min_before"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_1_hour_before"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_2_hour_before"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_1_day_before"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_2_days_before"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_1_week_before"));
        }
        else if (descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_repeat")) {
            optionsNames.push_back(utils::localize.get("app_calendar_repeat_never"));
            optionsNames.push_back(utils::localize.get("app_calendar_repeat_daily"));
            optionsNames.push_back(utils::localize.get("app_calendar_repeat_weekly"));
            optionsNames.push_back(utils::localize.get("app_calendar_repeat_two_weeks"));
            optionsNames.push_back(utils::localize.get("app_calendar_repeat_month"));
            optionsNames.push_back(utils::localize.get("app_calendar_repeat_year"));
            optionsNames.push_back(utils::localize.get("app_calendar_repeat_custom"));
        }
        optionLabel->setText(optionsNames[0]);
    }

    void SeveralOptionsItem::setNavigation()
    {
        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (event.keyCode == gui::KeyCode::KEY_LEFT) {
                actualVectorIndex--;
                if (actualVectorIndex >= optionsNames.size()) {
                    actualVectorIndex = optionsNames.size() - 1;
                    if (descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_repeat")) {
                        bottomBarTemporaryMode(utils::localize.get("app_calendar_edit"));
                    }
                }
                else {
                    bottomBarRestoreFromTemporaryMode();
                }
                optionLabel->setText(optionsNames[actualVectorIndex]);
                return true;
            }
            if (event.keyCode == gui::KeyCode::KEY_RIGHT) {
                actualVectorIndex++;
                if (actualVectorIndex >= optionsNames.size()) {
                    actualVectorIndex = 0;
                }
                optionLabel->setText(optionsNames[actualVectorIndex]);
                if (actualVectorIndex == optionsNames.size() - 1 &&
                    descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_repeat")) {
                    bottomBarTemporaryMode(utils::localize.get("app_calendar_edit"));
                }
                else {
                    bottomBarRestoreFromTemporaryMode();
                }
                return true;
            }
            if (event.keyCode == gui::KeyCode::KEY_LF && actualVectorIndex == optionsNames.size() - 1 &&
                descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_repeat")) {
                application->switchWindow(style::window::calendar::name::custom_repeat_window);
                return true;
            }
            return false;
        };
    }

    bool SeveralOptionsItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        hBox->setPosition(0, 0);
        hBox->setSize(newDim.w, newDim.h);
        LOG_DEBUG("SIZE SEVERALOPTIONS ITEM: %i,  %i", newDim.w, newDim.h);

        descriptionLabel->setArea(BoundingBox(0,
                                              0,
                                              style::window::calendar::item::severalOptions::description_label_w,
                                              style::window::calendar::item::severalOptions::description_label_h));
        optionLabel->setArea(BoundingBox(style::window::calendar::item::severalOptions::arrow_w,
                                         style::window::calendar::item::severalOptions::option_label_y,
                                         newDim.w - 2 * style::window::calendar::item::severalOptions::arrow_w,
                                         style::window::calendar::item::severalOptions::option_label_h));
        leftArrow->setArea(BoundingBox(0,
                                       style::window::calendar::item::severalOptions::arrow_y,
                                       style::window::calendar::item::severalOptions::arrow_w,
                                       style::window::calendar::item::severalOptions::arrow_h));
        rightArrow->setArea(BoundingBox(newDim.w - style::window::calendar::item::severalOptions::arrow_w,
                                        style::window::calendar::item::severalOptions::arrow_y,
                                        style::window::calendar::item::severalOptions::arrow_w,
                                        style::window::calendar::item::severalOptions::arrow_h));

        return true;
    }

} /* namespace gui */
