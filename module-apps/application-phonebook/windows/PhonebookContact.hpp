#pragma once

#include "../data/PhonebookItemData.hpp"
#include "AppWindow.hpp"
#include "ContactRecord.hpp"
#include "Label.hpp"
#include "Style.hpp"
#include "Text.hpp"
#include <memory>
#include <string>
#include <widgets/BoxLayout.hpp>

using namespace gui;

class PhonebookContact : public AppWindow
{
  protected:
    /** labels */
    Label *informationLabel = nullptr;
    Label *ifnormation = nullptr;
    Label *addressLabel = nullptr;
    Label *addressLine1 = nullptr;
    Label *addressLine2 = nullptr;
    Label *email = nullptr;
    Label *numberPrimary = nullptr;
    Label *numberSecondary = nullptr;
    Label *favouritesLabel = nullptr;
    Label *speeddialLabel = nullptr;
    Label *blockedLabel = nullptr;
    Label *speedDialLabel = nullptr;
    Label *speedDial = nullptr;
    Label *topSeparatorLabel = nullptr;
    Label *noteLabel = nullptr;
    Text *noteText = nullptr;

    /** icons */
    Image *favouritesIcon = nullptr;
    Image *blockedIcon = nullptr;

    Label *numberPrimaryLabel = nullptr;
    Image *numberPrimaryIcon = nullptr;

    Label *numberPrimaryMessageLabel = nullptr;
    Image *numberPrimaryMessage = nullptr;

    Label *numberSecondaryLabel = nullptr;
    Image *numberSecondaryIcon = nullptr;

    Label *numberSecondaryMessageLabel = nullptr;
    Image *numberSecondaryMessage = nullptr;

    ContactRecord readContact();

    /** currently displayed contact */
    std::shared_ptr<ContactRecord> contact = nullptr;

    std::list<Item *> page1;
    std::list<Item *> page2;

    void setVisible(std::list<Item *> *page, bool shouldBeVisible);

  public:
    PhonebookContact(app::Application *app);
    virtual ~PhonebookContact();

    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, SwitchData *data) override;
    bool handleSwitchData(SwitchData *data) override;
    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
    void setContactData();
    Label *addLabel(std::list<Item *> *parentPage, int x, int y, int w, int h, const std::string text, const std::string fontName,
                    const RectangleEdgeFlags edges, const Alignment alignment, const bool lineMode);
};
