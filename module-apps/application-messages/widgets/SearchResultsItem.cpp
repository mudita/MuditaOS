#include "SearchResultsItem.hpp"

#include <time/time_conversion.hpp>
#include <Style.hpp>

namespace gui
{
    SearchResultsItem::SearchResultsItem()
    {}

    bool SearchResultsItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        return true;
    }

    void SearchResultsItem::setTemplate(std::shared_ptr<SMSTemplateRecord> templ)
    {}

} /* namespace gui */
