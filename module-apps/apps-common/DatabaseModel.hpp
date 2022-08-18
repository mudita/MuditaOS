// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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

            if (!dbRecords.empty()) {
                for (uint32_t i = 0; i < dbRecords.size(); i++) {
                    records.push_back(std::make_shared<T>(dbRecords[i]));
                }

                return true;
            }
            else {
                LOG_WARN("DB is empty");
                return false;
            }
        }

        void clear()
        {
            records.clear();
            recordsCount = 0;
        }

        std::shared_ptr<T> getRecord(gui::Order order)
        {
            auto index = 0;
            if (order == gui::Order::Next) {
                index = modelIndex;

                modelIndex++;
            }
            else if (order == gui::Order::Previous) {
                index = records.size() - 1 + modelIndex;

                modelIndex--;
            }

            if (isIndexValid(index)) {
                return records[index];
            }
            else {
                return nullptr;
            }
        }

        [[nodiscard]] bool isIndexValid(unsigned int index) const noexcept
        {
            return index < records.size();
        }
    };

} /* namespace app */
