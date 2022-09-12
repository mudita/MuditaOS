// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SoundListItem.hpp"
#include <ListItem.hpp>
#include <TextFixedSize.hpp>

namespace app::bgSounds::sound_list_item
{
    constexpr auto margin             = 2;
    constexpr auto height             = 60U;
    constexpr auto default_text_width = 380;

    gui::ListItem *build(const db::multimedia_files::MultimediaFilesRecord &sound)
    {
        auto optionItem = new gui::ListItem();
        optionItem->setMinimumSize(default_text_width, height);
        optionItem->setMargins(gui::Margins(0, margin, 0, margin));
        optionItem->setAlignment(gui::Alignment::Horizontal::Center);

        auto optionBodyHBox = new gui::HBox(optionItem, 0, 0, 0, 0);
        optionBodyHBox->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        style::window::decorate(optionBodyHBox);

        auto optionText = new gui::TextFixedSize(optionBodyHBox, 0, 0, 0, 0);
        optionText->drawUnderline(false);
        optionText->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        optionText->setMaximumSize(default_text_width, height);
        optionText->setFont(style::window::font::largelight);
        optionText->setTextEllipsisType(gui::TextEllipsis::Right);
        optionText->setTextType(gui::TextType::SingleLine);
        optionText->setRichText(sound.tags.title);

        optionItem->dimensionChangedCallback = [optionBodyHBox](gui::Item &, const gui::BoundingBox &newDim) -> bool {
            optionBodyHBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        return optionItem;
    }
    std::uint32_t size()
    {
        return sound_list_item::height + 2 * sound_list_item::margin;
    }
} // namespace app::bgSounds::sound_list_item
