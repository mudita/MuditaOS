#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <algorithm>

#include "Application.hpp"

namespace app
{

    //    TODO: Remove template - nice to have
    template <class T> class DatabaseModel
    {
      protected:
        /// Number of elements requested from database in single message
        int pageSize = 1;
        /// Pointer to application that owns the model
        Application *application;
        /// Total number of records in database
        int recordsCount;
        /// vector that holds records received from database -
        std::vector<std::shared_ptr<T>> records;

      public:
        virtual void requestRecordsCount()
        {}

        virtual bool updateRecords(std::unique_ptr<std::vector<T>> dbRecords,
                                   const uint32_t offset,
                                   const uint32_t limit,
                                   uint32_t count)
        {

            auto maxElem = ((int)limit <= recordsCount ? limit : recordsCount);

            for (uint32_t i = 0; i <= maxElem; i++)
                records[i] = nullptr;

            records.clear();

            for (unsigned int i = 0; i < maxElem; i++) {
                auto ii     = i;
                records[ii] = nullptr;
                records[ii] = std::make_shared<T>(dbRecords.get()->operator[](i));
            }

            return true;
        }

        virtual ~DatabaseModel()
        {
            clear();
        }

        DatabaseModel(Application *app, int pageSize) : pageSize{pageSize}, application{app}, recordsCount{-1}
        {
            records.resize(pageSize * 3);
            std::fill(records.begin(), records.end(), nullptr);
        }

        virtual void clear()
        {

            for (int i = 0; i < pageSize * 3; i++)
                records[i] = nullptr;
            records.clear();
            recordsCount = -1;
        }

        std::shared_ptr<T> getRecord(int index)
        {

            // if index is greater than number of records or smaller than 0
            if ((index < 0) || (index > recordsCount - 1)) {
                return nullptr;
            }

            return records[index];
        }
    };

} /* namespace app */
