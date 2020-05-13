#include "catch.hpp"
#include <module-gui/gui/widgets/Item.hpp>

// there is strict order on which callbacks should be called
TEST_CASE("gui::Item on input flow test")
{
    bool success = false;
    auto item    = gui::Item();

    SECTION("gui item onInput callback called")
    {
        success            = false;
        item.inputCallback = [&success](gui::Item &, const gui::InputEvent &inputEvent) -> bool {
            success = true;
            return true;
        };
        // trigger callback
        item.onInput(gui::InputEvent({}));
        // test callback called
        REQUIRE(success == true);
    }

    SECTION("gui item resize called")
    {
        success                       = false;
        item.dimensionChangedCallback = [&success](gui::Item &, void *data) -> bool {
            success = true;
            return true;
        };
        // trigger callback
        item.setSize(item.getWidth(), item.getHeight());
        // test callback called
        REQUIRE(success == true);
    }
}
