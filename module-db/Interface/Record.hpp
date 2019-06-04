
/*
 * @file Record.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_RECORD_HPP
#define PUREPHONE_RECORD_HPP

#include <memory>
#include "../Database/Database.hpp"

template<typename T, typename F>
class RecordInterface {
public:

    RecordInterface(){};

    virtual ~RecordInterface(){};

    virtual bool Add(const T&){};

    virtual bool RemoveByID(uint32_t id){};

    virtual bool Update(const T&){};

    virtual T GetByID(uint32_t id){};

    virtual uint32_t GetCount(){}

    virtual std::unique_ptr<std::vector<T>> GetLimitOffset(uint32_t offset,uint32_t limit){}

    virtual std::unique_ptr<std::vector<T>> GetLimitOffsetByField(uint32_t offset,uint32_t limit,F field, const char* str){}



};


#endif //PUREPHONE_RECORD_HPP
