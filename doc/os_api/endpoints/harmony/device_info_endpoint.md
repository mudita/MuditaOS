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

```json
#000000036
{
"endpoint": 1,
"method": 1,
"uuid": 123
}
```

Parameters:

- *endpoint* - endpoint type (it’s set to 1, which means **“deviceInfo”** type) - see code snippets below
- *method* - method type (it’s set to 1, which means **“get”** method) - see code snippets below
- *uuid* - unique payload id

**Response Payload Structure**

```json
{
  "body": {
    "batteryLevel": "68",
    "batteryState": "2",
    "deviceSpaceTotal": "4005",
    "systemReservedSpace": "798",
    "usedUserSpace": "2",
    "gitBranch": "master",
    "gitRevision": "58e3688f6",
    "gitTag": "release-0.73.1-rc1-11-g58e3688f6",
    "currentRTCTime": "1626085629",
    "version": "0.73.1",
    "serialNumber": "12345678901234",
    "recoveryStatusFilePath": "path/to/recovery_status",
    "updateFilePath": "path/to/update_package",
    "backupFilePath": "path/to/backup_package",
    "syncFilePath": "path/to/sync_package",
    "mtpPath": "/path/to/music/assets/or/other/available/from/MTP",
    "onboardingState": "1"
  },
  "endpoint": 1,
  "status": 200,
  "uuid": 123
}
```

Parameters:

- *body*  - actual response from endpoint in response message
- *batteryLevel* - battery level (in percentage)
- *batteryState* - battery state - see code snippets below
- *currentRTCTime* - current timestamp from RTC (in seconds)
- *deviceToken* - unique device token
- *deviceSpaceTotal* - total storage space on the device (in Mebibytes)
- *systemReservedSpace* - storage space on the device reserved for the OS (in Mebibytes)
- *usedUserSpace* - storage space on the device used for user files on the user partition (in Mebibytes)
- *gitBranch* - git branch from which the system was built
- *gitRevision* - git commit sha from which the system was built
- *gitTag* - git tag from which the system was built
- *endpoint* - endpoint type (it’s set to 1, which means “deviceInfo” type)
- *status* - http like status code - see code snippets below
- *uuid* - unique payload id, same as in request message
- *version* - os version string
- *serialNumber* - device serial number (14 digits)
- *recoveryStatusFilePath* - location of the PureRecovery status file
- *updateFilePath* - location of the update package
- *backupFilePath* - location of the backup package
- *syncFilePath* - location of the sync package
- *mtpPath* - path to the catalog available via MTP
- *onboardingState* - information about current onboarding state

```c++
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

```c++
enum class OnboardingState{
    InProgress,
    Finished
};
```

### Get a List of logs and crash dumps

**Request Payload Structure**

```json
{
  "endpoint": 1,
  "method": 1,
  "uuid": 123,
  "body": {
    "fileList": 0
  }
}
```

Parameters:

- *endpoint* - endpoint type (it’s set to 1, which means “**deviceInfo**” type) - see code snippets below
- *method* - method type (it’s set to 1, which means “**get**” method) - see code snippets below
- *uuid* - unique payload id
- *fileList* - indicates the type of diagnostic data to be listed - see below

```c++
enum class DiagnosticsFileList
{
    LOGS = 0,
    CRASH_DUMPS,
    TDB
}
```

**Response Payload Structure**  
List of available log files

```json
{
  "endpoint": 1,
  "status": 200,
  "uuid": 123,
  "body": {
    "files": [
      "/sys/user/MuditaOS.log",
      "/sys/user/MuditaOS.log.1",
      "/sys/user/MuditaOS.log.2"
    ]
  }
}
```

Empty response, no log/crash dump files available on a device

```json
#000000036
{
"endpoint": 1,
"status": 204,
"uuid": 123
}
```