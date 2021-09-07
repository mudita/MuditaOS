// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/DatabaseAgent.hpp>
#include <vector>

template <typename RecordType> struct PagedData
{
    std::vector<RecordType> data;
    unsigned int count;
};

template <typename PagedDataType, typename RecordType>
auto getList(unsigned int offset, unsigned int limit, std::shared_ptr<QueryResult> query)
    -> std::unique_ptr<PagedDataType>
{
    auto pagedData = std::make_unique<PagedDataType>();
    if (nullptr == query || DatabaseAgent::ZERO_ROWS_FOUND == query->getRowCount()) {
        pagedData->count = 0;
        pagedData->data.clear();
        return pagedData;
    }
    pagedData->count = query->getRowCount();

    // returns all records
    if (limit == 0) {
        do {
            pagedData->data.push_back(RecordType(query.get()));
        } while (query->nextRow());
        return pagedData;
    }

    // validate offset against count
    if (offset > pagedData->count) {
        pagedData->count = 0;
        return pagedData;
    }

    unsigned int index = 0;
    // navigate to the proper record specified by offset
    if (offset > 0) {
        do {
            index++;
        } while (query->nextRow() && (index < offset));
    }

    // add records to the list starting from offset and limited by limit
    do {
        pagedData->data.push_back(RecordType(query.get()));
        index++;
    } while (query->nextRow() && (index < offset + limit));

    // returns records in chunks (offset,limit)
    return pagedData;
}
