// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "windows/BellWelcomeWindow.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>

#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/text/TextFixedSize.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    BellWelcomeWindowBase::BellWelcomeWindowBase(app::ApplicationCommon *app,
                                                 const std::string &name,
                                                 const std::string &icon,
                                                 const std::string &message,
                                                 Callback onAction)
        : AppWindow(app, name), icon(icon), message(message), onAction{onAction}
    {
        buildInterface();
    }

    void BellWelcomeWindowBase::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height, false);

        auto muditaLogo = new ImageBox(body->firstBox, new Image(this->icon, ImageTypeSpecifier::W_G));
        muditaLogo->setMinimumSizeToFitImage();

        auto welcomeText = new TextFixedSize(body->centerBox);
        welcomeText->setMinimumSize(style::bell_base_layout::center_layout_w,
                                    style::bell_base_layout::center_layout_h - midline_correction);
        welcomeText->setFont(style::window::font::largelight);
        welcomeText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        welcomeText->drawUnderline(false);
        welcomeText->setRichText(utils::translate(this->message));

        auto midLine = new Rect(body->centerBox, 0, 0, 0, 0);
        midLine->setMinimumSize(midline_w, midline_h);
        midLine->setEdges(RectangleEdge::Bottom);

        auto informationBody = new HBox(body->lastBox);
        informationBody->setMinimumSize(style::bell_base_layout::outer_layouts_w,
                                        style::bell_base_layout::outer_layouts_h);
        informationBody->setEdges(RectangleEdge::None);
        informationBody->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));

        auto bottomLogo = new ImageBox(informationBody, new Image("bell_off_bottom", ImageTypeSpecifier::W_G));
        bottomLogo->setMinimumSizeToFitImage();
        bottomLogo->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        bottomLogo->setMargins(Margins(0, 0, 0, 0));

        body->resize();
    }

    bool BellWelcomeWindowBase::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isKeyRelease(KeyCode::KEY_ENTER)) {
            if (onAction) {
                onAction();
            }
        }
        return false;
    }

    BellWelcomeWindow::BellWelcomeWindow(app::ApplicationCommon *app, Callback onAction)
        : BellWelcomeWindowBase(app, BellWelcomeWindow::name, "bell_mudita_logo", "app_bell_welcome_message", onAction)
    {}

    BellChargeWelcomeWindow::BellChargeWelcomeWindow(app::ApplicationCommon *app, Callback onAction)
        : BellWelcomeWindowBase(
              app, BellChargeWelcomeWindow::name, "bell_welcome_battery", "app_bell_welcome_charge_message", onAction)
    {}

} /* namespace gui */
