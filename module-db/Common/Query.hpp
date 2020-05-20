#pragma once

#include <string>

namespace db
{
    /// virtual query input interface
    class Query
    {
      public:
        virtual ~Query() = default;
        [[nodiscard]] virtual auto debugInfo() const -> std::string
        {
            return "Base Query";
        };
    };

    /// virtual query output (result) interface
    class QueryResult
    {
      public:
        virtual ~QueryResult() = default;
    };
} // namespace db
