#include "PukLockBox.hpp"

#include "../windows/PinLockBaseWindow.hpp"
#include "../data/AppDesktopStyle.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include <i18/i18.hpp>
#include <Style.hpp>

namespace lock_style = style::window::pin_lock;

namespace gui
{
    void PukLockBox::popChar(uint32_t charNum)
    {
        LockWindow->pinLabels[charNum]->clear();
    }
    void PukLockBox::putChar(uint32_t charNum)
    {
        LockWindow->pinLabels[charNum]->setText("*");
    }
    void PukLockBox::buildLockBox(unsigned int pinSize)
    {
        LockWindow->buildImages("pin_lock", "pin_lock_info");
        LockWindow->buildInfoText(lock_style::info_text_h_puk);
        buildPinLabels(pinSize);
    }
    void PukLockBox::buildPinLabels(unsigned int pinSize)
    {
        // labels with stars for displaying entered digits
        uint32_t pinLabelX           = lock_style::pin_label_x;
        const uint32_t pinLabelWidth = style::window_width - 2 * pinLabelX;
        LockWindow->pinLabel =
            new gui::Label(LockWindow, pinLabelX, lock_style::pin_label_y, pinLabelWidth, lock_style::label_size);
        LockWindow->pinLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

        LockWindow->buildPinLabels(LockWindow->pinLabel, pinSize, lock_style::label_size);
        for (auto label : LockWindow->pinLabels) {
            label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        }
    }
    void PukLockBox::setVisibleStateEnterPin()
    {
        LockWindow->clearPinLabels();
        LockWindow->pinLabel->setVisible(true);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_puk_lock1"));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(utils::localize.get("app_desktop_puk_lock2"));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(false, false, true);
    }
    void PukLockBox::setVisibleStateVerifiedPin()
    {
        LockWindow->clearPinLabels();
        LockWindow->pinLabel->setVisible(false);
        LockWindow->infoText->setVisible(false);
    }
    void PukLockBox::setVisibleStateInvalidPin(uint32_t remainingAttempts)
    {
        LockWindow->pinLabel->setVisible(false);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_puk_info1"));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_info2"));
        LockWindow->infoText->addText(" ");
        LockWindow->infoText->addText(std::to_string(remainingAttempts));
        LockWindow->infoText->addText(" ");

        if (remainingAttempts > 1) {
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_info3"));
        }
        else {
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_info4"));
            LockWindow->infoText->addText("\n\n");
            LockWindow->infoText->addText(utils::localize.get("app_desktop_puk_info3"));
            LockWindow->infoText->addText("\n");
            LockWindow->infoText->addText(utils::localize.get("app_desktop_puk_info4"));
            LockWindow->infoText->addText("\n");
            LockWindow->infoText->addText(utils::localize.get("app_desktop_puk_info5"));
        }
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(true, true, false);
    }
    void PukLockBox::setVisibleStatePhoneBlocked()
    {
        LockWindow->pinLabel->setVisible(false);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_puk_blocked1"));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(utils::localize.get("app_desktop_puk_blocked2"));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(true, false, false);
    }

} // namespace gui
