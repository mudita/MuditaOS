#pragma once
#include <functional>
#include <string>

#include "../models/PhonebookModel.hpp"
#include "../widgets/PhonebookListView.hpp"
#include "AppWindow.hpp"
#include "ListView.hpp"
#include "Style.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"

namespace gui
{

class PhonebookMainWindow : public AppWindow
{
  protected:
    PhonebookListView *list;
    PhonebookModel *phonebookModel = nullptr;

    Image *leftArrowImage = nullptr;
    Image *rightArrowImage = nullptr;
    Image *newContactImage = nullptr;
    Image *searchImage = nullptr;

  public:
    PhonebookMainWindow(app::Application *app);
    virtual ~PhonebookMainWindow();

    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, SwitchData *data) override;
    bool onDatabaseMessage(sys::Message *msgl) override;

    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
};

} /* namespace gui */
