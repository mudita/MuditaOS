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

            const inline int day_cell_width    = 60;
            const inline int day_cell_height   = 55;
            const inline int month_year_height = 60;
            const inline int week_days_number  = 7;
            const inline int max_weeks_number  = 6;

            const inline int cross_x    = 48;
            const inline int cross_y    = 55;
            const inline int arrow_x    = 30;
            const inline int arrow_y    = 62;
            const inline int listView_x = 16;
            const inline int listView_y = 110;
            const inline int listView_w = 448;
            const inline int listView_h = 440;

            namespace test
            {
                const inline int prev_month_id       = 1;
                const inline int month_id            = 2;
                const inline int next_month_id       = 3;
                const inline std::string date_text_1 = "January 2019";
                const inline std::string date_text_2 = "February 2019";
                const inline std::string date_text_3 = "March 2019";
            } // namespace test

            namespace item
            {
                namespace dayEvents
                {
                    const inline int title_w       = 200;
                    const inline int h_box_h       = 40;
                    const inline int description_h = 30;
                    const inline int margins       = 10;
                    const inline int height        = 90;
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
                    const inline int height_max    = 160;
                    const inline int margin_top    = 25;
                    const inline int margin_bottom = 10;
                    const inline int title_h       = 20;
                    const inline int event_time_h  = 30;
                    const inline int description_h = 40;
                } // namespace eventDetail

                namespace eventTime
                {
                    const inline int height               = 120;
                    const inline int margin               = 22;
                    const inline int description_label_w  = 150;
                    const inline int description_label_h  = 30;
                    const inline int colon_label_w        = 25;
                    const inline int colon_label_h        = 20;
                    const inline int time_input_12h_w     = 125;
                    const inline int time_input_24h_w     = 202;
                    const inline int mode12h_input_margin = 25;
                } // namespace eventTime

                namespace checkBox
                {
                    const inline int height              = 52;
                    const inline int margin_top          = 6;
                    const inline int margin_bottom       = 5;
                    const inline int input_box_label_w   = 55;
                    const inline int input_box_label_h   = 50;
                    const inline int description_label_w = 280;
                    const inline int description_label_h = 40;
                    const inline int description_margin  = 20;
                    const inline int tick_image_w        = 30;
                    const inline int tick_image_h        = 30;
                } // namespace checkBox

                namespace severalOptions
                {
                    const inline int height              = 62;
                    const inline int margin              = 15;
                    const inline int description_label_w = 100;
                    const inline int description_label_h = 30;
                    const inline int option_label_h      = 30;
                    const inline int arrow_w             = 20;
                    const inline int arrow_h             = 20;
                } // namespace severalOptions

                namespace textWithLabel
                {
                    const inline int height        = 80;
                    const inline int margin        = 5;
                    const inline int description_h = 30;
                    const inline int text_input_h  = 40;
                } // namespace textWithLabel
            }     // namespace item
        };        // namespace calendar
    }             // namespace window
} // namespace style
