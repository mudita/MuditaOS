/*
 * @file PhonebookListView.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 11 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_PHONEBOOK_WIDGETS_PHONEBOOKLISTVIEW_HPP_
#define MODULE_APPS_APPLICATION_PHONEBOOK_WIDGETS_PHONEBOOKLISTVIEW_HPP_

#include "Application.hpp"
#include "ListView.hpp"
#include <functional>

namespace gui
{
class PhonebookItem;
class PhonebookListView : public ListView
{
    /**
     * Pointer to application that owns the model
     */
    app::Application *application = nullptr;

  public:
    PhonebookListView();
    PhonebookListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    virtual ~PhonebookListView();
    void setApplication(app::Application *app);
    gui::PhonebookItem *getSelectedPhonebookItem();

    bool onInput(const InputEvent &inputEvent) override;
    void updatePageItems() override;

    // onClick callback to register
    std::function<bool(gui::PhonebookItem *item)> cb_ENTER = nullptr;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_PHONEBOOK_WIDGETS_PHONEBOOKLISTVIEW_HPP_ */
