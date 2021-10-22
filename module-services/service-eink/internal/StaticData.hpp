// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace service::eink::internal
{
    /**
     * @brief Static settings data
     */
    class StaticData
    {
      public:
        StaticData(const StaticData &) = delete;
        StaticData &operator=(const StaticData &) = delete;

        /**
         * Gets instance of static data object
         * @return instance of data object
         */
        static StaticData &get();

        void enableDarkMode() noexcept;
        void disableDarkMode() noexcept;
        [[nodiscard]] auto isDarkModeEnabled() const noexcept -> bool;

      private:
        bool darkModeEnabled{false};

        StaticData() noexcept = default;
    };
} // namespace service::eink::internal
