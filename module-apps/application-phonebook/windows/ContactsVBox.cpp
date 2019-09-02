#include "ContactsVBox.hpp"
#include "DefaultStyle.hpp"

using namespace gui;

ContactsVBox::ContactsVBox( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h, const int maxsize):
    VBox(parent,x,y,w,h), maxsize(maxsize), count(0)
{
    this->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
}

bool ContactsVBox::addWidget( Item* item )
{
    if(count < maxsize) {
        VBox::addWidget(item);
        ++count;
        return true;
    } else {
        return false;
    }
}

bool ContactsVBox::addWidget( Item* item, std::function<bool(Item&)>cb)
{
    if(count<maxsize) {
        VBox::addWidget(item);
        action_items.push_back(item);
        item->activatedCallback = cb;
        item->focusChangedCallback = [=] (Item&item) {
            if(item.focus) {
                ++visible_cnt;
            } else {
                --visible_cnt;
            }
            return true;
        };
        ++count;
        return true;
    } else {
        return false;
    }
}

void ContactsVBox::setNavigation(Item *prev, Item *next)
{
    for (auto iter=action_items.begin(); iter != action_items.end();++iter) {
        if(iter != action_items.begin()) {
            (*iter)->setNavigationItem(NavigationDirection::UP, *std::prev(iter));
        } else if(prev) {
            (*iter)->setNavigationItem(NavigationDirection::UP, prev);
        }
        if(std::next(iter) != action_items.end()) {
            (*iter)->setNavigationItem(NavigationDirection::DOWN, *std::next(iter));
        } else if(next) {
            (*iter)->setNavigationItem(NavigationDirection::DOWN, next);
        }
    }
}


std::list<gui::Item*> &ContactsVBox::getNavigationItems()
{
    return action_items;
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
