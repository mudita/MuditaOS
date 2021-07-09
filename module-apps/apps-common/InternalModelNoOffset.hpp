// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/widgets/ListItemProvider.hpp>
#include <vector>

namespace app
{

    template <class T> class InternalModelNoOffset
    {

      protected:
        int modelIndex = 0;
        std::vector<T> internalData;

        virtual ~InternalModelNoOffset()
        {
            eraseInternalData();
        }

        void eraseInternalData()
        {
            for (auto item : internalData) {
                delete item;
            }
            internalData.clear();
            modelIndex = 0;
        }

        T getRecord(gui::Order order)
        {
            auto queryIndex = 0;
            auto newIndex   = 0;
            if (order == gui::Order::Previous) {
                queryIndex = modelIndex - 1;
                newIndex   = modelIndex - 1;
            }
            if (order == gui::Order::Next) {
                queryIndex = modelIndex;
                modelIndex++;
                newIndex = modelIndex + 1;
            }
            auto element = getInternalDataElement(queryIndex, order);
            if (element) {
                modelIndex = newIndex;
            }
            return element;
        }

        unsigned int getIndex()
        {
            return modelIndex;
        }

        [[nodiscard]] bool isIndexValid(unsigned int index, gui::Order order) const noexcept
        {
            return (index < internalData.size());
        }

        void clearItemProperties(T &Item)
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
