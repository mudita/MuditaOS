// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "QueryResult.hpp"

QueryResult::QueryResult() : currentRow(0)
{}

void QueryResult::addRow(const std::vector<Field> &row)
{
    rows.push_back(row);
}

bool QueryResult::nextRow()
{
    ++currentRow;

    return (currentRow < rows.size());
}
