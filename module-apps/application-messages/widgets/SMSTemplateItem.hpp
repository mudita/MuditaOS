#pragma once

#include "SMSTemplateModel.hpp"

#include <Label.hpp>
#include <Image.hpp>
#include <ListItem.hpp>

namespace gui
{
    namespace smsTemplItemStyle
    {
        constexpr uint32_t w = style::window::default_body_width;
        constexpr uint32_t h = style::window::label::big_h;
    } // namespace smsTemplItemStyle

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
