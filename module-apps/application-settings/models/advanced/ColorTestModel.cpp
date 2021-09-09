// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ColorTestModel.hpp"

#include <application-settings/widgets/advanced/ColorTestListStyle.hpp>

namespace gui
{

    ColorTestModel::ColorTestModel(app::ApplicationCommon *app) : application{app}
    {
        createData();
    }

    auto ColorTestModel::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    unsigned int ColorTestModel::getMinimalItemSpaceRequired() const
    {
        return style::colorTest::item::color::height;
    }

    gui::ListItem *ColorTestModel::getItem(gui::Order order)
    {
        return getRecord(order);
    }

    void ColorTestModel::requestRecords(const uint32_t offset, const uint32_t limit)
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    void ColorTestModel::createData()
    {
        internalData.clear();

        for (Color color = gui::ColorFullBlack; color.intensity != (gui::ColorFullWhite.intensity + 1);
             ++color.intensity) {
            auto newColorListItem          = new ColorTestListItem(application, color);
            newColorListItem->deleteByList = false;
            internalData.push_back(newColorListItem);
        }
    }

    gui::ColorScheme ColorTestModel::getColorScheme()
    {
        gui::ColorScheme scheme = gui::Color::defaultColorScheme;
        int i                   = 0;

        for (auto const &item : internalData) {
            scheme.intensity[i] = item->getColorIntensity();
            ++i;
        }

        return scheme;
    }
} // namespace gui
