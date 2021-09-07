// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-utils/time/time/time_locale.hpp>

namespace stm::internal
{
    /**
     * @brief Static settings data
     */
    class StaticData
    {
      private:
        bool isAutomaticDateAndTimeOn              = false;
        utils::time::Locale::DateFormat dateFormat = utils::time::Locale::DateFormat::DD_MM_YYYY;
        utils::time::Locale::TimeFormat timeFormat = utils::time::Locale::TimeFormat::FormatTime12H;
        std::string timezoneName;
        std::string timezoneRules;

        StaticData() = default;

      public:
        StaticData(const StaticData &) = delete;
        StaticData &operator=(const StaticData &) = delete;
        /**
         * Gets instance of static data object
         * @return instance of data object
         */
        static StaticData &get();
        /**
         * Sets value corresponded to current Automatic Date and Time setting
         * @param value new setting value
         */
        void setAutomaticDateAndTime(bool value);
        /**
         * Gets value corresponded to current Automatic Date and Time setting
         * @return actual setting value
         */
        [[nodiscard]] bool getAutomaticDateAndTime() const noexcept;
        /**
         * Sets value corresponded to current Date format setting
         * @param format new setting value
         */
        void setDateFormat(utils::time::Locale::DateFormat format);
        /**
         * Gets value corresponded to current Date format setting
         * @return actual setting value
         */
        [[nodiscard]] utils::time::Locale::DateFormat getDateFormat() const noexcept;
        /**
         * Sets value corresponded to current Time format setting
         * @param format new setting value
         */
        void setTimeFormat(utils::time::Locale::TimeFormat format);
        /**
         * Gets value corresponded to current Time format setting
         * @return actual setting value
         */
        [[nodiscard]] utils::time::Locale::TimeFormat getTimeFormat() const noexcept;
        /**
         * Sets value corresponded to current Timezone name setting
         * @param timezone new timezone to set
         */
        void setTimezoneName(const std::string &newTimezone);
        /**
         * Gets value corresponded to current Timezone name setting
         * @retrun actual timezone setting
         */
        [[nodiscard]] std::string getCurrentTimezoneName() const;
        /**
         * Sets value corresponded to current Timezone rules setting
         * @param timezone new timezone to set
         */
        void setTimezoneRules(const std::string &newTimezone);
        /**
         * Gets value corresponded to current Timezone rules setting
         * @retrun actual timezone setting
         */
        [[nodiscard]] std::string getCurrentTimezoneRules() const;
    };
} // namespace stm::internal
