#include "ContactFlagIconWidget.hpp"

#include "ContactFlagsStyle.hpp"
#include "tools/Common.hpp"

namespace gui
{

    ContactFlagIconWidget::ContactFlagIconWidget(const std::string &iconName, const std::string &itemText, Item *parent)
        : Rect(parent, 0, 0, style::widget::ContactFlag::itemWidth, style::widget::ContactFlag::itemHeight),
          iconName(iconName), itemText(itemText)
    {
        buildItemWithIcon();
        buildItem();
    }

    ContactFlagIconWidget::ContactFlagIconWidget(uint8_t iconNumber, const std::string &itemText, Item *parent)
        : Rect(parent, 0, 0, style::widget::ContactFlag::itemWidth, style::widget::ContactFlag::itemHeight),
          number(iconNumber), itemText(itemText), numericIcon(true)
    {
        buildItemWithText();
        buildItem();
    }

    void ContactFlagIconWidget::setIconNumber(uint8_t iconNumber)
    {
        if ((numericLabel != nullptr) && (number != iconNumber)) {
            number = iconNumber;
            numericLabel->setText(std::to_string(number));
        }
    }

    void ContactFlagIconWidget::buildItemWithIcon()
    {
        icon = new Image(
            this, 0, 0, style::widget::ContactFlag::iconsSize, style::widget::ContactFlag::iconsSize, iconName);
        icon->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    }

    void ContactFlagIconWidget::buildItemWithText()
    {
        numericLabel =
            new Label(this, 0, 0, style::widget::ContactFlag::iconsSize, style::widget::ContactFlag::iconsSize);
        numericLabel->setFont(style::window::font::bigbold);
        numericLabel->setText(std::to_string(number));
        numericLabel->setAlignment(Alignment(Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        numericLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    }

    void ContactFlagIconWidget::buildItem()
    {
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        label = new Label(this, 0, 0, style::widget::ContactFlag::itemWidth, style::widget::ContactFlag::labelHeight);
        label->setFilled(false);
        label->setFont(style::window::font::verysmall);
        label->setText(itemText);
        label->setEllipsis(gui::Ellipsis::None);
        label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        label->setAlignment(Alignment(Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        mainBox = new VBox(this, 0, 0, style::widget::ContactFlag::itemWidth, style::widget::ContactFlag::itemHeight);
        mainBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        mainBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        if (numericIcon) {
            mainBox->addWidget(numericLabel);
        }
        else {
            mainBox->addWidget(icon);
        }
        mainBox->addWidget(label);
    }

} // namespace gui
