#pragma once

#include "../data/PhonebookItemData.hpp"
#include "AppWindow.hpp"
#include "ContactRecord.hpp"
#include "Label.hpp"
#include "Text.hpp"
#include <memory>
#include <string>
#include <widgets/BoxLayout.hpp>

using namespace gui;

class PhonebookDeleteContact : public AppWindow
{
  protected:
    /** labels */
    Label *titleLabel = nullptr;
    Text *confirmationLabel = nullptr;
    Label *noLabel = nullptr;
    Label *yesLabel = nullptr;
    Image *trashacnIcon = nullptr;
    /** currently displayed contact */
    std::shared_ptr<ContactRecord> contact = nullptr;

  public:
    PhonebookDeleteContact(app::Application *app);
    virtual ~PhonebookDeleteContact();
    void setContactData();
    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, SwitchData *data) override;
    bool handleSwitchData(SwitchData *data) override;
    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
};

class PhonebookBlockContact : public AppWindow
{
  protected:
    /** labels */
    Label *titleLabel = nullptr;
    Text *confirmationLabel = nullptr;
    Label *noLabel = nullptr;
    Label *yesLabel = nullptr;
    Image *icon = nullptr;
    /** currently displayed contact */
    std::shared_ptr<ContactRecord> contact = nullptr;

  public:
    PhonebookBlockContact(app::Application *app);
    virtual ~PhonebookBlockContact();
    void setContactData();
    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, SwitchData *data) override;
    bool handleSwitchData(SwitchData *data) override;
    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
};

class PhonebookDuplicateNumber : public AppWindow
{
  protected:
    /** labels */
    Label *titleLabel = nullptr;
    Text *confirmationLabel = nullptr;
    Label *noLabel = nullptr;
    Label *yesLabel = nullptr;
    Image *icon = nullptr;
    /** currently displayed contact */
    std::shared_ptr<ContactRecord> contact = nullptr;

  public:
    PhonebookDuplicateNumber(app::Application *app);
    virtual ~PhonebookDuplicateNumber();
    void setContactData();
    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, SwitchData *data) override;
    bool handleSwitchData(SwitchData *data) override;
    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
};

class PhonebookDuplicateSpeedDial : public AppWindow
{
  protected:
    /** labels */
    Label *titleLabel = nullptr;
    Text *confirmationLabel = nullptr;
    Label *noLabel = nullptr;
    Label *yesLabel = nullptr;
    Label *dialValue = nullptr;
    Image *icon = nullptr;
    /** currently displayed contact */
    std::shared_ptr<ContactRecord> contact = nullptr;

  public:
    PhonebookDuplicateSpeedDial(app::Application *app);
    virtual ~PhonebookDuplicateSpeedDial();
    void setContactData();
    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, SwitchData *data) override;
    bool handleSwitchData(SwitchData *data) override;
    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
};
