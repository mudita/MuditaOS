#include "ContactsVBox.hpp"
#include "DefaultStyle.hpp"
#include <log/log.hpp>

using namespace gui;

ContactsVBox::ContactsVBox( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h, const int maxsize):
    VBox(parent,x,y,w,h), maxsize(maxsize), count(0)
{
    active_item = action_items.begin();
    this->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    focusChangedCallback = [=](Item& it) {
        if(!*active_item) return false;
        return (*active_item)->setFocus(it.focus);
    };

    // controller first try to bother box , then do it's stuff
    inputCallback = [=](const InputEvent &evt)
    {
        // LOG_INFO("Box inputCallback %d %s", action_items.size(), evt.keyCode == KeyCode::KEY_UP?"KeyUp":evt.keyCode == KeyCode::KEY_DOWN?"KeyDown":"Unknown");
        if (action_items.size()) {
            if (evt.keyCode == KeyCode::KEY_UP) {
                if (active_item == action_items.begin()) {
                    active_item_number = 0;
                    return false;
                } else {
                    // get previous element from the list
                    active_item = std::prev(active_item);
                    active_item_number--;
                }
            } else if (evt.keyCode == KeyCode::KEY_DOWN) {
                if (action_items.end() == std::next(active_item)) {
                    active_item_number = action_items.size()-1;
                    return false;
                } else {
                    active_item = std::next(active_item);
                    active_item_number++;
                }
            } else {
                return false;
            }
            return true;
        } else {
            return false;
        }
    };
}

bool ContactsVBox::addWidget( Item* item )
{
    if(count < maxsize) {
        VBox::addWidget(item);
        ///// TEMP TEMP TEMp
        action_items.push_back(item);
        ++count;
        return true;
    } else {
        return false;
    }
}

ContactBox::ContactBox( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h):
    HBox(parent, x, y, w, h)
{
}

void ContactBox::create(const UTF8 &text, std::vector<gui::Image*> &&elements) {
    static const int last_element_offset=10;
    int label_w = this->w() - 1 - last_element_offset - [=]{ int i =0; for (auto el:elements) { i+= el->w(); } ; return i; }();
    style::round(this);
    auto label = new Label(this, 0,0, label_w, this->h(), text);
    label->setFont("gt_pressura_regular_24");
    label->setMaxSize(label_w, this->h());
    label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    for( auto el :elements) {
        el->setPosition(0, (this->h()-el->h())/2);
        this->Item::addWidget(el);
    }
}


ContactLabel::ContactLabel( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h):
    HBox(parent, x,y,w,h)
{
}

void ContactLabel::create(const UTF8 &text)
{
    int letters_w = style::letter_w() * text.length();
    this->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    auto text_label = new Label(this, 0, 0, this->w(), this->h(), text);
    text_label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    text_label->setFont("gt_pressura_bold_24");
    text_label->setMaxSize(letters_w, this->h());
    auto rect = new Rect(this, 0,0, this->w() - letters_w -1, this->h());
    rect->setMaxSize(this->w() - letters_w - 1, this->h());
    rect->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
    // bottom element move up
    rect->setPosition(0,-10);
    this->resizeItems();
}
