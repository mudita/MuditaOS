#include "ScreenLockBox.hpp"

#include "PinLockBaseWindow.hpp"
#include "application-desktop/widgets/PinLock.hpp"
#include "application-desktop/data/AppDesktopStyle.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include <i18/i18.hpp>

namespace lock_style = style::window::pin_lock;

namespace gui
{
    void ScreenLockBox::popChar(uint32_t charNum)
    {
        LockWindow->pinLabels[charNum]->clear();
    }
    void ScreenLockBox::putChar(uint32_t charNum)
    {
        LockWindow->pinLabels[charNum]->setText("*");
    }
    void ScreenLockBox::buildLockBox(unsigned int pinSize)
    {
        LockWindow->buildImages("pin_lock", "pin_lock_info");
        LockWindow->buildInfoText(lock_style::info_text_h_screen);
        ScreenLockBox::buildPinLabels(pinSize);
    }
    void ScreenLockBox::buildPinLabels(unsigned int pinSize)
    {
        // labels with stars for displaying entered digits
        const uint32_t pinLabelWidth = style::window_width - 2 * lock_style::pin_label_x_screen;
        LockWindow->pinLabel         = new gui::Label(
            LockWindow, lock_style::pin_label_x_screen, lock_style::pin_label_y, pinLabelWidth, lock_style::label_size);
        LockWindow->pinLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        LockWindow->buildPinLabels(LockWindow->pinLabel, pinSize, lock_style::label_size_screen);
        for (auto label : LockWindow->pinLabels) {
            label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        }
    }
    void ScreenLockBox::setVisibleStateEnterPin()
    {
        LockWindow->pinLabel->setVisible(true);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_screen_enter_passcode"));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(true, false, true);
    }
    void ScreenLockBox::setVisibleStateVerifiedPin()
    {
        LockWindow->clearPinLabels();
        LockWindow->pinLabel->setVisible(false);
        LockWindow->titleLabel->setVisible(false);
    }
    void ScreenLockBox::setVisibleStateInvalidPin()
    {
        LockWindow->clearPinLabels();
        LockWindow->pinLabel->setVisible(false);

        LockWindow->titleLabel->setVisible(true);
        LockWindow->titleLabel->setText(utils::localize.get("app_desktop_screen_wrong_pin"));

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get(utils::localize.get("app_desktop_screen_allowed_attempts")));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(std::to_string(LockWindow->lock.getRemainingAttempts()));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, true);
    }
    void ScreenLockBox::setVisibleStateBlocked()
    {
        LockWindow->pinLabel->setVisible(false);
        LockWindow->titleLabel->setVisible(false);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get(utils::localize.get("app_desktop_screen_blocked_info")));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, false);
    }
} // namespace gui
