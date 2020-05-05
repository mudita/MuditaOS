UI README
=========

@tableofcontents

schematics parts used:
* `[name : part]` - name of module : part of code responsible
* `=> / <=>` - direction of communication
* `<>` event on diagrams, name placeholder of element in text

# introduction

## How widgets are rendered

Our UI is split into:
```
[bsp] <=> [renderer] <=> [application : widgets]
```

* All widgets are children of `gui::Item`
* There are two major commands to trigger screen redraw:
    * `gui::Item::buildDrawList` in each `gui::Item` - uses `gui::Item`s tree to builds draw commands 
    * `app::Application::refreshWindow` in `app::Application` - triggers update on display on message: `gui::AppRefreshMessage` (final draw on screen done with: `app::Application::render`)
* All interface actions can be made with either:
    * `gui::Item` callbacks, which are in [callbacks](@ref callbacks "Item callbacks")
    * `gui::Item` virtual functions, [callback functions](@ref callbackCallers) When overriding please mind that you might want to use ancestor function inside too. I.e. to not loose key presses etc.
* All `gui::Item` are able to handle keyPresses via `gui::InputEvent`
* All `gui::Item` are able to traverse down on their `gui::Item::children` and know which child have `gui::Item::focusItem` at the time

## How does it work from application

Please see `app::Application`, `sapm::ApplicationManager` for detailed information on how messages are handled between both. This is jus overall documentation.

```
[ApplicationManager]                 [Application]
               <=>    START       <=>
               <=>   Set focus    <=>
               <=> Set keyboard   <=>
               <=> Put background <=>
               <=>      kill      <=>
```

These actions are done on chained bus request between: `app::Application`, `sapm::ApplicationManager` and `sapm::EventWorker`
There all of these are asynchronous and there is little state machine maintenance.
1. `app::Application` has it's own state which is managed both in application and in manager (via setters and getters)
2. `sapm::ApplicationManager` has it's own state which tells what exactly it's processing right now

*Important* all `app::Application`
1. register and initialize their windows on start of application in `app::Application::createUserInterface`
2. need to pass `app::Application::DataReceivedHandler` first to parent function call to properly handle bus messages
3. have windows based on `gui::AppWindow`

*Important* `gui::AppWindow`
1. `gui::AppWindow::buildInterface` has to call parent build interface first. Otherwise elements for child won't be created and it will crash
2. `gui::AppWindow::onInput` has to call parent onInput, otherwise handling key presses might and most probably will fail
3. Applications react on key *releases* actions, in most scenarios key press event is useless.
4. All applications, if it wasn't overriden in `gui::AppWindow` will try to return to previous window or application on `back`

# gui::Item key press handling

## What happens when you press the key?

```
[bsp] <basic freertos pipe> => [EventWorker : basic key translation] < key press event> => [Application with focus]
```

* bsp handles key press on I2C IRQ and sends Event to event worker on naked freertos pipe (on target rt1051, on linux gtk does that)
* EventWorker worker of EventService
    * handles the press sends it to current Application
    * with focus **warning** when no application with focus this will stuck
* application can either:
    * process `gui::InputEvent` with `RawKey` or `gui::KeyInputSimpleTranslation`
    * use callbacks see how to handle key press below
    * use widgets which override default key handling (see `gui::Item::onInput`)
    * have own `gui::KeyInputMappedTranslation` +  `gui::InputMode` and process key however they want

## How to handle key press

There are at least 3 ways to handle key press, listed in order of execution.
* `gui::Item::onInput` - if not redefined calls inputCallback, if handled here, other calls wont be called
* `gui::Item::inputCallback` - handles any key
* `gui::Item::activatedCallback` - handles enter only
* `gui::Item::itemNavigation` - handles up,down,left,right if next/previous elements are added for item

**return true in any of callbacks ends processing for whole Items tree**

There are 2 set of parameters for key press:
* `gui::InputEvent::State` - tate of key, pressed, released, long released. In generall applications handle *releases* not presses!
* `gui::KeyCode`   - initially parsed key code
* `gui::RawKey`  - raw key code, to be processed in widget based on event. ( i.e. Translate 3 times 1 press to C in `gui::Text` mode `ABC`)

## How to add key mapping when basic key maps are not enough?

* Key maps are specific key translation mappings. I.e. 3 times press 1 to get C, 4 times press 1 to get A etc.
* basic key maps are stored in: `InputMode`, right now there are `InputMode::Mode`s: [ABC, abc, digit, phone]
* key maps in `gui::InputMode` are changed in regards of lang settings

### To add new key map, i.e. phone

1. Add new file for your key map: `cp image/assets/profiles/template.kprof image/assets/profiles/phone.kprof`
2. change your template according to your desires
3. Pin new key map = add it to language support, add: `"common_kbd_phone": "phone"` to at least `image/assets/lang/lang_en.json` if it will differ per language, prepare one kprof per language
4. Add new key map to `gui::InputMode`
    1. add `InputMode::Mode` enum i.e. `InputMode::Mode::phone`
    2. add new Mode to input mode mapping in `InputMode.cpp` (same as with other enums)
    3. test new added mode in: `UITestWindow.cpp`
    4. test new key map on phone
5. load key map to phone, as you probably forgot

Now you can use `InputMode::Mode::phone` translation in `gui::Text` widget.
This means `gui::Text` will automatically change text on key press for you same as in modes `InputMode::Mode::phone` etc.
