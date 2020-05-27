#include "QuerySMSSearch.hpp"

namespace db::query
{
    SMSSearch::SMSSearch(std::string text_to_search, unsigned int starting_position, unsigned int depth)
        : text(text_to_search), starting_postion(starting_position), depth(depth)
    {}

    auto SMSSearch::debugInfo() const -> std::string
    {
        return "SMSSearch";
    }

    SMSSearchResult::SMSSearchResult(unsigned int results_max_depth, std::vector<ThreadsTableRow> result_rows)
        : results_max_depth(results_max_depth), results(result_rows)
    {}

    auto SMSSearchResult::getMax() const -> unsigned int
    {
        return results_max_depth;
    }

    auto SMSSearchResult::getResults() const -> std::vector<ThreadsTableRow>
    {
        return results;
    }

    [[nodiscard]] auto SMSSearchResult::debugInfo() const -> std::string
    {
        return "SMSSearchResult";
    }
} // namespace db::query
