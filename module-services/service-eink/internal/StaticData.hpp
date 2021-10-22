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

        void setInvertedMode(bool enabled) noexcept;
        [[nodiscard]] auto isInvertedModeEnabled() const noexcept -> bool;

      private:
        bool invertedEnabled{false};

        StaticData() noexcept = default;
    };
} // namespace service::eink::internal
