
/*
 * @file Table.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 27.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_TABLE_HPP
#define PUREPHONE_TABLE_HPP

#include <stdint.h>
#include <vector>
#include "Database/Database.hpp"

template <typename T, typename F>
class Table {

protected:

    Table(Database* db):db(db){}

    ~Table(){}

    virtual bool Create() = 0;
    virtual bool Add(T entry) = 0;
    virtual bool RemoveByID(uint32_t id) = 0;
    virtual bool RemoveByField(F field, const char* str){return true;}
    virtual bool Update(T entry) = 0;
    virtual T GetByID(uint32_t id) = 0;
    virtual std::vector<T> GetLimitOffset(uint32_t offset,uint32_t limit) = 0;
    virtual std::vector<T> GetLimitOffsetByField(uint32_t offset,uint32_t limit,F field,const char* str) = 0;
    virtual uint32_t GetCount() = 0;
    virtual uint32_t GetCountByFieldID(const char* field,uint32_t id) = 0;

    uint32_t GetLastInsertRowID(){return db->GetLastInsertRowID();}

    Database* db;

};


#endif //PUREPHONE_TABLE_HPP
