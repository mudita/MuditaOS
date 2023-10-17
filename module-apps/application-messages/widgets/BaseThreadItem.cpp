// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BaseThreadItem.hpp"
#include "MessagesStyle.hpp"

namespace gui
{
    namespace
    {
        constexpr auto NumberImportancePrefix = '#';
    } // namespace

    BaseThreadItem::BaseThreadItem()
    {
        using namespace style;
        setMargins(Margins(0, style::margins::small, 0, style::margins::small));
        setMinimumSize(window::default_body_width, style::messages::threadItem::sms_thread_item_h);
        setMaximumSize(window::default_body_width, style::messages::threadItem::sms_thread_item_h);

        setRadius(0);
        setEdges(RectangleEdge::Bottom | RectangleEdge::Top);

        setPenFocusWidth(window::default_border_focus_w);
        setPenWidth(window::default_border_no_focus_w);

        contact = createEmptyLabel(this);
        contact->setFont(style::window::font::bigbold);
        contact->setTextEllipsisType(TextEllipsis::Right);
        contact->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});

        numberImportance = createEmptyLabel(this);
        numberImportance->setFont(style::window::font::small);
        numberImportance->setAlignment(
            gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});

        timestamp = createEmptyLabel(this);
        timestamp->setFont(style::window::font::small);
        timestamp->setTextEllipsisType(TextEllipsis::Right);
        timestamp->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center});

        snippetPrefix = createEmptyLabel(this);
        snippetPrefix->setFont(style::window::font::mediumlight);
        snippetPrefix->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});

        snippet = createEmptyLabel(this);
        snippet->setFont(style::window::font::medium);
        snippet->setTextEllipsisType(TextEllipsis::Right);
        snippet->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
    }

    gui::Label *BaseThreadItem::createEmptyLabel(Item *parent)
    {
        using namespace style;

        auto label = new gui::Label(parent, 0, 0, 0, 0);
        label->setPenFocusWidth(window::default_border_no_focus_w);
        label->setPenWidth(window::default_border_no_focus_w);
        return label;
    }

    void BaseThreadItem::onDimensionChangedTop(const BoundingBox & /*oldDim*/, const BoundingBox &newDim)
    {
        namespace msgStyle = style::messages::threadItem;

        contact->setPosition(msgStyle::leftMargin, msgStyle::topMargin);
        contact->setSize(newDim.w - msgStyle::cotactWidthOffset, newDim.h / 2 - msgStyle::topMargin);

        const auto isNumberImportanceSet = !numberImportance->getText().empty();
        if (isNumberImportanceSet) {
            contact->setSize(contact->getWidth() - msgStyle::numberImportanceWidth, Axis::X);
            numberImportance->setPosition(
                msgStyle::leftMargin + contact->getWidth() + msgStyle::numberImportanceLeftMargin, msgStyle::topMargin);
            numberImportance->setSize(msgStyle::numberImportanceWidth, newDim.h / 2 - msgStyle::topMargin);
        }

        timestamp->setPosition(newDim.w - msgStyle::timestampWidth, msgStyle::topMargin);
        timestamp->setSize(msgStyle::timestampWidth, newDim.h / 2 - msgStyle::topMargin);
    }

    void BaseThreadItem::onDimensionChangedBottom(const BoundingBox & /*oldDim*/, const BoundingBox &newDim)
    {
        resizeSnippet(newDim);
    }

    void BaseThreadItem::resizeSnippet(const BoundingBox &dimensions, unsigned int leftOffset)
    {
        namespace msgStyle = style::messages::threadItem;

        const auto leftMargin = msgStyle::leftMargin + leftOffset;
        if (const auto isPrefixSet = !snippetPrefix->getText().empty(); isPrefixSet) {
            snippetPrefix->setPosition(leftMargin, dimensions.h / 2);
            snippetPrefix->setSize(snippetPrefix->getTextNeedSpace(), dimensions.h / 2 - msgStyle::bottomMargin);

            const auto prefixSpace = snippetPrefix->getWidth() + msgStyle::snippetLeftMargin;
            snippet->setPosition(leftMargin + prefixSpace, dimensions.h / 2);
            snippet->setSize(dimensions.w - msgStyle::previewWidthOffset - prefixSpace - leftOffset,
                             dimensions.h / 2 - msgStyle::bottomMargin);
        }
        else {
            snippetPrefix->setPosition(0, 0);
            snippetPrefix->setSize(0, 0);
            snippet->setPosition(leftMargin, dimensions.h / 2);
            snippet->setSize(dimensions.w - msgStyle::previewWidthOffset - leftOffset,
                             dimensions.h / 2 - msgStyle::bottomMargin);
        }
    }

    bool BaseThreadItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        onDimensionChangedTop(oldDim, newDim);
        onDimensionChangedBottom(oldDim, newDim);
        return true;
    }

    void BaseThreadItem::displayNumberImportance(long int id)
    {
        numberImportance->setText(NumberImportancePrefix + std::to_string(id));
    }
} // namespace gui
