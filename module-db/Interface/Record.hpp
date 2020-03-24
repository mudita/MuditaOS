
/*
 * @file Record.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include <memory>
#include "../Database/Database.hpp"

template <typename T, typename F> class RecordInterface
{
  public:
    RecordInterface(){};

    virtual ~RecordInterface(){};

    virtual bool Add(const T &)
    {
        return true;
    };

    virtual bool RemoveByID(uint32_t id)
    {
        return true;
    };

    virtual bool RemoveByField(F field, const char *str)
    {
        return true;
    };

    virtual bool Update(const T &)
    {
        return true;
    };

    virtual T GetByID(uint32_t id)
    {
        return T{};
    };

    virtual uint32_t GetCount()
    {
        return 0;
    }

    virtual std::unique_ptr<std::vector<T>> GetLimitOffset(uint32_t offset, uint32_t limit)
    {
        return std::make_unique<std::vector<T>>();
    }

    virtual std::unique_ptr<std::vector<T>> GetLimitOffsetByField(uint32_t offset,
                                                                  uint32_t limit,
                                                                  F field,
                                                                  const char *str)
    {
        return std::make_unique<std::vector<T>>();
    }
};
