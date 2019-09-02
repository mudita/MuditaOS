#include "ContactInputBox.hpp"

namespace gui {

ContactInputBox::Input::Input(ContactInputBox *parent, const UTF8 &text, const UTF8 &val) {
    if (!parent)
        return;
    const unsigned int wall_offset = 10;
    iname = new gui::Label();
    iname->setMaxSize(parent->getMaxWidth() - wall_offset * 2, 40);
    iname->setPosition(wall_offset, 0);
    iname->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    iname->setText(text);
    iname->setFont("gt_pressura_bold_16");
    iname->setPenFocusWidth(3);
    iname->setPenWidth(1);
    parent->addWidget(iname);

    input = new gui::Label();
    input->setMaxSize(parent->getMaxWidth() - wall_offset * 2, 40);
    input->setPosition(wall_offset, 0);
    input->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
    input->setPenWidth(3);
    input->setFont("gt_pressura_regular_16");
    input->setPenFocusWidth(3);
    input->setPenWidth(1);
    input->focusChangedCallback = parent->parentSetCallback();
    input->setText(val);
    parent->addWidget(input);
}

ContactInputBox::ContactInputBox(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h)
    : VBox(parent, x, y, w, h), focus_cnt(0) {
    focusChangedCallback = [](Item &) {
        return true;
    };
}

ContactInputBox::~ContactInputBox() {}

bool DebugLabel::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) {
    // TODO this is left for debug of VBox/HBox
    return Label::onDimensionChanged(oldDim, newDim);
}

/// offsetting rectangle - it moves element in it by offset. This way element resized by VBox/HBox will have set offset
class OffsetRect : public Rect {
    static const uint32_t default_offset = 2;
    uint32_t off;

  public:
    OffsetRect() : off(default_offset) {}
    OffsetRect(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, uint32_t off = 2)
        : Rect(parent, x, y, w, h), off(off) {}
    virtual ~OffsetRect() {}
    void setOff(uint32_t off) { this->off = off; }
    bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) {
        for (auto element : children) {
            element->setSize(widgetArea.w - 2 * off, widgetArea.h - 2 * off);
            element->setPosition(off, off);
        }
        return Rect::onDimensionChanged(oldDim, newDim);
    }
};

ContactInputBox::Option::Option(ContactInputBox *parent, const UTF8 &text, const UTF8 &val)
    : hbox(nullptr), name_label(nullptr), input_label(nullptr), x2(0) {
    if (!parent)
        return;
    if (!hbox) {
        hbox = new HBox();
        hbox->setMaxSize(parent->getMaxWidth(), 50);
        hbox->setSize(parent->widgetArea.w, 40);
        hbox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    }

    // TODO - VBox doesn't work well with size == maxsize -> size of elements must be maxize-1
    x2 = hbox->widgetArea.w - x1 - x3 - 1;

    auto *element = new OffsetRect();
    element->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    element->setMaxSize(x1, 40);

    auto first_label = new DebugLabel("B1");
    first_label->setRadius(15);
    element->addWidget(first_label);
    boxItems.push_back(element);

    name_label = new DebugLabel("B2");
    name_label->setMaxSize(x2, parent->getMaxHeight());
    name_label->setText(text);
    name_label->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER));
    name_label->setMargins(gui::Margins(16, 0, 0, 0));
    name_label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    boxItems.push_back(name_label);

    element = new OffsetRect();
    element->setMaxSize(x3, 40);

    input_label = new DebugLabel("B1");
    input_label->setRadius(11);
    element->addWidget(input_label);
    input_label->setPenFocusWidth(3);
    input_label->setPenWidth(1);
    input_label->setText(val);
    input_label->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
    input_label->setFont("gt_pressura_bold_24");

    input_label->focusChangedCallback = parent->parentSetCallback();
    boxItems.push_back(element);

    for (auto el : boxItems) {
        hbox->addWidget(el);
    }
    parent->addWidget(hbox);
}

Item *ContactInputBox::Option::getNavigationItem() {
    return input_label;
}

ContactInputBox::Option::~Option() {}

void ContactInputBox::addInput(const UTF8 &text, const UTF8 &val) { elements.push_back(new Input(this, text, val)); }

void ContactInputBox::addOption(const UTF8 &text, const UTF8 &val) { elements.push_back(new Option(this, text, val)); }

Item *ContactInputBox::getNavigationItem(Navigation what) {
    Item *ret = nullptr;
    if (what == Navigation::Last && elements.back()) {
        ret = elements.back()->getNavigationItem();
    } else if (what == Navigation::First && elements.front()) {
        ret = elements.front()->getNavigationItem();
    }
    return ret;
}

std::vector<Item *> ContactInputBox::getNavigationItems() {
    std::vector<Item *> ret;
    for (auto element : elements) {
        ret.push_back(element->getNavigationItem());
    }
    return ret;
}

std::map<std::string, UTF8> ContactInputBox::getData() {
    std::map<std::string, UTF8> lmap;
    for (auto item : elements) {
        lmap.insert(std::make_pair(item->name().c_str(), item->value()));
    }
    return lmap;
};

std::function<bool(Item &)> ContactInputBox::parentSetCallback() {
    return [=](Item &el) {
        if (el.focus) {
            ++this->focus_cnt;
            this->setVisible(true);
        } else {
            if (--this->focus_cnt == 0) {
                this->setVisible(false);
            }
        }
        return true;
    };
}

}; // namespace gui
