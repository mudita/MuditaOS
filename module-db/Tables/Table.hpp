
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

template <typename T>
class Table {

protected:

    virtual bool Add(T entry) = 0;
    virtual bool Remove(T entry) = 0;
    virtual bool RemoveByID(uint32_t id) = 0;
    virtual bool Update(T entry) = 0;
    virtual T GetByID(uint32_t id) = 0;
    virtual std::vector<T> GetLimitOffset(uint32_t offset,uint32_t limit) = 0;
    virtual uint32_t GetCount() = 0;

};


#endif //PUREPHONE_TABLE_HPP
