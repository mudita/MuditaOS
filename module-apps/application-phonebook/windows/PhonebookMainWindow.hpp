#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/TopBar.hpp"
#include "gui/widgets/Window.hpp"
#include <widgets/BoxLayout.hpp>
#include "ContactsVBox.hpp"
#include <ContactRecord.hpp>

namespace gui {

/// no show item -> just used for callbacks triggering
/// if limit of contacts shown reached on navigation gets next/previous contacts
class Notifier : public Item {
    public:
        virtual ~Notifier() = default;
        Notifier(std::function<bool(Item&)> f) {
            focusChangedCallback = f;
        }
        Notifier(Notifier &&) = default;
};

class PhonebookMainWindow : public AppWindow {
    enum Type {
        Favourites,
        Normals
    };
    private:
        std::unique_ptr<Notifier> notifier_next, notifier_previous;
        uint32_t max_contacts_possible_to_show;
        uint32_t max_normals, normals_position;
        uint32_t usable_h();
        std::function<bool(ContactRecord record)> contact_click_cb;
        void buildContactBox(std::vector<ContactRecord> *contacts, ContactsVBox *&box, Type type);
        void destroyContactBox(ContactsVBox *&box);
        void showContacts(uint32_t max_favourites, uint32_t start_contacts, bool focus_first=true);
        void changeContactsPage(NavigationDirection dir);
        void addSearch();
    public:
        const int contact_size_h_px;
  protected:
    const unsigned int window_margin;
    HBox *topbox;
    Image *left,*right;
    ContactsVBox *favourites, *contacts;
    std::unique_ptr<std::vector<ContactRecord>> fav_contacts, norm_contacts;
    std::unique_ptr<Label> searchbox;

    gui::Item *addOptionLabel(const UTF8 &text, std::function<bool(Item &)> activatedCallback);

  public:
    PhonebookMainWindow(app::Application *app);
    virtual ~PhonebookMainWindow();

    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) override;

    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
};

} /* namespace gui */
