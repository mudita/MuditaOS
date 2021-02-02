// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-desktop/data/AppDesktopStyle.hpp"
#include "application-desktop/widgets/PinLock.hpp"
#include <i18n/i18n.hpp>
#include "PinLockBaseWindow.hpp"

namespace lock_style = style::window::pin_lock;

namespace gui
{
    void PinLockBaseWindow::buildImages(const std::string &lockImg, const std::string &infoImg)
    {
        lockImage = new gui::Image(this, lock_style::image::x, lock_style::image::y, 0, 0, lockImg);
        infoImage = new gui::Image(this, lock_style::image::x, lock_style::image::y, 0, 0, infoImg);
    }

    auto PinLockBaseWindow::getToken(Token token) const -> std::string
    {
        if (token == Token::Sim) {
            return "$SIM";
        }
        else if (token == Token::Attempts) {
            return "$ATTEMPTS";
        }
        else if (token == Token::Mins) {
            return "$MINUTES";
        }
        else if (token == Token::CmeCode) {
            return "$CMECODE";
        }
        return std::string{};
    }
    top_bar::Configuration PinLockBaseWindow::configureTopBar(top_bar::Configuration appConfiguration)
    {
        appConfiguration.enable(top_bar::Indicator::Lock);
        appConfiguration.disable(top_bar::Indicator::Time);
        return appConfiguration;
    }
    void PinLockBaseWindow::restore() noexcept
    {
        LockWindow::restore();
        lockImage->setVisible(false);
        infoImage->setVisible(false);
    }

    void PinLockBaseWindow::setImagesVisible(bool lockImg, bool infoImg)
    {
        lockImage->setVisible(lockImg);
        infoImage->setVisible(infoImg);
    }

    void PinLockBaseWindow::setTitleBar(bool isVisible, bool isIceActive)
    {
        LockWindow::setTitleBar(isVisible);
    }

    void PinLockBaseWindow::buildBottomBar()
    {
        LockWindow::buildBottomBar();
        setBottomBarWidgetsActive(false, false, false);
    }

    void PinLockBaseWindow::buildTitleBar()
    {
        using namespace style::window::pin_lock;

        auto arrow        = new gui::Image("left_label_arrow");
        arrow->activeItem = false;
        arrow->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        arrow->setMargins(Margins(0, 0, ice::margin, 0));

        title = new gui::Text(this, title::x, title::y, title::w, title::h);
        title->setFilled(false);
        title->setBorderColor(gui::ColorFullBlack);
        title->setFont(style::header::font::title);
        title->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        title->setVisible(false);
        title->setPenWidth(2);
    }

} // namespace gui
