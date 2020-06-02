#pragma once

#include <module-gui/gui/widgets/ListItemProvider.hpp>
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
        /// Pointer to application that owns the model
        Application *application = nullptr;
        uint32_t recordsCount;
        std::vector<std::shared_ptr<T>> records;
        uint32_t modelIndex = 0;

      public:
        DatabaseModel(Application *app) : application{app}, recordsCount{0}
        {}

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

            records.clear();

            if (dbRecords != nullptr) {
                for (uint32_t i = 0; i < dbRecords->size(); i++) {
                    records.push_back(std::make_shared<T>(dbRecords.get()->operator[](i)));
                }

                return true;
            }
            else {
                LOG_ERROR("Null pointer received from DB");
                return false;
            }
        }

        virtual void clear()
        {
            records.clear();
            recordsCount = 0;
        }

        std::shared_ptr<T> getRecord(gui::Order order)
        {
            auto index = modelIndex;

            if ((index < 0) || (index >= records.size())) {
                return nullptr;
            }

            if (order == gui::Order::Previous) {
                index = records.size() - 1 - modelIndex;
            }

            auto item = records[index];

            modelIndex++;

            return item;
        }
    };

} /* namespace app */
