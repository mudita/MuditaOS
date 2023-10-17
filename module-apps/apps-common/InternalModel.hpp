// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/widgets/ListItemProvider.hpp>
#include <vector>

namespace app
{

    template <class T>
    class InternalModel
    {

      protected:
        int modelIndex              = 0;
        unsigned int internalOffset = 0;
        unsigned int internalLimit  = 0;
        std::vector<T> internalData;

        virtual ~InternalModel()
        {
            eraseInternalData();
        }

        void eraseInternalData()
        {
            for (auto item : internalData) {
                delete item;
            }
            internalData.clear();
        }

        void setupModel(const uint32_t offset, const uint32_t limit)
        {
            modelIndex     = 0;
            internalOffset = offset;
            internalLimit  = limit;
        }

        T getRecord(gui::Order order)
        {
            auto index = 0;
            if (order == gui::Order::Previous) {
                auto calculatedIndex = internalOffset + internalLimit >= internalData.size()
                                           ? internalData.size()
                                           : internalOffset + internalLimit;

                index = calculatedIndex - 1 + modelIndex;
                modelIndex--;
            }
            if (order == gui::Order::Next) {
                index = internalOffset + modelIndex;

                modelIndex++;
            }

            return getInternalDataElement(index, order);
        }

        [[nodiscard]] bool isIndexValid(unsigned int index, gui::Order order) const noexcept
        {
            return (order == gui::Order::Next && index < internalData.size()) ||
                   (order == gui::Order::Previous && static_cast<int>(index) >= 0 && index < internalData.size() &&
                    index >= internalOffset);
        }

        void clearItemProperties(T Item)
        {
            Item->setFocus(false);
            Item->setVisible(true);
            Item->clearNavigationItem(gui::NavigationDirection::UP);
            Item->clearNavigationItem(gui::NavigationDirection::DOWN);
        }

        [[nodiscard]] T getInternalDataElement(unsigned int index, gui::Order order)
        {
            if (isIndexValid(index, order)) {
                clearItemProperties(internalData[index]);
                return internalData[index];
            }
            else {
                return nullptr;
            }
        }
    };

} /* namespace app */
