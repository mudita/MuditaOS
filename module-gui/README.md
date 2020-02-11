UI README
=========

schematics parts used:
* `[name : part]` - name of module : part of code responsible
* `=> / <=>` - direction of communication
* `<>` event on diagrams, name placeholder of element in text

# introduction

Our UI is split into:
```
[bsp] <=> [renderer] <=> [application : widgets]
```

* All widgets are children of `gui::Item`
* There are two major commands to trigger screen redraw:
    * `buildDrawList(...)` in each `gui::Item` - which uses `gui::Item`s tree to builds draw commands 
    * `refreshWindow(...)` in `Application` - which triggers update on display with message: `AppRefreshMessage` 
* All interface actions can be made with either:
    * `gui::Item` callbacks, which are named `<something>Callback`
    * `gui::Item` virtual functions. When overriding please mind that you might want to use ancestor function inside too. I.e. to not loose key presses etc.
* All `gui::Items` are able to handle keyPresses
* All `gui::Items` are able to travers down on their children and know which child have focus at the time

# `gui::Items` key press handling

## What happens when you press the key?

```
[bsp] <basic freertos pipe> => [EventWorker : basic key translation] < key press event> => [Application with focus]
```

* bsp handles key press on I2C IRQ and sends Event to event worker on naked freertos pipe (on target rt1051, on linux gtk does that)
* EventWorker worker of EventService
    * handles the press sends it to current Application
    * with focus **warning** when no application with focus this will stuck
* application can either:
    * process `RawKey` which is numerical value of the key
    * handle key press(es) in widgets via `<any>KeyTranslator` and `InputMode`
        * key translator:
            * receives key from widget
            * translates it (and number of times it was pressed) to `utf8` key code from key map
        * `InputMode` is a cache manager / selector of key maps
        * _this way widgets manages their own key modes, change of key mode on lost focus etc._
    * have own `<any>keyTranslator` and process key however they want

## How to handle key press

There are at least 3 ways to handle key press, listed in order of execution.
* `onInput()` - if not redefined calls inputCallback, if handled here, other calls wont be called
* `inputCallback` - handles any key
* `activatedCallback` - handles enter only
* `navigation` - handles up,down,left,right if next/previous elements are added for item

**return true in any of callbacks ends processing for whole Items tree**

There are 2 set of parameters for key press:
* key press type:
    * press, release, long press
    * key code (based of number of times key was pressed & long press if needed)

## How to add key mapping when basic key maps are not enough?

* Key maps are specific key translation mappings. I.e. 3 times press 1 to get C, 4 times press 1 to get A etc.
* basic key maps are stored in: InputMode, right now there are three: [ABC, abc, digit]
* key maps in InputMode are changed in regards of lang settings

### To add new key map, i.e. `phone`

1. Add new file for your key map: `cp image/assets/profiles/template.kprof image/assets/profiles/phone.kprof`
2. change your template according to your desires
3. Pin new key map = add it to language support, add: `"common_kbd_phone": "phone"` to at least `image/assets/lang/lang_en.json` if it will differ per language, prepare one kprof per language
4. Add new key map to `InputMode`
    1. add `InputMode::Mode` enum i.e. `InputMode::Mode::Phone`
    2. add new Mode to input_mode mapping in `InputMode.cpp` (same as with other enums)
    3. test new added mode in: `module-apps/application-settings/windows/UITestWindow.cpp`
    4. test new key map on phone
5. rebuild assets and load key map to phone, as you probably forgot

Now you can use `InputMode::Mode::Phone` translation in gui::Text widget.
This means `gui::Text` will automatically change text on key press for you same as in modes `ABC` etc.
