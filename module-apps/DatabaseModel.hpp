#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <algorithm>

#include "Application.hpp"

namespace app
{

    template <class T> class DatabaseModel
    {
      protected:
        static constexpr auto defBuffSize = 12;

        /// Receive messages buffer size
        int buffSize = defBuffSize;
        /// Pointer to application that owns the model
        Application *application = nullptr;
        int recordsCount;
        std::vector<std::shared_ptr<T>> records;

      public:
        DatabaseModel(Application *app, int buffSize = defBuffSize)
            : buffSize{buffSize}, application{app}, recordsCount{-1}
        {
            resizeBuffer(buffSize);
        }

        virtual ~DatabaseModel()
        {
            clear();
        }

        virtual void requestRecordsCount()
        {}

        virtual bool updateRecords(std::unique_ptr<std::vector<T>> dbRecords,
                                   const uint32_t offset,
                                   const uint32_t limit,
                                   uint32_t count)
        {
            for (auto i = 0; i < buffSize; i++)
                records[i] = nullptr;

            auto maxElem = (static_cast<int>(limit) <= buffSize ? limit : buffSize);

            for (uint32_t i = 0; i < maxElem; i++) {
                records[i] = std::make_shared<T>(dbRecords.get()->operator[](i));
            }

            return true;
        }

        virtual void resizeBuffer(int buffSize)
        {
            records.resize(buffSize);
            std::fill(records.begin(), records.end(), nullptr);
        }

        virtual void clear()
        {
            std::fill(records.begin(), records.end(), nullptr);
            records.clear();
            recordsCount = -1;
        }

        std::shared_ptr<T> getRecord(int index)
        {

            if ((index < 0) || (index > buffSize)) {
                return nullptr;
            }

            return records[index];
        }
    };

} /* namespace app */
