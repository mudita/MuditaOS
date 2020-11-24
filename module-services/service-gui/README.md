Service GUI
===========


This service has one purpose: process draw requests to frames to draw on screen.

It processes following messages:
- DrawMessage : request to `draw` GUI commands on `canvas`. This canvas will be sent to Service Eink to display.
- RenderingFinished : response from GUI worker which processes (draws) gui commands on canvas
- GUIDisplayReady : message from service Eink that eink display is ready to use

It produces following messages:
- service::eink::ImageMessage - message to service Eink with rendered `canvas` which will be shown on display
- service::eink::StateRequest - request state from service eink
- 

It has following worker commands:
- WorkerGUICommands::Render - render request to process in Worker thread
- !!! sys::SystemManager::CloseSystem(this);                <-- **this should never be here**
- !!! app::manager::Controller::changePowerSaveMode(this);  <-- **this should never be here**


Context = Canvas

# Caveats:

## 1. wrong managers initialization

Service GUI creates 2 manager instances which are used in other parts of code. This shouldn't probably be in this service ctor.
Most probably this should be dependency to this service (and Applications too and eink too ) 

```
        gui::FontManager &fontManager = gui::FontManager::getInstance();
        fontManager.init("assets");

        gui::ImageManager &imageManager = gui::ImageManager::getInstance();
        imageManager.init("assets");
```

## 2. service GUI orchestrates power modes which isn't it's job

- !!! sys::SystemManager::CloseSystem(this);                <-- **this should never be here**
- !!! app::manager::Controller::changePowerSaveMode(this);  <-- **this should never be here**

## 3. We tend to forget Canvas area ( and just want to draw on whole screen

## 4. we don't have proper state machine, so we don't know what to do

States are driven on frame ID which is bad


## 5. no power state in service - this sucks...

## 6. constantly creating 2 / 3 frames just to remove them sucks

- 1mb at most 
- needless removes and allocations
- needless memory flushes

we should consider at max 3fb in constant rotation ( 1. to process data 2. to be ready to be shown 3. to be shown ) 
or better: 2 in constant rotation: 1. to process data 2. to be shown

Right now we have 3 shm's:
- 1 in eink (to show)
- 2 in gui  (to draw)

Should we have some FrameBuffer manager?
