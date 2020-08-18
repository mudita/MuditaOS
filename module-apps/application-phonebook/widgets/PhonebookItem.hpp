#pragma once

#include "Interface/ContactRecord.hpp"
#include "Label.hpp"
#include "Image.hpp"
#include "ListItem.hpp"
#include <BoxLayout.hpp>
#include <TextFixedSize.hpp>

namespace gui
{

    class PhonebookItem : public ListItem
    {
        gui::Label *contactName = nullptr;
        gui::HBox *hBox         = nullptr;

        bool favourite = false;
        void markFavourite(bool val);
        void markBlocked(bool val);

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
