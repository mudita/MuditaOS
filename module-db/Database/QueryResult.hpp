
/*
 * @file QueryResult.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_QUERYRESULT_HPP
#define PUREPHONE_QUERYRESULT_HPP

#include <stdint.h>
#include <vector>
#include <memory>
#include "Field.hpp"

class QueryResult {

public:
    QueryResult();

    virtual ~QueryResult() {};


    const Field &operator[](int index) const { return rows[currentRow][index]; }

    bool NextRow();

    void AddRow(const std::vector<Field> &row);

    uint32_t GetFieldCount() const { return fieldCount; }

    uint32_t GetRowCount() const { return rowCount; }


private:
    uint32_t currentRow;
    std::vector<std::vector<Field>> rows;
    uint32_t fieldCount;
    uint32_t rowCount;

};


#endif //PUREPHONE_QUERYRESULT_HPP
