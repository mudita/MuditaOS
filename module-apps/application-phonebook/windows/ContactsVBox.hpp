#pragma once

#include <BoxLayout.hpp>
#include <Label.hpp>
#include <functional>
#include <Image.hpp>

namespace gui {

class ContactsVBox : public VBox {
    private:
        int maxsize, count, visible_cnt, active_item_number=0;
        // just to store items which are interactive
        std::list<Item*> action_items;
        std::list<Item*>::iterator active_item;
    public:
        void setMaxsize(const int val) { maxsize = val; }
        ContactsVBox() : maxsize(0) {};
        ContactsVBox( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h, int maxsize=0);
        virtual ~ContactsVBox() = default;
        virtual bool addWidget(Item *item) override;

        uint32_t size() {
            return action_items.size();
        }

        uint32_t getCount() {
            return count;
        }

        bool onInput(const InputEvent &inputEvent) override
        {
            return inputCallback(inputEvent);
        };

        bool onLastItem() {
            return *active_item == action_items.back();
        }

        uint32_t height() {
            uint32_t h =0;
            for( auto a:children) {
                if(a!=nullptr)
                    h += a->h();
            }
            return h;
        }

        uint32_t activeItemNumber() {
            return active_item_number-1;
        }
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
