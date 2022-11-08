Device Info endpoint (1)
=============================
* [Parameters explanation](#parameters-explanation)
* [Usage examples](#usage-examples)
  * [Get Device Information](#get-device-information)
  * [Get a List of logs and crash dumps](#get-a-list-of-logs-and-crash-dumps)


## Parameters explanation
[Common parameters explanation](../../protocol_description/common_parameters_explanation.md)

## Usage examples
### Get Device Information
**Request Payload Structure**
```
#000000036
{
   "endpoint":1,
   "method":1,
   "uuid":123
}
```

 Parameters:

 - *endpoint* - endpoint type (it’s set to 1, which means **“deviceInfo”** type) - see code snippets below
 - *method* - method type (it’s set to 1, which means **“get”** method) - see code snippets below
 - *uuid* - unique payload id

 “*body*” parameter is not needed in this case.

```
enum class EndpointType
{
    invalid = 0,
    deviceInfo,
    update, 
    filesystemUpload,
    backup,
    restore,
    factory,
    contacts,
    messages,
    calllog,
    developerMode,
};
```
```
enum class Method
{
    get = 1,
    post, 
    put,
    del
};
```

**Response Payload Structure**
```
{
   "body":{
      "accessTechnology":"7",
      "batteryLevel":"68",
      "batteryState":"2",
      "caseColour":"gray",
      "currentRTCTime":"1626085629",
      "deviceToken":"<32-character string"
      "deviceSpaceTotal":"14945",
      "systemReservedSpace":"2042",
      "usedUserSpace":"440",
      "gitBranch":"master",
      "gitRevision":"58e3688f6",
      "gitTag":"release-0.73.1-rc1-11-g58e3688f6",
      "networkOperatorName":"Play",
      "networkStatus":"1",
      "selectedSim":"0",
      "signalStrength":"2",
      "trayState":"1",
      "version": "0.73.1",
      "serialNumber": "12345678901234",
      "backupLocation": "path/to/directory",
      "syncPackageLocationLocation": "path/to/directory"
   },
   "endpoint":1,
   "status":200,
   "uuid":123
}
```

 Parameters:
 
 - *body*  - actual response from endpoint in response message
 - *accessTechnology* - network access technology - see code snippets below
 - *batteryLevel* - battery level (in percentage)
 - *batteryState* - battery state - see code snippets below
 - *caseColour* - colour of the Pure’s case (gray or black)
 - *currentRTCTime* - current timestamp from RTC (in seconds)
 - *deviceToken* - unique device token
 - *deviceSpaceTotal* - total storage space on the device (in Mebibytes)
 - *systemReservedSpace* - storage space on the device reserved for the OS (in Mebibytes)
 - *usedUserSpace* - storage space on the device used for user files on the user partition (in Mebibytes)
 - *gitBranch* - git branch from which the system was built 
 - *gitRevision* - git commit sha from which the system was built
 - *gitTag* - git tag from which the system was built
 - *networkOperatorName* - current network operator name
 - *networkStatus* - current status of network - see code snippets below
 - *selectedSim* - selected SIM status - see code snippets below
 - *signalStrength* - number of signal strength indicator bars - from 0 to 4
 - *trayState* - state of SIM tray - see code snippets below
 - *endpoint* - endpoint type (it’s set to 1, which means “deviceInfo” type)
 - *status* - http like status code - see code snippets below
 - *uuid* - unique payload id, same as in request message
 - *version* - os version string
 - *serialNumber* - device serial number (14 digits)
 - *"backupLocation"* - path to the location of backup files on device file-system
 - *"syncPackageLocation"* - path to the location of sync package on device filesystem

```
struct Network
{
    enum class Status
    {
        NotRegistered,
        RegisteredHomeNetwork,
        Searching,
        RegistrationDenied,
        Unknown,
        RegisteredRoaming
    }

    enum class AccessTechnology
    {
        Gsm   = 0x00,
        Utran = 0x02,
        GsmWEgprs,
        UtranWHsdpa,
        UtranWHsupa,
        UtranWHsdpaAndWHsupa,
        EUtran, // 7
        Unknown = 0xFF
    }
};
```
```
struct Battery
{
    enum class State
    {
        Discharging,
        Charging,
        PluggedNotCharging,
    }
};
```
```
enum class SIM
{
    SIM1 = 0,
    SIM2,
    SIM_FAIL,
    SIM_UNKNOWN,
    NONE,
}
```
```
enum class Tray
{
    OUT = 0,
    IN
}
```


### Get a List of diagnostics files
**Request Payload Structure**
```
{
   "endpoint":1,
   "method":1,
   "uuid":123,
   "body": { "fileList":0 }
}
```

 Parameters:
 - *endpoint* - endpoint type (it’s set to 1, which means “**deviceInfo**” type) - see code snippets below
 - *method* - method type (it’s set to 1, which means “**get**” method) - see code snippets below
 - *uuid* - unique payload id
 - *fileList* - indicates the type of diagnostic data to be listed - see below

```
enum class DiagnosticsFileList
{
    LOGS = 0,
    CRASH_DUMPS,
    TDB
}
```

**Response Payload Structure**  
List of available log files
```
{
   "endpoint":1,
   "status":200,
   "uuid":123,
   "body": {"files":[ "/sys/user/MuditaOS.log",
                      "/sys/user/MuditaOS.log.1",
                      "/sys/user/MuditaOS.log.2" ] }
}
```
Empty response, no log/crash dump files available on a device
```
#000000036
{
   "endpoint":1,
   "status":204,
   "uuid":123
}
```