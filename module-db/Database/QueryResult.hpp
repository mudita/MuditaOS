// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * @file QueryResult.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

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
        return fieldCount;
    }

    uint32_t getRowCount() const
    {
        return rowCount;
    }

  private:
    uint32_t currentRow;
    std::vector<std::vector<Field>> rows;
    uint32_t fieldCount;
    uint32_t rowCount;
};
