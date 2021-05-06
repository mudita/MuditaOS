Settings
========

# settings::Settings API

settings::Settings class aims to store service/app specific non volatile data and provide it in runtime.

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
