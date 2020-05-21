#pragma once

#include "../models/PhonebookModel.hpp"
#include "Label.hpp"
#include "ListItem.hpp"

namespace gui
{

    class PhonebookItem : public ListItem
    {
        gui::Label *value = nullptr;
        bool favourite    = false;

        void markFavourite(bool val);

      public:
        std::shared_ptr<ContactRecord> contact = nullptr;

        PhonebookItem();
        virtual ~PhonebookItem() = default;

        // sets copy of contact
        void setContact(std::shared_ptr<ContactRecord> note);
        void setMarkerItem(UTF8 text);
        UTF8 getLabelMarker();

        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
