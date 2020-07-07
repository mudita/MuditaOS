#include "ContactFlagIconWidget.hpp"

#include "ContactFlagsStyle.hpp"
#include "tools/Common.hpp"

namespace gui
{

    ContactFlagIconWidget::ContactFlagIconWidget(const std::string &iconName, const std::string &itemText, Item *parent)
        : Rect(parent, 0, 0, style::widget::ContatFlas::itemWidth, style::widget::ContatFlas::itemHeight),
          iconName(iconName), itemText(itemText)
    {
        buildItemWithIcon();
        buildItem();
    }

    ContactFlagIconWidget::ContactFlagIconWidget(uint8_t iconNumber, const std::string &itemText, Item *parent)
        : Rect(parent, 0, 0, style::widget::ContatFlas::itemWidth, style::widget::ContatFlas::itemHeight),
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
        icon =
            new Image(this, 0, 0, style::widget::ContatFlas::iconsSize, style::widget::ContatFlas::iconsSize, iconName);
        icon->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    }

    void ContactFlagIconWidget::buildItemWithText()
    {
        numericLabel =
            new Label(this, 0, 0, style::widget::ContatFlas::iconsSize, style::widget::ContatFlas::iconsSize);
        numericLabel->setFont(style::window::font::bigbold);
        numericLabel->setText(std::to_string(number));
        numericLabel->setAlignment({gui::Alignment::ALIGN_HORIZONTAL_CENTER | gui::Alignment::ALIGN_VERTICAL_CENTER});
        numericLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    }

    void ContactFlagIconWidget::buildItem()
    {
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        label = new Label(this, 0, 0, style::widget::ContatFlas::itemWidth, style::widget::ContatFlas::labelHeight);
        label->setFilled(false);
        label->setFont(style::window::font::verysmall);
        label->setText(itemText);
        label->setEllipsis(gui::Ellipsis::None);
        label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        label->setAlignment({gui::Alignment::ALIGN_VERTICAL_CENTER | gui::Alignment::ALIGN_HORIZONTAL_CENTER});

        mainBox = new VBox(this, 0, 0, style::widget::ContatFlas::itemWidth, style::widget::ContatFlas::itemHeight);
        mainBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        if (numericIcon) {
            mainBox->addWidget(numericLabel);
            center(mainBox, numericLabel, gui::Axis::X);
        }
        else {
            mainBox->addWidget(icon);
            center(mainBox, icon, gui::Axis::X);
        }
        mainBox->addWidget(label);
        center(mainBox, label, gui::Axis::X);
    }

} // namespace gui
