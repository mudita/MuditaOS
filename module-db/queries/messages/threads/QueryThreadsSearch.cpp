#include "SMSTable.hpp"
#include "QueryThreadsSearch.hpp"

namespace db::query
{
    ThreadsSearch::ThreadsSearch(std::string text_to_search, unsigned int starting_position, unsigned int depth)
        : Query(Query::Type::Read), text(text_to_search), startingPosition(starting_position), depth(depth)
    {}

    auto ThreadsSearch::debugInfo() const -> std::string
    {
        return "SMSSearch";
    }

    ThreadsSearchResult::ThreadsSearchResult(unsigned int results_max_depth, std::vector<ThreadsTableRow> result_rows)
        : results_max_depth(results_max_depth), results(result_rows)
    {}

    auto ThreadsSearchResult::getMax() const -> unsigned int
    {
        return results_max_depth;
    }

    auto ThreadsSearchResult::getResults() const -> std::vector<ThreadsTableRow>
    {
        return results;
    }

    [[nodiscard]] auto ThreadsSearchResult::debugInfo() const -> std::string
    {
        return "SMSSearchResult";
    }
} // namespace db::query
