// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/widgets/ListItemProvider.hpp>
#include <cstdint>
#include <vector>
#include <utility>
#include <algorithm>

#include <apps-common/ApplicationCommon.hpp>

namespace app
{
    template <class T>
    class DatabaseModel
    {
      protected:
        ApplicationCommon *application = nullptr;
        unsigned int recordsCount      = std::numeric_limits<unsigned int>::max();
        int modelIndex                 = 0;
        std::vector<std::shared_ptr<T>> records;

      public:
        explicit DatabaseModel(ApplicationCommon *app) : application{app}
        {}

        virtual ~DatabaseModel()
        {
            clear();
        }

        virtual bool updateRecords(std::vector<T> dbRecords)
        {
            modelIndex = 0;
            records.clear();

            assert(dbRecords.size() <= recordsCount);

            if (dbRecords.empty()) {
                LOG_INFO("DB is empty");
                return false;
            }

            for (const auto &dbRecord : dbRecords) {
                records.push_back(std::make_shared<T>(dbRecord));
            }
            return true;
        }

        void clear()
        {
            records.clear();
            recordsCount = 0;
        }

        std::shared_ptr<T> getRecord(gui::Order order)
        {
            int index = 0;

            switch (order) {
            case gui::Order::Next:
                index = modelIndex;
                modelIndex++;
                break;

            case gui::Order::Previous:
                index = records.size() - 1 + modelIndex;
                modelIndex--;
                break;

            default:
                break;
            }

            if (!isIndexValid(index)) {
                return nullptr;
            }
            return records[index];
        }

        [[nodiscard]] bool isIndexValid(unsigned int index) const noexcept
        {
            return index < records.size();
        }
    };
} /* namespace app */
