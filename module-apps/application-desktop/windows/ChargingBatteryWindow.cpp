// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ChargingBatteryWindow.hpp"
#include "Names.hpp"
#include "DeadBatteryInfoWindowData.hpp"

#include <gui/widgets/Image.hpp>
#include <service-appmgr/Controller.hpp>
#include <InputEvent.hpp>
#include <EventStore.hpp>
#include <FontManager.hpp>

namespace gui
{
    namespace
    {
        constexpr inline auto imgPositionX = 176;
        constexpr inline auto imgPositionY = 250;

        std::string getInfoText()
        {
            const auto &rawText     = utils::translate("app_desktop_too_low_battery_charging");
            const auto batteryLevel = static_cast<int>(Store::Battery::get().level);
            auto format             = TextFormat(FontManager::getInstance().getFont(style::window::font::medium));
            auto tokenMap           = text::RichTextParser::TokenMap({{"$VALUE", batteryLevel}});

            return text::RichTextParser().parse(rawText, &format, std::move(tokenMap))->getText();
        }
    } // namespace

    ChargingBatteryWindow::ChargingBatteryWindow(app::ApplicationCommon *app, const std::string &name)
        : AppWindow(app, name)
    {
        buildInterface();
        preventsAutoLock = true;
    }

    void ChargingBatteryWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void ChargingBatteryWindow::buildInterface()
    {
        AppWindow::buildInterface();
        navBar->setVisible(false);
        statusBar->setVisible(false);
        new gui::Image(this, imgPositionX, imgPositionY, 0, 0, "charging_battery_W_G");
    }

    void ChargingBatteryWindow::destroyInterface()
    {
        erase();
    }

    bool ChargingBatteryWindow::onInput([[maybe_unused]] const InputEvent &inputEvent)
    {
        auto data = std::make_unique<DeadBatteryInfoWindowData>(getInfoText());
        application->switchWindow(app::window::name::dead_battery_info, std::move(data));
        return true;
    }
} /* namespace gui */
