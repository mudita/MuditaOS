Module apps
===========

# What's an application

In MuditaOS, application is a service derivative which:
- Has access to UI 
- Have a limited lifespan
- Can operate with actions and popups

# How to add application

This paragraph is using `applicaton-test` as an example.

**WARNING** All applications are enabled by default. Please disable yours if it's not always needed in [CMakeLists.txt](./CMakeLists.txt). I.e.: `option(ENABLE_APP_TEST "Enable application test" OFF)`

* Add new `application` catalog in `module-apps` i.e. `application-test`
* Add a CMake for it, simple example here: [application-test/CMakeLists.txt](./application-test/CMakeLists.txt)
* Create:
    * `include/application-test/` for public includes for this application
    * create the application header with:
        * Application definition - inheriting from `Application`
        * ApplicationManifest - stating what actions can this application support, and its lock policy
**NOTE** We strongly encourage using [MVP](https://en.wikipedia.org/wiki/Model–view–presenter) application GUI pattern.
* create presenters & windows catalogs with all the windows and presenters you require. Each application require at least one window.
* Write `InitHandler()` for application.
    * **NOTE** This is your application entry point!
    * **NOTE** if it's not copied properly - application won't start and work
Please see example in: [ApplicationTest.cpp](./application-test/ApplicationTest.cpp) for `InitHandler()`
* Write `createUserInterface()`, which role is:
    * provide `windowsFactory` with windows blueprints for this application
    * inform what popups this application handles
**WARNING** default window for each application is always named `gui::name::window::main_window`. Without that window application may be never shown properly

Basic example can be found in this catalog: [application-test](./application-test/)

# How to launch app:

1. From service/app via action `Launch`
example:
``` c++
return app::manager::Controller::sendAction(application,
                                            app::manager::actions::Launch,
                                            std::make_unique<app::ApplicationLaunchData>("ApplicationAlarmClock"));
                                  });

```
2. Enable it in background with parameter: `startInBackground` in Application constructor

**WARNING** Enabling app in the background means it will always lay on stack and heap and never terminate
**WARNING** App in the background just doesn't handle `Launch`. This means that if you call `Launch` on the system, there will be a "Terrible, terrible damage" log due to undefined system behavior. `Launch` action is meant to launch applications - the issue is system-side - the application manager might not be started yet.

**IMPORTANT**
Please always remember that it goes the same as with services:
- Constructor of application is called in its parent scope. In this case in `application-manager`
- The first place where appliaction is running in its scope is `InitHandler()`
log explaining:
```
2290 ms INFO  [ApplicationManager] ApplicationTest.cpp:ApplicationTest:19: created!
2293 ms INFO  [ApplicationTest] ApplicationTest.cpp:InitHandler:27: initializing!
```

# Async actions on data notifications on changed data in DB

Due to the fact that the application can be closed before asynchronous data is received one should have initialized `AsyncCallbackReceiver(this)`.
To have notifications on changed DB one should connect to the notifications:
```
bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
```
To propagate changes from the database to UI handling, one has to pass these from the application to windows. It can be done via `presenter`, but here's an alternative approach:
``` c++
if (msgl->messageType == MessageType::DBServiceNotification) {
    auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
    if (msg != nullptr) {
        userInterfaceDBNotification(msgl,
            [&]([[maybe_unused]] sys::Message *, [[maybe_unused]] const std::string &) {
                // your code here
             });
        return std::make_shared<sys::ResponseMessage>();
    }
}
```

# Asynchronous database queries

You can perform datababase queries via [query interface](/module-db/queries/README.md). 

To have a safe way to perform asynchronous calls there is `app::AsyncQuery` which:
* creates a query
* when it's done results are passed to the callback
* if i.e. UI element is removed its callback is automatically removed

It's defined here: [AsyncTask.hpp](apps-common/AsyncTask.hpp)

**NOTE** to automatically pass async `db::query` data to an application you have to:
* inherit in application/item from `public app::AsyncCallbackReceiver`
* pass the data to the aforementioned item in DataReceivedHandler via
`return handleAsyncResponse(resp);` at the end of `DataReceivedHandler()`

# GUI library

GUI library documentation: [module-gui](/module-gui/README.md)
Please mind that it has Doxygen documentation explaining its use for all base elements and functions.

# System actions

System action is a call from service to application.
**NOTE** By design applications are running only when required, whereas services are available all time when started.


With this, we can:
- Let application manager decide what application should handle action
- Let the user decide what actions the application can handle
- Change application's internal state on service/app request
    i.e. disable popups or request to show different app
- Change application's external state on service
    i.e. show popup in application

## What's a popup

**WARNING** In the MuditaOS a popup is a Window that can be shown any time on an action request.
**NOTE** `attachPopups` should be most probably moved to `manifest` and popup enum just removed

Popups weren't designed as a part of the system, but rather step by step integrated into it. While there should be no issue with partially overflowing popups in the applications and UI, it would mean major source code refactoring and planning.

Each popup has:
- **ID** used to identify popup and in the end is just translated to its string name.
- **blueprint** that follows the same logic as a window blueprint - it's used to create handling of the popup.
- **priority** that discerns in what order should the popups be shown

## Adding popup

### To use popup one have to

1. send popup action somewhere in the code
2. add registration on popup in application
3. add popup blueprint in the application if required - by default popup will be created from the window requested in the registered blueprint 
4. enable popup in application via `attachPopups`

### Adding popup to the code

* [ ] Add next enum to the list in:
    * [ ] [Popups.hpp](apps-common/popups/Popups.hpp)
    * [ ] Mapping to the window to use in: [Popups.cpp](apps-common/popups/Popups.cpp)
* [ ] Add i.e. AppTestPopup in `createUserInterface()` `attachPopups({gui::popup::ID::AppTestPopup});}`
* [ ] Create a new popup blueprint for that popup, or use the default one.
* [ ] Add popup handling in the window you specified in either: Popups.cpp or popup blueprint.

Now we can open such popup in the application via: `app::manager::Controller::sendAction(..., ShowPopup, ...)`

**NOTE** Mapping enum <-> string from Popups.cpp can be overridden in popup blueprint. More advanced popups will most likely require it.
**NOTE** Default fallback won't pass any data to the popup - application won't have the information that requested window is a popup and any additional `SwitchData`. To have this data - you need to call `popupBlueprint.registerBlueprint(...)`

# System communication caveats

* [Caveats and good practices](/module-sys/README.md#caveats-and-good-practices)

# Adding 3rd party library

Please follow information here: [third party libraries](/third-party/ThirdParty.md)

# Adding tests to service

Please follow information here: [unit tests gathering cmake](/test/AddingUnitTests.md)

# Application Manifest

[ApplicationManifest.hpp](/module-services/service-appmgr/include/service-appmgr/ApplicationManifest.hpp) is undocumented.
Its role is to:
- inform what actions the application can perform - actions are asynchronous application capabilities. Action can result in no change in UI, but a major change in logic behavior.
    - a special type of action is `ActionPopup` - it enables applications to show popup
- it can set `AutoLockPolicy` - which defines if the application is on top should phone permit auto-lock

**WARNING** ApplicationManifest does not have a dependency on services.
Applications are started when application-manager is started:
- This means that if your service starts after application-manager service it may cause race condition
- Even if service is started before application - there still is race condition.
    - Starting service happens in SystemManger scope, before `InitHandler()`
