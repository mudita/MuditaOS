
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

template<typename T>
class RecordInterface {
public:

    RecordInterface(){};

    virtual ~RecordInterface(){};

    virtual bool Add(const T&){};

    virtual bool RemoveByID(uint32_t id){};

    virtual bool RemoveByName(const char* str){};

    virtual bool Update(const T&){};

    virtual T GetByID(uint32_t id){};

    virtual T GetByName(const char* str){};

    virtual uint32_t GetCount(){}

    virtual std::unique_ptr<std::vector<T>> GetLimitOffset(uint32_t offset,uint32_t limit){}



};


#endif //PUREPHONE_RECORD_HPP
