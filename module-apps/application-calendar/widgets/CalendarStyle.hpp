#pragma once
#include <string>

namespace style
{
    namespace window
    {
        namespace calendar
        {

            namespace name
            {
                const inline std::string day_window       = "DayWindow";
                const inline std::string no_events_window = "NoEventsWindow";
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
            } // namespace test

            namespace item
            {
                const inline int label_x      = 11;
                const inline int title_y      = 10;
                const inline int title_w      = 250;
                const inline int title_h      = 40;
                const inline int descripton_y = 42;
                const inline int height       = 90;
            } // namespace item

            namespace options
            {
                const inline int item_height = 55;
                const inline int margin      = 15;
                const inline int text_margin = 10;
                const inline int label_y     = 112;
            } // namespace options

            namespace deletion
            {
                const inline int label_w     = 150;
                const inline int label_h     = 80;
                const inline int label_y     = 420;
                const inline int label_no_x  = 70;
                const inline int label_yes_x = 260;
            } // namespace deletion
        };    // namespace calendar
    }         // namespace window
} // namespace style
