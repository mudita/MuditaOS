#pragma once

#include <string>

namespace db
{
    /// virtual query input interface
    class Query
    {
      public:
        virtual ~Query() = default;
        /// TODO make it pure virtual
        /// literally any debug info which might be helpful for debugging
        [[nodiscard]] virtual auto debugInfo() const -> std::string
        {
            return "";
        };
    };

    /// virtual query output (result) interface
    class QueryResult
    {
      public:
        virtual ~QueryResult() = default;
    };
} // namespace db
