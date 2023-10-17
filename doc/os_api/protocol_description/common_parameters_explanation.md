# Common parameters explanation

- *endpoint* - the target endpoint
```
    invalid = 0,
    deviceInfo = 1,
    update = 2, 
    filesystemUpload = 3,
    backup = 4,
    restore = 5,
    factory = 6,
    contacts = 7,
    messages = 8,
    calllog = 9,
    calendarEvents= 10,
    developerMode = 11,
    bluetooth = 12,
    usbSecurity = 13
```

- *method* - the request methods that indicate the desired action to be performed for a given resource.
  - Get - to retrieve resources only, and not to modify it in any way.
  - Post - to create a new resource into the collection of resources.
  - Put - to update an existing resource, and if the resource doesn’t exist, then don’t create it.
  - Del - to delete a resource.

```
    GET = 1,
    POST = 2,
    PUT = 3,
    DEL = 4
```

- *uuid* - unique payload id, request and the response to it should have the same uuid parameter.
The payload id is 32bits unsigned integer, that wraps around once it reaches its max value.
Both request and response should transport the payload id by a numeric type (in case of JSON format).

- *category* - Optional parameter. It accepts one of three values: *thread*, *message*, *template*.

- *status* - HTTP-like status code.
\* - Accepted response code is used to indicate that the request has been accepted for processing, but the processing is asynchronous. Thanks to that, a client doesn’t have to wait for the result of the processing.
** - SeeOther response code indicates that the resource has finished its work, but instead of sending a response body, it redirects the client to a resource.
*** - Forbidden response code indicates that the resource is not accessible due to phone lock - see Security endpoint (13) 
**** - Locked the response code indicates that the resource is unavailable due to not accepting the EULA - see Security endpoint (13)
```
    OK                  = 200,
    Accepted            = 202, *
    NoContent           = 204,
    SeeOther            = 303, **
    BadRequest          = 400,
    Forbidden           = 403, ***
    NotFound            = 404,
    NotAcceptable       = 406,
    UnprocessableEntity = 422,
    Locked              = 423, ****
    InternalServerError = 500,
    NotImplemented      = 501,
    InsufficientStorage = 507
```

- *SMSType* - Optional parameter. It accepts the following values:
```
    DRAFT   = 1,  // Defines unfinished message that should be loaded in editor for a given thread and may be later scheduled for sending.
    FAILED  = 2,  // Defines message that was queued for sending but this process for some reasons failed.
    INBOX   = 4,  // Defines incoming message.
    OUTBOX  = 8,  // Defines outgoing message.
    QUEUED  = 16, // Defines message that is scheduled for sending.
    INPUT   = 18, // Used only internally on MuditaOS side.
    UNKNOWN = 255 // Type used for initialization.
```
 