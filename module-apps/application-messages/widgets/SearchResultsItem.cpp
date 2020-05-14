#include "SearchResultsItem.hpp"

#include <time/time_conversion.hpp>
#include <Style.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    SearchResultsItem::SearchResultsItem()
    {
        layout = new VBox(this);

        title = new gui::Label(layout);
        title->setMaximumWidth(style::window_width);
        title->setText("Title");

        date = new gui::Label(this);
        date->setMinimumWidth(50);
        date->setFont(style::window::font::small);
        date->setText("00:00");

        preview = new gui::Label(this);
        preview->setFont(style::window::font::small);
        preview->setText("let the lol be with you!");
    }

    bool SearchResultsItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        setSize(newDim.w, newDim.h);
        layout->setSize(newDim.w, 60);
        preview->setSize(newDim.w, newDim.h - 60);
        preview->setPosition(0, 60);
        return true;
    }

    void SearchResultsItem::setTitle(const UTF8 &text)
    {
        title->setText(text);
    }
    void SearchResultsItem::setDate(const UTF8 &text)
    {
        date->setText(text);
    }
    void SearchResultsItem::setPreview(const UTF8 &text)
    {
        preview->setText(text);
    }

} /* namespace gui */
