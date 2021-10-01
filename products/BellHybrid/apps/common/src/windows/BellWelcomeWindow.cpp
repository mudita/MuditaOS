// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "windows/BellWelcomeWindow.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>

#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/TextFixedSize.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    BellWelcomeWindow::BellWelcomeWindow(app::ApplicationCommon *app, const std::string &name, Callback onAction)
        : AppWindow(app, name), onAction{onAction}
    {
        buildInterface();
    }

    void BellWelcomeWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height, false);

        auto muditaLogo = new ImageBox(body->firstBox, new Image("bell_mudita_logo", ImageTypeSpecifier::W_G));
        muditaLogo->setMinimumSizeToFitImage();

        auto welcomeText = new TextFixedSize(body->centerBox);
        welcomeText->setMinimumSize(style::bell_base_layout::w,
                                    style::bell_base_layout::center_layout_h - midline_correction);
        welcomeText->setFont(style::window::font::largelight);
        welcomeText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        welcomeText->drawUnderline(false);
        welcomeText->setText(utils::translate("app_bell_onboarding_welcome_message"));

        auto midLine = new Rect(body->centerBox, 0, 0, 0, 0);
        midLine->setMinimumSize(midline_w, midline_h);
        midLine->setEdges(RectangleEdge::Bottom);

        auto informationBody = new HBox(body->lastBox);
        informationBody->setMinimumSize(style::bell_base_layout::outer_layouts_w,
                                        style::bell_base_layout::outer_layouts_h);
        informationBody->setEdges(RectangleEdge::None);
        informationBody->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));

        auto sunLogo = new ImageBox(informationBody, new Image("button_icon_sun", ImageTypeSpecifier::W_G));
        sunLogo->setMinimumSizeToFitImage();
        sunLogo->setMargins(Margins(0, sun_logo_margin_top, sun_logo_margin_right, 0));

        auto instructionText = new TextFixedSize(informationBody);
        instructionText->setMargins(Margins(0, sun_logo_margin_top, 0, 0));
        instructionText->setFont(style::window::font::largelight);
        instructionText->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        instructionText->drawUnderline(false);
        instructionText->setFont(style::window::font::verybiglight);
        // non localized string by purpose
        instructionText->setText(": ON");
        instructionText->setMinimumWidthToFitText(instructionText->getText());
        instructionText->setMinimumHeight(sunLogo->widgetMinimumArea.h);

        body->resize();
    }

    bool BellWelcomeWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isKeyRelease(KeyCode::KEY_LF)) {
            if (onAction) {
                onAction();
            }
        }
        return false;
    }

} /* namespace gui */
