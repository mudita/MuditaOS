// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * @file QueryResult.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "QueryResult.hpp"

QueryResult::QueryResult() : currentRow(0), fieldCount(0), rowCount(0)
{}

void QueryResult::addRow(const std::vector<Field> &row)
{
    rows.push_back(row);
    rowCount++;
    fieldCount = row.size();
}

bool QueryResult::nextRow()
{
    if (currentRow < (rowCount - 1)) {
        currentRow++;
        return true;
    }
    else {
        return false;
    }
}