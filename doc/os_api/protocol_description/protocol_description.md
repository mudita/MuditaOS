# Protocol description
## Message structure
**[ message_type | payload_length | payload[0] | payload[1] | ... | payload[payload_length-1]**
```
uint8_t message_type;
uint8_t payload_length[9];
uint8_t payload[payload_length];
```

### Message types   
*Single printable ASCII character.*
```
endpoint = '#'
rawData = '$
```

### Payload length  
*Represented by 9 printable ASCII characters.*

### Payload structure  
*Simple json with some fields. Difference between request and response payload is "method" and "status" fields.*
```
requestPayloadJson:
{
     { "endpoint", endpointNumber },
     { "method", methodNumber},
     { "body", requestBodyJson },
     { "uuid", uuidNumber }
}

responsePayloadJson:
{
     { "endpoint", endpointNumber },
     { "status", statusCode},
     { "body", responseBodyJson },
     { "uuid", uuidNumber }
}

```

##### Endpoint 
*Each endpoint has its unique number.*
```
invalid             = 0
deviceInfo          = 1
contacts            = 7
```
<!--- Update later when documentation will be extended --->
##### Method
*HTTP - like methods. Each has different number. Only in request message.*
```
get     = 1
post    = 2
put     = 3
del     = 4
```

##### Status
*HTTP status codes. Only in response message.*
```
OK                  = 200
Accepted            = 202
NoContent           = 204
SeeOther            = 303
BadRequest          = 400
Forbidden           = 403
NotFound            = 404
NotAcceptable       = 406
InternalServerError = 500
NotImplemented      = 501
```

##### Body
*Actual response from endpoint in response message. In request message its optional and depends on used method.*

## API
### Example

##### Request
```
#000000053{"endpoint":"invalid", "method":"GET", "payload":{"test":"test"}}
```
##### Response
```
#000000413{"body":
{"accessTechnology": "7", "batteryLevel": "99", "batteryState": "1", 
"currentRTCTime": "1611586206", "fsFree": "13903", "fsFreePercent": "100", "fsTotal": "13913", 
"gitBranch": "master", "gitRevision": "34594b42a", "gitTag": "release-0.53.2-46-g34594b42a", 
"networkStatus": "1", "selectedSim": "4", "signalStrength": "4", "trayState": "1", 
"updateHistory": null},
"endpoint": 1, "status": 200, "uuid": 0}
```

### Contacts
##### get contacts count:
```
#000000059{"endpoint":7, "method":1, "uuid":1, "body":{"count":true}}
```
sample response:
```
#000000064{"body": {"count": 76}, "endpoint": 7, "status": 200, "uuid": 1}}}
```
##### get contact by id:
```
#000000053{"endpoint":7, "method":1, "uuid":1, "body":{"id":5}}
```
parameters:
- *id* - contact id

sample response:
```
#000000205{"body": 
{"address": "6 Czeczota St.02600 Warsaw", 
"altName": "Pacha", 
"blocked": false, 
"favourite": false, 
"id": 5, 
"numbers": ["123456789"], 
"priName": "Zofia"}, 
"endpoint": 7, "status": 200, "uuid": 1}
```
##### get contacts ( eg. 5 first contacts) :
```
#000000056{"endpoint":7, "method":1, "uuid":1, "body":{"count":5}}
```


parameters:
- *count* - amount of contacts to be fetched starting from position 0

sample response:
```
#000000846{"body": 
[
{"address": "6 Czeczota St.02600 Warsaw", "altName": "Arbuz", "blocked": false, "favourite": true, "id": 4, "numbers": ["123456789"], "priName": "Alek"},
{"address": "6 Czeczota St.02600 Warsaw", "altName": "Boligłowa", "blocked": false, "favourite": true, "id": 19, "numbers": ["123456789"], "priName": "Alek"},
{"address": "6 Czeczota St.02600 Warsaw", "altName": "Boligłowa", "blocked": false, "favourite": true, "id": 22, "numbers": ["123456789"], "priName": "Grażyna"},
{"address": "6 Czeczota St.02600 Warsaw", "altName": "Boligłowa", "blocked": false, "favourite": true, "id": 20, "numbers": ["123456789"], "priName": "Zofia"},
{"address": "6 Czeczota St.02600 Warsaw", "altName": "Bubel", "blocked": false, "favourite": true, "id": 44, "numbers": ["123456789"], "priName": "Brian"}
]
 , "endpoint": 7, "status": 200, "uuid": 1}
 ```

 ##### get contacts by setting count and offset
 ```
 #000000069{"endpoint":7, "method":1, "uuid":1, "body":{"count":3, "offset":20}}
 ```
 parameters:
- *count* - amount of contacts to be fetched   
- *offset* - offset on  the contact list

sample response:
```
#000000538{"body":
[
{"address": "6 Czeczota St.02600 Warsaw", "altName": "Kwasigroch", "blocked": false, "favourite": true, "id": 73, "numbers": ["123456789"], "priName": "Roland"}, 
{"address": "6 Czeczota St.02600 Warsaw", "altName": "Małolepszy", "blocked": false, "favourite": true, "id": 13, "numbers": ["123456789"], "priName": "Alek"}, 
{"address": "6 Czeczota St.02600 Warsaw", "altName": "Małolepszy", "blocked": false, "favourite": true, "id": 18, "numbers": ["123456789"], "priName": "Mariola"}
], 
"endpoint": 7, "status": 200, "uuid": 1}
```

##### add contact:  

```
#000000193{"endpoint":7, "method":3, "uuid":124, "body":
{"address": "6 Czeczota St.\n 02600 Warsaw", 
"altName": "Turk", 
"blocked": true, 
"favourite": true, 
"numbers": ["123456789"], 
"priName": "Tolek"}}
```

parameters:
- as seen on listing above

sample response:
```
#000000063{"body": {"id": 79}, "endpoint": 7, "status": 200, "uuid": 124}
```

##### update contact:

```
#000000202{"endpoint":7, "method":2, "uuid":123, "body": 
{"address": "6 Czeczota St.02600 Warsaw", 
"altName": "Cicero", 
"blocked": true, 
"favourite": true, 
"id": 25, 
"numbers": ["123456789"], 
"priName": "Wacek"}}
```
parameters:
- as seen on listing above

sample response:
```
#000000043{"endpoint": 7, "status": 204, "uuid": 123}
```
or bad one:
```
#000000043{"endpoint": 7, "status": 500, "uuid": 123}
```

##### remove contact:
```
#000000057{"endpoint":7, "method":4, "uuid":123, "body":{"id": 23}}
```
parameters:
- *id* - id of contact to be removed

sample response:
```
#000000043{"endpoint": 7, "status": 204, "uuid": 123}
```
