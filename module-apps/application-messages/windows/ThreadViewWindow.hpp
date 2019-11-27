/*
 * @file ThreadViewWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 25 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_MESSAGES_WINDOWS_THREADVIEWWINDOW_HPP_
#define MODULE_APPS_APPLICATION_MESSAGES_WINDOWS_THREADVIEWWINDOW_HPP_

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include "ListView.hpp"

namespace gui {

/*
 *
 */
class ThreadViewWindow: public AppWindow {
public:
  ThreadViewWindow(app::Application *app);
  virtual ~ThreadViewWindow();

  // virtual methods
  bool onInput(const InputEvent &inputEvent) override;
  void onBeforeShow(ShowMode mode, SwitchData *data) override;

  bool onDatabaseMessage( sys::Message* msgl ) override;

  void rebuild() override;
  void buildInterface() override;
  void destroyInterface() override;

private:
  ListView* list;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_MESSAGES_WINDOWS_THREADVIEWWINDOW_HPP_ */
