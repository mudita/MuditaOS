#pragma once

#include "../PhonebookModel.hpp"
#include "../data/PhonebookItemData.hpp"
#include "../widgets/PhonebookListView.hpp"
#include "AppWindow.hpp"
#include "ContactRecord.hpp"
#include "Label.hpp"
#include "Text.hpp"
#include <memory>
#include <string>
#include <widgets/BoxLayout.hpp>

using namespace gui;

class PhonebookSearch : public AppWindow
{
  protected:
    /** labels */
    Label *titleLabel = nullptr;
    Label *topSeparatorLabel = nullptr;
    Label *searchHeader = nullptr;
    Text *informationLabel = nullptr;
    Text *inputField = nullptr;
    Image *searchTop;
    Image *searchCenter;
    Image *fancyArrowDown;

    PhonebookListView *searchResultList;
    PhonebookModel *phonebookModel = nullptr;

    /** currently displayed contact */
    std::shared_ptr<ContactRecord> contact = nullptr;

  public:
    PhonebookSearch(app::Application *app);
    virtual ~PhonebookSearch();

    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, SwitchData *data) override;
    bool handleSwitchData(SwitchData *data) override;
    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;

  private:
    Label *addLabel(std::list<Item *> *parentPage, int x, int y, int w, int h, const std::string text,
                    const std::string fontName, const RectangleEdgeFlags edges, const Alignment alignment,
                    const bool lineMode);
};
