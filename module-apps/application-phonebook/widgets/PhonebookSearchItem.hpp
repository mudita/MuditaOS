#pragma once

#include "../models/PhonebookModel.hpp"
#include "Label.hpp"
#include "ListItem.hpp"

namespace gui
{

/*
 *
 */
class PhonebookSearchItem : public ListItem
{
    // pointer to the contact record
    std::shared_ptr<ContactRecord> contact = nullptr;
    // this is contact's name and surname or "favourites" or "first letter of surname
    gui::Label *value = nullptr;
    bool favourite = false;

  public:
    PhonebookSearchItem();
    virtual ~PhonebookSearchItem();
    // sets copy of contact
    void setContact(std::shared_ptr<ContactRecord> note);
    void setValue(UTF8 text);
    UTF8 getValue();
    /**
     * Marks item as a favourite. This will cause to use bold font instead of normal font.
     */
    void markFavourite(bool val);

    // returns contact's structure
    std::shared_ptr<ContactRecord> getContact();

    // virtual methods from Item
    bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    bool onActivated(void *data) override;
};

} /* namespace gui */
