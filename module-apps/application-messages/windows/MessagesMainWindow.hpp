/*
 * @file MessagesMainWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 25 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_MESSAGES_WINDOWS_MESSAGESMAINWINDOW_HPP_
#define MODULE_APPS_APPLICATION_MESSAGES_WINDOWS_MESSAGESMAINWINDOW_HPP_

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include "ListView.hpp"
//#include "../PhonebookModel.hpp"
//#include "../widgets/PhonebookListView.hpp"

namespace gui {

class MessagesMainWindow: public AppWindow {
  protected:
    gui::Label *title = nullptr;
//    PhonebookListView* list;
//    PhonebookModel* phonebookModel = nullptr;

    Image* leftArrowImage = nullptr;
    Image* rightArrowImage = nullptr;
    Image* newMessageImage = nullptr;
    Image* searchImage = nullptr;

  public:
    MessagesMainWindow(app::Application *app);
    virtual ~MessagesMainWindow();

    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) override;

    bool onDatabaseMessage( sys::Message* msgl );

    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
};

} /* namespace gui */


#endif /* MODULE_APPS_APPLICATION_MESSAGES_WINDOWS_MESSAGESMAINWINDOW_HPP_ */
