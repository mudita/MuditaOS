#pragma once

#include <memory>
#include <string>

#include "AppWindow.hpp"
#include "ContactInputBox.hpp"
#include <widgets/BoxLayout.hpp>

#include "../ContactBuilder.hpp"
#include "PhonebookData.hpp"
#include <widgets/BoxLayout.hpp>

namespace gui {

class PhonebookInfo : public AppWindow {
  protected:
    std::unique_ptr<PhonebookInfoData> data;
    Label *text_label;
    Image *image;
    HBox *box;

  public:
    PhonebookInfo(app::Application *app);
    virtual ~PhonebookInfo();

    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) override;
    virtual bool handleSwitchData(SwitchData *data) override;

    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;

  private:
    void addContact(ContactBuilder &contact);
};

} /* namespace gui */
