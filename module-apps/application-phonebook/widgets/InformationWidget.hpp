#pragma once

#include "application-phonebook/data/PhonebookInternals.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"
#include "NumberWithIconsWidget.hpp"

#include <ListItem.hpp>
#include <Text.hpp>

namespace gui
{
    class InformationWidget : public ContactListItem
    {
      public:
        InformationWidget(app::Application *app);
        ~InformationWidget() override = default;
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;
        VBox *vBox                                   = nullptr;
        Label *titleLabel                            = nullptr;
        NumberWithIconsWidget *primaryNumberHBox     = nullptr;
        NumberWithIconsWidget *secondNumberHBox      = nullptr;
        Text *emailText                              = nullptr;
        Item *savedFocusItem                         = nullptr;
    };

} /* namespace gui */
