Security endpoint (13)
=============================

* [Parameters explanation](#parameters-explanation)
* [Usage examples](#usage-examples)
    * [Get phone lock status](#get-phone-lock-status)
    * [Get phone lock time](#get-phone-lock-time) 
    * [Set phone lock off](#set-phone-lock-off)

## Parameters explanation

[Common parameters explanation](../../protocol_description/common_parameters_explanation.md)

## Usage examples

### Get phone lock status

**Request Payload Structure**

```json
{
  "endpoint":13,
  "method":1,
  "uuid":123,
  "body":{
    "category":"phoneLockStatus"
  }
}
```

**Response Payload Structure**

```json
{
  "endpoint": 13,
  "status": 204,
  "uuid": 123
}
```

Parameters:

- *status* - 204 when phone is unlocked or 403 when phone is locked

**Response Payload Structure**

```json
{
  "endpoint": 13,
  "body": {
    "reason": "2"
  },
  "status": 423,
  "uuid": 123
}
```

Parameters:

- *status* - 423 when phone is unlocked or 403 when phone is locked
- *reason* - block reason, see code snippets below

```c++
    enum class BlockReason
    {
        OnboardingNotFinished = 2,
        BatteryCriticalLevel
    };
```

Access to all edpoints is blocked when:

- Device is locked - 403 status is returned
- Onboarding not finished - 423 status is returned
- Critical battery level - 423 status is returned

### Get phone lock time

**Request Payload Structure**

```json
{
  "endpoint":13,
  "method":1,
  "uuid":123,
  "body":{
    "category":"phoneLockTime"
  }
}
```

**Response Payload Structure**

```json
{
  "endpoint":13,
  "status":200,
  "uuid":123,
  "body":{
    "phoneLockTime":1630919524,
    "timeLeftToNextAttempt":120
  }
}
```

Parameters:
- *phoneLockTime* - timestamp in seconds when the next attempt of phone unlock will be possible
- *timeLeftToNextAttempt* - time left in seconds, after which the next attempt of phone unlock will be possible

**Response Payload Structure when phone is not time locked**

```json
{
  "endpoint":13,
  "status":422,
  "uuid":123
}
```

Parameters:
- *status* - 422 when phone is unlocked, or unlocking phone is possible at this moment

### Set phone lock off

**Request Payload Structure**

```json
{
  "endpoint":13,
  "method":3,
  "uuid":123,
  "body":{
    "phoneLockCode":[1, 1, 1, 1]
  }
}
```

**Response Payload Structure**

```json
{
  "endpoint":13,
  "status":204,
  "uuid":123
}
```

Parameters:
- *phoneLockCode* - code needed to unlock phone as array of integer values
- *status* - 204 if passed phoneLockCode has a correct format (eg. length), 400 otherwise

It is Mudita Center responsibility to check if phone was successfully unlocked by checking phone lock status.