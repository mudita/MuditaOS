#include "catch.hpp"
#include <module-gui/gui/widgets/Item.hpp>
#include <module-gui/gui/widgets/Text.hpp>
#include "mock/TestWindow.hpp"

// TESTS TO BE ADDED
//         focusChangedCallback     = [](Item &) { return false; };
//             -> input mode change!!!
//         activatedCallback        = [](Item &) { return false; };
//         inputCallback            = [](Item &, const InputEvent &inputEvent) { return false; };
//             -> character returned!
//             -> character modes!
//             -> special char request
//             -> space handling
//             -> newline handling
//         dimensionChangedCallback = [](Item &, void *data) { return false; };
//             -> what will happen to BoxLayout
//         contentCallback          = [=](Item &) {
//             -> why is it needed
//             -> what it does
//         calbacks order: there is strict order on which callbacks should be called
