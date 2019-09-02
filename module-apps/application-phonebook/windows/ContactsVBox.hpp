#pragma once

#include <BoxLayout.hpp>
#include <Label.hpp>
#include <functional>
#include <Image.hpp>

namespace gui {

class ContactsVBox : public VBox {
    private:
        int maxsize, count, visible_cnt;
        // just to store items which are interactive
        std::list<Item*> action_items;
    public:
        void setMaxsize(const int val) { maxsize = val; }
        ContactsVBox() : maxsize(0) {};
        ContactsVBox( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h, int maxsize=0);
        virtual ~ContactsVBox() = default;
        virtual bool addWidget(Item *item) override;
        virtual bool addWidget(Item *item, std::function<bool(Item&)>cb);
        // set navigation through list on valid elements, if prev / next != null -> sets previous/next element
        void setNavigation(Item *prev, Item *next);
        // return if box should be visible or not
        bool isVisible() { return visible_cnt > 0; }
        // get all items used for navigation
        std::list<Item*> &getNavigationItems();
        /// size of action_items stored
        uint32_t size() { return action_items.size(); };
        /// return first navigation item
        Item* navigation_begin() { if(size()) { return *action_items.begin(); } else { return nullptr;}}
        Item* navigation_end() { if(size()==0) { return nullptr; } else if (size() == 1) return *action_items.begin(); else return *std::prev(action_items.end()); };
};

// this: |[ Name     o]|
// TODO getNavigationItem && setAction - setActive -> forward to proper child
class ContactBox : public HBox {
    public:
        ContactBox() = default;
        virtual ~ContactBox() = default;
        ContactBox( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h);
        void create(const UTF8 &text, std::vector<gui::Image*> &&elements=std::vector<gui::Image*>());
};

// this: | A ------- |
class ContactLabel : public HBox {
    public:
        ContactLabel() = default;
        virtual ~ContactLabel() = default;
        ContactLabel( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h);
        void create(const UTF8 &text);
};

}
