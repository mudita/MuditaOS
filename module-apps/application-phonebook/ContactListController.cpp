#include <log/log.hpp>
#include "ContactListController.hpp"
#include "windows/PhonebookData.hpp"

namespace gui {

void ContactListController::reloadFavouritesNum(uint32_t num) { numFavourites = num; }
void ContactListController::reloadContactsNum(uint32_t num) { numContacts = num; }

void ContactListController::position_boxes()
{
    if (box[ContactsPosition] && box[FavouritesPosition]) {
        box[FavouritesPosition]->setPosition(box[FavouritesPosition]->widgetArea.x, boundingBox.y);
        box[ContactsPosition]->setPosition(box[ContactsPosition]->widgetArea.x, box[FavouritesPosition]->offset_h());
    }
}

void ContactListController::buildPageBox(ContactListModel::ContactType type, uint32_t available_h, uint32_t offset_h)
{
    auto box = this->box.end();
    if (type == ContactListModel::ContactType::Favouirte) {
        box = this->box.begin() + FavouritesPosition;
    } else if (type == ContactListModel::ContactType::Contact) {
        box = this->box.begin() + ContactsPosition;
    } else {
        return;
    }

    if ((*box) != nullptr) {
        window->removeWidget(*box);
        delete (*box);
        (*box) = nullptr;
    }
    std::vector<ContactRecord> *records;
    records = model->getContacts(type, maxElements);

    uint32_t true_height = 0;
    if (records && records->size() != 0) {
        (*box) = new ContactsVBox(window, boundingBox.x, offset_h, boundingBox.w, available_h, maxElements);
        for (auto a : *records) {
            auto el = tmp_lambda((*box), a.primaryName);
            true_height += el->h();
            if (!(*box)->addWidget(el)) {
                true_height -= el->h();
                break;
            }
            LOG_INFO("Added : %s", a.primaryName.c_str());
        }
    } else {
        /// empty (*box) - just to be able to derive the height
        (*box) = new ContactsVBox(window, 0, 0, 0, 0, 0);
    }

    (*box)->setSize((*box)->w(), true_height);
    (*box)->setMaxSize((*box)->w(), true_height);
    LOG_INFO("%s count: %d true_height [%d] size: x: %d y: %d w: %d h: %d",
             (type == ContactListModel::ContactType::Favouirte) ? "Favourite" : "Contact", (*box)->getCount(), true_height, (*box)->widgetArea.x,
             (*box)->widgetArea.y, (*box)->widgetArea.w, (*box)->widgetArea.h);
}

bool ContactListController::buildPage()
{
    LOG_INFO("Build page!");
    if (!model)
        return false;
    // build it all!
    uint32_t available_h = boundingBox.h, offset_h = boundingBox.y;
    buildPageBox(ContactListModel::ContactType::Favouirte, boundingBox.h, boundingBox.y);
    // build on whats left...
    available_h -= box[FavouritesPosition]->height();
    offset_h += box[FavouritesPosition]->height();
    buildPageBox(ContactListModel::ContactType::Contact, available_h, offset_h);
    // update how many we print now
    model->updateLastCount(box[FavouritesPosition] ? box[FavouritesPosition]->size() : 0, box[ContactsPosition] ? box[ContactsPosition]->size() : 0);
    return false;
}

bool ContactListController::getPreviousPage()
{
    if (model == nullptr) {
        return false;
    }
    bool handled = false;

    LOG_INFO("-> getPreviousPage");

    if (pageNumber == 0) {
        return false;
    } else {
        --pageNumber;
    }

    if (!handled) {
        auto type = ContactListModel::ContactType::Contact;
        handled = model->getPreviousContacts(type);
        if (handled) {
            buildPageBox(type, boundingBox.h, boundingBox.y);
            model->updateLastCount(type, box[ContactsPosition]->size());
        }
    }

    uint32_t size_left = boundingBox.h - box[ContactsPosition]->h();
    if (size_left) {
        auto type = ContactListModel::ContactType::Favouirte;
        bool addhandled = model->getPreviousContacts(type);
        if (addhandled) {
            buildPageBox(type, size_left, boundingBox.y + box[ContactsPosition]->h());
            model->updateLastCount(type, box[FavouritesPosition]->size());
        }
        handled |= addhandled;
    }
    position_boxes();
    return handled;
}

bool ContactListController::getNextPage(const gui::InputEvent &inputEvent)
{
    bool handled = false;
    if (model == nullptr) {
        return false;
    }
    // we are on last page -> on last item -> we took everything from db
    if ((focusItem == *std::prev(box.end())) && focusItem->onLastItem() && model->all()) {
        return false;
    }
    LOG_INFO("-> getNextPage");
    if (!handled) {
        auto type = ContactListModel::ContactType::Favouirte;
        handled = model->getNextContacts(type);
        if (handled) {
            buildPageBox(type, boundingBox.h, boundingBox.y);
            model->updateLastCount(type, box[FavouritesPosition]->size());
        }
    }
    uint32_t size_left = boundingBox.h - box[FavouritesPosition]->h();
    if (size_left) {
        auto type = ContactListModel::ContactType::Contact;
        bool addhandled = model->getNextContacts(type);
        if (addhandled) {
            buildPageBox(type, size_left, boundingBox.y + box[FavouritesPosition]->h());
            model->updateLastCount(type, box[ContactsPosition]->size());
        }
        handled |= addhandled;
    }
    if (handled) {
        ++pageNumber;
    }
    return handled;
}

ContactListController::ContactListController(app::Application *app, ContactListModel *model, AppWindow *window) : model(model), app(app), window(window)
{
}

bool ContactListController::handleEnter()
{
    if(!app) return false;
    if(!focusItem) return false;
    ContactRecord record;
    if(focusItem == box[FavouritesPosition]) {
        record = model->getContactFromPos(ContactListModel::ContactType::Favouirte, focusItem->activeItemNumber());
    } else {
        record = model->getContactFromPos(ContactListModel::ContactType::Contact, focusItem->activeItemNumber());
    }
    return app->switchWindow("PhonebookOptions",0,std::unique_ptr<PhonebookContactData>(new PhonebookContactData(record)));
}

// TODO handle enter key  : item in db[index = ( box[FavouritesPosition]?box[ContactsPosition]) position] -> return ContactItem
bool ContactListController::onInput(const gui::InputEvent &inputEvent, bool firstTry)
{
    // react only on key released
    if ((inputEvent.state != InputEvent::State::keyReleasedShort) && ((inputEvent.state != InputEvent::State::keyReleasedLong)))
        return false;
    if(inputEvent.keyCode == KeyCode::KEY_ENTER) {
        return handleEnter();
    }
    bool up = inputEvent.keyCode == KeyCode::KEY_UP;
    // handle input by kiddos
    bool done = false;
    if (focusItem) {
        // why setFocus don't work? (o_o)
        Item tmp;
        tmp.focus = false;
        focusItem->focusChangedCallback(tmp);
    }
    // switch items: for up order will be box[ContactsPosition] -> box[FavouritesPosition], for down box[FavouritesPosition] -> box[ContactsPosition],
    // try to handle in order from last
    {
        auto tryhandle = [=](gui::ContactsVBox *ptr) {
            bool done = ptr->onInput(inputEvent);
            if (done) {
                focusItem = ptr;
            }
            return done;
        };
        if (up) {
            for (auto it = box.rbegin(); it != box.rend(); ++it) {
                if ((done = tryhandle(*it))) {
                    break;
                }
            }
        } else {
            for (auto it = box.begin(); it != box.end(); ++it) {
                if ((done = tryhandle(*it))) {
                    break;
                }
            }
        }
    }
    // kiddos didn't hande (no next/ no prev from them)
    if (!done) {
        // avoid infinite recurence
        if (!firstTry) {
            return false;
        }
        if (up) {
            done = getPreviousPage();
        } else if (inputEvent.keyCode == KeyCode::KEY_DOWN) {
            done = getNextPage(inputEvent);
        }
        if (done) {
            // there are new kids in the town
            LOG_INFO("Build page on new data!");
            focusItem = nullptr;
            // if new kiddos did better -> handle focus
            onInput(inputEvent, false);
        }
    }
    if (focusItem) {
        focusItem->focusChangedCallback(*this);
    }
    return done;
}

bool ContactListController::onInput(const gui::InputEvent &inputEvent)
{
    return onInput(inputEvent, true);
}

void ContactListController::setViewArea(const BoundingBox &box)
{
    boundingBox = box;
}

bool ContactListController::initialize()
{
    if (!first_run && buildPage()) {
        first_run = true;
        return true;
    } else {
        return false;
    }
}

}; // namespace gui
