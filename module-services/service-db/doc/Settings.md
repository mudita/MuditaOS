settings::Settings
==================

It's a class that aims to store service/app specific non volatile data and provide it in runtime.
It works with Settings agent provides with a backend to serve all services and apps data to it.

What it essentially does is:
1. provide with global cache for data on RAM via SettingsCache
2. provide simple API to get and set value

Caveats:
1. It's a getter/setter code. While we can build business logic on it we should use system notifications to do so
2. It doesn't provide us with information if there is data in it - if there is none it will return an empty string
3. It doesn't provide us with any abstraction to store our own data structures. To do so one will have to i.e. store settings as string dump of i.e. JSON of msgpack

**IMPORTANT:**
You need to initialize settings::Settings in Service/Application when init function, not in the constructor.
Please see [module-sys documentation](../../module-sys/README.md#Services)

# How to add settings to use

1. add `settings::Settings` handle to your service i.e. in class definition:
```
settings::Settings settings;
```

Then after/in init handler call:
```
settings.init(service::ServiceProxy{service->weak_from_this()});
```
To initialize it.

At this moment you can get/set data from settings. Follow [settings Gudeline](#Guideline) for more information

## Guideline

value ownership - service/application own value means it's has to be used by that application and that application only.

**Do use**:  
* getValue to get
* setValue to set
* services API to drive Application -> Service requests i.e. set sim card

**Don't**:  
* drive apps via settings::Settings callbacks it can be used for initialization of data on app/service start-up only
* implement new caching mechanisms
* use other applications/services settings - use API to get values
* ApplicationSettings name doesn't mean that it can use settings as it wants it has follow general rules (use API) - it shouldn't access values it doesn't own

**Shouldn’t**:  
* access global scope settings. Right now these are only in: ApplicationSettings, ApplicationManager, ServiceCellular, ApplicationDesktop. Possibly we could remove these values from the global scope at all.
