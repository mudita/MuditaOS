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

#include "../ContactListController.hpp"
#include "../ContactListModel.hpp"

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
    public:
        const int contact_size_h_px;
  protected:
    const unsigned int window_margin;
    HBox *topbox;
    Image *left,*right;
    ContactListController *controller = nullptr;
    ContactListModel *model = nullptr;

    void addSearch();
    uint32_t usable_h();
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
