// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/DatabaseAgent.hpp>
#include <vector>

template <typename RecordType> struct PagedData
{
    std::vector<RecordType> data;
    unsigned int limit;
    unsigned int offset;
    unsigned int count;
};

template <typename PagedDataType, typename RecordType>
auto getList(std::unique_ptr<PagedDataType> pagedData, std::unique_ptr<QueryResult> query)
    -> std::unique_ptr<PagedDataType>
{
    if (nullptr == query || DatabaseAgent::ZERO_ROWS_FOUND == query->getRowCount()) {
        pagedData->count = 0;
        pagedData->data.clear();
        return pagedData;
    }
    pagedData->count   = query->getRowCount();
    unsigned int index = 0;

    // returns all records
    if (pagedData->limit == 0) {
        do {
            pagedData->data.push_back(RecordType(query.get()));
        } while (query->nextRow());
        return pagedData;
    }

    // validate offset against count
    if (pagedData->offset > pagedData->count) {
        pagedData->count = 0;
        return pagedData;
    }

    // navigate to the proper record specified by offset
    if (pagedData->offset > 0) {
        do {
            index++;
        } while (query->nextRow() && (index < pagedData->offset));
    }

    // add records to the list starting from offset and limited by limit
    do {
        pagedData->data.push_back(RecordType(query.get()));
        index++;
    } while (query->nextRow() && (index < pagedData->offset + pagedData->limit));

    // returns records in chunks (offset,limit)
    return pagedData;
}
