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
                const inline std::string day_events_window = "DayEventsWindow";
                const inline std::string no_events_window  = "NoEventsWindow";
                const inline std::string events_options    = "Options";
                const inline std::string dialog_yes_no     = "DialogYesNo";
                const inline std::string all_events_window = "AllEventsWindow";
                const inline std::string details_window    = "DetailsWindow";
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
                const inline int prev_month_id = 1;
                const inline int month_id      = 2;
                const inline int next_month_id = 3;
                const inline std::string date_text_1 = "January 2019";
                const inline std::string date_text_2 = "February 2019";
                const inline std::string date_text_3 = "March 2019";
            } // namespace test

            namespace item
            {
                const inline int label_x      = 11;
                const inline int title_y      = 10;
                const inline int title_w      = 250;
                const inline int title_h      = 40;
                const inline int descripton_y = 42;
                const inline int height       = 90;

                namespace all
                {
                    const inline int description_w = 310;
                    const inline int description_x = 120;
                    const inline int start_time_x  = 11;
                } // namespace all

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
            } // namespace item

            namespace options
            {
                const inline int item_height = 55;
                const inline int margin      = 15;
                const inline int text_margin = 10;
                const inline int label_y     = 112;
            } // namespace options

            namespace details
            {
                const inline int description_y    = 225;
                const inline int time_y           = 175;
                const inline int repeat_heading_y = 290;
                const inline int repeat_y         = 345;
                const inline int remind_heading_y = 290;
                const inline int remind_y         = 345;
            } // namespace details

        }; // namespace calendar
    }      // namespace window
} // namespace style
