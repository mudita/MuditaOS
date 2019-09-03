#pragma once

#include <stdint.h>
#include <Item.hpp>
#include "windows/ContactsVBox.hpp"
#include "ContactListModel.hpp"
#include "AppWindow.hpp"
#include <log/log.hpp>

namespace gui {

class ContactListController : public Item {
    protected:
        ContactListModel *model = nullptr;
        uint32_t numFavourites =0;
        uint32_t numContacts =0;
        uint32_t pageNumber = 0;
        const uint32_t maxElements = 7;

        /// TODO use list widget instead, so that it would handle multiscreen long views
        enum {
            FavouritesPosition,
            ContactsPosition,
        };
        std::vector<gui::ContactsVBox*> box = { nullptr, nullptr };
        app::Application *app = nullptr;
        gui::AppWindow *window = nullptr;
        gui::BoundingBox boundingBox;
        bool first_run = false;
        gui::ContactsVBox *focusItem = nullptr;

        void reloadFavouritesNum(uint32_t num);
        void reloadContactsNum(uint32_t num);
        void position_boxes();

        // TODO add proper views instead
        std::function<Item*(Item*,const UTF8 &text)> tmp_lambda = [](Item *parent, const UTF8 &text) -> Item* {
                                gui::Label *item = new Label();
                                item->setSize(parent->w(), 50);
                                item->setMaxSize(item->w(), item->h());
                                item->setText(text);
                                item->setPenWidth(1);
                                item->setPenFocusWidth(3);
                                return item;
                          };

        void buildPageBox(ContactListModel::ContactType type, uint32_t available_h, uint32_t offset_h);
        bool buildPage();
        bool getPreviousPage();
        /// next page -> first to last box order
        bool getNextPage(const gui::InputEvent& inputEvent);
        bool handleEnter();

    public:
        ContactListController(app::Application *app, ContactListModel *model, AppWindow *window);
        bool onInput(const gui::InputEvent& inputEvent, bool firstTry);
        bool onInput(const gui::InputEvent& inputEvent);
        void setViewArea(const BoundingBox &box);
        bool initialize();
};

};
