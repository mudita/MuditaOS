#pragma once

#include "SMSTemplateModel.hpp"

#include <Label.hpp>
#include <Image.hpp>
#include <ListItem.hpp>

namespace gui
{
    class SMSTemplateItem : public ListItem
    {
        std::shared_ptr<SMSTemplateRecord> templ = nullptr;
        gui::Label *text                         = nullptr;

      public:
        SMSTemplateItem();
        virtual ~SMSTemplateItem() = default;
        void setTemplate(std::shared_ptr<SMSTemplateRecord>);

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
