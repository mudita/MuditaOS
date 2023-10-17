// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <stdint.h>
#include <vector>
#include <memory>
#include "Field.hpp"

class QueryResult
{

  public:
    QueryResult();

    virtual ~QueryResult(){};

    const Field &operator[](int index) const
    {
        return rows[currentRow][index];
    }

    bool nextRow();

    void addRow(const std::vector<Field> &row);

    uint32_t getFieldCount() const
    {
        return rows[currentRow].size();
    }

    uint32_t getRowCount() const
    {
        return rows.size();
    }

  private:
    uint32_t currentRow;
    std::vector<std::vector<Field>> rows;
};
