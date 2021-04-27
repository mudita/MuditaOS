// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WindowsFactory.hpp"
#include <AppWindow.hpp>

#include <popups/VolumeWindow.hpp>
#include <popups/HomeModesWindow.hpp>
#include <popups/TetheringPhoneModePopup.hpp>
#include <popups/TetheringConfirmationPopup.hpp>

namespace app
{

    void WindowsFactory::attach(const std::string &name, builder builder)
    {
        builders[name] = builder;
    }

    void WindowsFactory::attachPopups(const std::vector<gui::popup::ID> &popupsList)
    {
        using namespace gui::popup;
        for (auto popup : popupsList) {
            switch (popup) {
            case ID::Volume:
                attach(window::volume_window, [](Application *app, const std::string &name) {
                  return std::make_unique<gui::VolumeWindow>(app, window::volume_window);
                });
                break;
            case ID::Tethering:
            case ID::TetheringPhoneModeChangeProhibited:
                attach(window::tethering_confirmation_window,
                                      [](Application *app, const std::string &name) {
                                        return std::make_unique<gui::TetheringConfirmationPopup>(
                                            app, window::tethering_confirmation_window);
                                      });
                attach(window::tethering_phonemode_change_window,
                                      [](Application *app, const std::string &name) {
                                        return std::make_unique<gui::TetheringPhoneModePopup>(
                                            app, window::tethering_phonemode_change_window);
                                      });
                break;
            case ID::PhoneModes:
                attach(window::phone_modes_window, [](Application *app, const std::string &name) {
                  return std::make_unique<gui::HomeModesWindow>(app, window::phone_modes_window);
                });
                break;
            case ID::Brightness:
                break;
            }
        }
    }

    auto WindowsFactory::isRegistered(const std::string &name) const -> bool
    {
        return builders.find(name) != std::end(builders);
    }

    auto WindowsFactory::build(Application *app, const std::string &name) -> handle
    {
        return builders[name](app, name);
    }
} // namespace app
