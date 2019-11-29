#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <memory>
#include <GridLayout.hpp>

namespace gui
{

class UiCharSelector : public AppWindow
{
  protected:
    GridLayout *box;
  public:
    UiCharSelector(app::Application *app);
    virtual ~UiCharSelector() = default;

    // virtual methods
    void onBeforeShow(ShowMode mode, SwitchData *data) override;
    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
};

};
