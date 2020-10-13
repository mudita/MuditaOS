
/*
 * @file Table.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 27.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#pragma once

#include <stdint.h>
#include <vector>
#include "module-db/Database/Database.hpp"

template <typename T, typename F> class Table
{

  protected:
    Table(Database *db) : db(db)
    {}

    virtual ~Table()
    {}

    virtual bool create()                = 0;
    virtual bool add(T entry)            = 0;
    virtual bool removeById(uint32_t id) = 0;
    virtual bool removeByField(F field, const char *str)
    {
        return true;
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

    Database *db;
};
