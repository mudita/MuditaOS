#pragma once

#include <module-gui/gui/widgets/ListItemProvider.hpp>
#include <vector>

#include "Application.hpp"

namespace app
{

    template <class T> class InternalModel
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

        gui::ListItem *getRecord(gui::Order order)
        {
            unsigned int index = 0;
            if (order == gui::Order::Previous) {
                index = internalLimit - modelIndex - 1;
            }
            if (order == gui::Order::Next) {
                index = internalOffset + modelIndex;
            }

            if (isValidIndex(index, order)) {

                return getNextInternalDataElement(index);
            }
            else {

                return nullptr;
            }
        }

        [[nodiscard]] bool isValidIndex(unsigned int index, gui::Order order) const
        {
            return (index < internalData.size()) || (order == gui::Order::Previous && index < internalOffset);
        }

        gui::ListItem *getNextInternalDataElement(unsigned int index)
        {
            modelIndex++;
            internalData[index]->setVisible(true);
            return internalData[index];
        }
    };

} /* namespace app */
