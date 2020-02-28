/*
 * @file EnterNumberWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "../ApplicationCall.hpp"
#include "../widgets/Icon.hpp"
#include "AppWindow.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/TopBar.hpp"
#include "gui/widgets/Window.hpp"

namespace gui {

/*
 *
 */
class EnterNumberWindow: public AppWindow {
protected:
  gui::Label *numberLabel = nullptr;
  gui::Icon *newContactIcon = nullptr;

  void setNumberLabel(const std::string num);

public:
  EnterNumberWindow() = delete;
  EnterNumberWindow(app::Application *app, std::string windowName = app::window::name_enterNumber);
  virtual ~EnterNumberWindow();

  bool onInput(const InputEvent &inputEvent) override;
  bool handleSwitchData(SwitchData *data) override;

  void rebuild() override;
  void buildInterface() override;
  void destroyInterface() override;

private:
  gui::KeyInputMappedTranslation translator;
};

} /* namespace gui */

