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
    template <typename T> class Filter
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

}; // namespace db::query
