#pragma once

#include <string>

namespace db
{
    /// virtual query input interface
    class Query
    {
      public:
        enum class Type
        {
            Create,
            Read,
            Update,
            Delete
        };

        Query(Type type) : type(type)
        {}
        virtual ~Query() = default;

        const Type type;

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
