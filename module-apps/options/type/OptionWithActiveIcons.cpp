// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionWithActiveIcons.hpp"
#include <widgets/TextWithIconsWidget.hpp>
#include <widgets/ActiveIconFactory.hpp>
#include <TextFixedSize.hpp>

using namespace gui::option;

namespace
{
    [[nodiscard]] auto makeIcon(gui::ActiveIconFactory &factory,
                                const std::shared_ptr<ContactRecord> &contact,
                                OptionWithActiveIcons::BasicIcon icon) -> gui::ImageBox *
    {
        switch (icon) {
        case OptionWithActiveIcons::BasicIcon::SendSMS:
            return factory.makeSMSIcon(contact->numbers.front().number);
        case OptionWithActiveIcons::BasicIcon::Call:
            return factory.makeCallIcon(contact->numbers.front().number);
        case OptionWithActiveIcons::BasicIcon::AddContact:
            return factory.makeAddContactIcon(contact);
        }
        return nullptr;
    }
} // namespace

OptionWithActiveIcons::OptionWithActiveIcons(app::Application *app,
                                             std::shared_ptr<ContactRecord> contact,
                                             std::vector<BasicIcon> icons)
    : app{app}, contact{std::move(contact)}, icons{std::move(icons)}
{}

auto OptionWithActiveIcons::build() const -> ListItem *
{
    auto optionItem = new gui::ListItem();
    optionItem->setMinimumSize(style::window::default_body_width, style::window::label::big_h);
    auto optionBodyHBox = new gui::TextWithIconsWidget(optionItem);
    const auto &number  = contact->numbers.front().number;
    optionBodyHBox->addText(!number.getE164().empty() ? number.getE164() : number.getFormatted(),
                            style::window::font::bigbold);

    ActiveIconFactory factory(app);
    for (auto icon : icons) {
        optionBodyHBox->addIcon(makeIcon(factory, contact, icon));
    }

    optionItem->dimensionChangedCallback = [optionBodyHBox](gui::Item &, const BoundingBox &newDim) -> bool {
        optionBodyHBox->setPosition(0, 0);
        optionBodyHBox->setSize(newDim.w, newDim.h);
        return true;
    };

    optionItem->inputCallback = [optionBodyHBox](Item &, const InputEvent &inputEvent) {
        return optionBodyHBox->onInput(inputEvent);
    };

    optionItem->focusChangedCallback = [optionItem, optionBodyHBox, this](gui::Item &item) -> bool {
        optionItem->setEdges(RectangleEdge::None);
        if (item.focus) {
            item.setFocusItem(optionBodyHBox);
        }
        else {
            item.setFocusItem(nullptr);
            app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode();
        }
        return true;
    };
    return optionItem;
}
