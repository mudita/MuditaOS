// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Record.hpp"
#include <Database/Database.hpp>

#include <functional>
#include <stdint.h>
#include <vector>

template <typename T, typename F>
class Table
{

  protected:
    explicit Table(Database *db) : db(db)
    {}

    virtual ~Table()
    {}

    virtual bool create()     = 0;
    virtual bool add(T entry) = 0;
    virtual bool removeAll()
    {
        return false;
    }
    virtual bool removeById(uint32_t id) = 0;
    virtual bool removeByField([[maybe_unused]] F field, [[maybe_unused]] const char *str)
    {
        return false;
    }
    virtual bool update(T entry)                                                                            = 0;
    virtual T getById(uint32_t id)                                                                          = 0;
    virtual std::vector<T> getLimitOffset(uint32_t offset, uint32_t limit)                                  = 0;
    virtual std::vector<T> getLimitOffsetByField(uint32_t offset, uint32_t limit, F field, const char *str) = 0;
    virtual uint32_t count()                                                                                = 0;
    virtual uint32_t countByFieldId(const char *field, uint32_t id)                                         = 0;

    uint32_t getLastInsertRowId()
    {
        return db->getLastInsertRowId();
    }

    virtual std::vector<T> retQueryUnpack(std::unique_ptr<QueryResult> retQuery) const
    {
        if ((retQuery == nullptr) || (retQuery->getRowCount() == 0) || (createTableRow == nullptr)) {
            return {};
        }

        std::vector<T> outVector;
        outVector.reserve(retQuery->getRowCount());

        do {
            outVector.push_back(createTableRow(*retQuery));
        } while (retQuery->nextRow());
        return outVector;
    }

    Database *db                                               = nullptr;
    std::function<T(const QueryResult &result)> createTableRow = nullptr;
};
