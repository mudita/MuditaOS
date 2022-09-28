// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

namespace db::query
{
    /**
     * @brief This template class eases constructing custom Queries which carry custom filter data.
     * This helper interface allows to inform that no filter data is present (in an abstract, not literal
     * way).
     *
     * @tparam T Type of filter data to carry
     */
    template <typename T>
    class Filter
    {
      protected:
        bool filterPresent = false;
        T filterData;

      public:
        /**
         * @brief Default constructor
         */
        Filter() noexcept = default;

        /**
         * @brief Construct a new Filter object
         *
         * @param filterData filter data to carry of type T
         * @param empty allows to mark if filterData data is empty in an abstract was (e.g. empty vector or a string)
         */
        Filter(T filterData, bool empty = false) : filterPresent(!empty), filterData(std::move(filterData))
        {}

        /**
         * @brief Filter data getter
         *
         * @return filter data
         */
        [[nodiscard]] const T &getFilterData() const noexcept
        {
            return filterData;
        }

        /**
         * @brief Checks if filter data is present
         *
         * @return true when the query filter is present, false otherwise
         */
        [[nodiscard]] bool isFilterPresent() const noexcept
        {
            return filterPresent;
        }
    };

    /**
     * @brief std::string specialization of db::query::Filter class
     *
     */
    class TextFilter : public Filter<std::string>
    {
      public:
        /**
         * @brief Default constructor
         *
         */
        TextFilter() = default;

        /**
         * @brief Construct a new Text Filter object. Marks filter as empty if string is empty ("")
         *
         * @param filterText filter text
         */
        TextFilter(const std::string &filterText) : Filter<std::string>(filterText, filterText.empty())
        {}
    };

    class ContactGroupFilter
    {
      protected:
        std::uint32_t filterData;

      public:
        /**
         * @brief Default constructor
         *
         */
        ContactGroupFilter() = default;

        /**
         * @brief Construct a new Text Filter object. Marks filter as empty if string is empty ("")
         *
         * @param groupText group filter text
         */
        ContactGroupFilter(std::uint32_t groupId) : filterData(groupId)
        {}

        [[nodiscard]] const std::uint32_t &getGroupFilterData() const noexcept
        {
            return filterData;
        }
    };

    class ContactDisplayMode
    {
      protected:
        std::uint32_t displayMode = 0;

      public:
        /**
         * @brief Default constructor
         *
         */
        ContactDisplayMode() = default;

        /**
         * @brief Construct a new Contact Display Mode object
         *
         * @param mode contact display mode
         */
        ContactDisplayMode(std::uint32_t mode) : displayMode(mode)
        {}

        [[nodiscard]] const std::uint32_t &getContactDisplayMode() const noexcept
        {
            return displayMode;
        }
    };

}; // namespace db::query
