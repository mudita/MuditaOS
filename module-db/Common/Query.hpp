#pragma once

#include <string>

namespace db
{
    /// virtual query input interface
    class Query
    {
      public:
        virtual ~Query()                                            = default;
        [[nodiscard]] virtual auto debugInfo() const -> std::string = 0;
    };

    /// virtual query output (result) interface
    class QueryResult
    {
      public:
        virtual ~QueryResult()                                      = default;
        [[nodiscard]] virtual auto debugInfo() const -> std::string = 0;
    };
} // namespace db
