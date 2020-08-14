#pragma once
#include <Style.hpp>
#include <string>

namespace style
{
    namespace window
    {
        namespace calendar
        {

            namespace name
            {
                const inline std::string day_events_window    = "DayEventsWindow";
                const inline std::string no_events_window     = "NoEventsWindow";
                const inline std::string events_options       = "Options";
                const inline std::string dialog_yes_no        = "DialogYesNo";
                const inline std::string all_events_window    = "AllEventsWindow";
                const inline std::string details_window       = "DetailsWindow";
                const inline std::string new_edit_event       = "NewEditEvent";
                const inline std::string custom_repeat_window = "CustomRepeat";
            } // namespace name

            const inline std::string new_event  = "New";
            const inline std::string edit_event = "Edit";
            const inline int day_cell_width    = 60;
            const inline int day_cell_height   = 55;
            const inline int month_year_height = 60;
            const inline int week_days_number  = 7;
            const inline int max_weeks_number  = 6;

            const inline int cross_x    = 48;
            const inline int cross_y    = 55;
            const inline int arrow_x    = 30;
            const inline int arrow_y    = 62;
            const inline int listView_x = style::window::default_left_margin;
            const inline int listView_y = style::header::height;
            const inline int listView_w = style::listview::body_width_with_scroll;
            const inline int listView_h = style::window_height - listView_y - style::footer::height;

            namespace test
            {
                const inline int prev_month_id       = 1;
                const inline int month_id            = 2;
                const inline int next_month_id       = 3;
                const inline std::string date_text_1 = "January 2019";
                const inline std::string date_text_2 = "February 2019";
                const inline std::string date_text_3 = "March 2019";
            } // namespace test

            namespace time
            {
                const inline int max_time_length   = 2;
                const inline int max_hour_24H_mode = 23;
                const inline int max_hour_12H_mode = 12;
                const inline int max_minutes       = 59;
            } // namespace time

            namespace item
            {
                namespace dayEvents
                {
                    const inline int title_w    = 255;
                    const inline int box_height = style::window::label::small_h;
                    const inline int height     = 100;
                } // namespace dayEvents

                namespace allEvents
                {
                    const inline int description_w    = 310;
                    const inline int start_time_min_w = 60;
                } // namespace allEvents

                namespace repeatAndReminder
                {
                    const inline int height             = 150;
                    const inline int title_label_h      = 45;
                    const inline int title_label_margin = 15;
                    const inline int description_w      = style::window::default_body_width / 2 - 30;
                    const inline int description_h      = 30;
                } // namespace repeatAndReminder

                namespace eventDetail
                {
                    const inline int height_min    = 90;
                    const inline int height_max        = 155;
                    const inline int margin_top        = 2 * style::margins::big;
                    const inline int event_time_margin = 25;
                    const inline int title_h       = 20;
                    const inline int label_h           = 35;
                } // namespace eventDetail

                namespace eventTime
                {
                    const inline int height           = 106;
                    const inline int margin           = 21;
                    const inline int separator        = 30;
                    const inline int time_input_12h_w = 120;
                    const inline int time_input_24h_w = 195;
                } // namespace eventTime

                namespace checkBox
                {
                    const inline int height              = 44;
                    const inline int margin_top          = 18;
                    const inline int input_box_label_w   = style::window::label::big_h;
                    const inline int description_label_w = 280;
                } // namespace checkBox

                namespace severalOptions
                {
                    const inline int height    = 63;
                    const inline int label_h   = 30;
                    const inline int arrow_w_h = 20;
                } // namespace severalOptions

                namespace textWithLabel
                {
                    const inline int height        = 80;
                    const inline int description_h = 30;
                    const inline int text_input_h  = 40;
                } // namespace textWithLabel
            }     // namespace item
        };        // namespace calendar
    }             // namespace window
} // namespace style
