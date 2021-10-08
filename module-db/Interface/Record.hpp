// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "../Database/Database.hpp"
#include "BaseInterface.hpp"

template <typename T, typename F> class RecordInterface : public db::Interface
{
  public:
    RecordInterface() = default;

    virtual ~RecordInterface() = default;

    virtual bool Add(const T &)
    {
        return true;
    };

    virtual bool Add(T &)
    {
        return true;
    };

    virtual bool RemoveAll()
    {
        return true;
    }

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
