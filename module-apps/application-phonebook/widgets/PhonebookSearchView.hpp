#pragma once

#include "Application.hpp"
#include "ListView.hpp"

namespace gui
{
class PhonebookSearchItem;
class PhonebookSearchView : public ListView
{
    /**
     * Pointer to application that owns the model
     */
    app::Application *application = nullptr;

  public:
    PhonebookSearchView();
    PhonebookSearchView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    virtual ~PhonebookSearchView();
    void setApplication(app::Application *app);
    PhonebookSearchItem *getSelectedSearchItem();
    bool onInput(const InputEvent &inputEvent) override;
    void updatePageItems() override;
};

} /* namespace gui */
