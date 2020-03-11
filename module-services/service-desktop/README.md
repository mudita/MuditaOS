# Service Desktop
=================

This service is handling communication between Mudita Deskatop App and PurePhone.

**Note:
Service desktop is disabled by default.
To turn it on, please uncomment this line in mail.cpp:**

`        ret |= sys::SystemManager::CreateService(std::make_shared<ServiceDesktop>(), sysmgr.get());
`

### Protocol description

#### Message structure

**[ message_type | payload_length | payload[0] | payload[1] | ... | payload[payload_length-1]**

```
uint8_t message_type;
uint8_t payload_length[9];
uint8_t payload[payload_length];
```

##### Message types
*Single printable ASCII character.*
```
enum class Type
    {
        endpoint = '#',
        rawData = '$'
    };
```
##### Payload length
*Represented by 9 printable ASCII characters.*

##### Payload structure
*Simple json with some fields. Difference between request and response payload is "method" and "status" fields.*
```
requestPayloadJson:
{
     { "endpoint", endpointNumber },
     { "method", methodNumber}, 
     { "body", requestBodyJson },
     { "uuid", uuidString }
}
```
```
responsePayloadJson:
{
     { "endpoint", endpointNumber },
     { "status", statusCode}, 
     { "body", responseBodyJson },
     { "uuid", uuidString }
}
```
###### Endpoint
*Each endpoint has its unique number.*

```
enum class Endpoint
{
    deviceInfo = 1,
    update
};
```

###### Method
*HTTP - like methods. Each has different number. Only in request message.*

```
enum class Method
{
    get = 1,
    post,
    put,
    del
};
```

###### Status
*HTTP status codes. Only in response message.*

```
enum class Code
{
    OK = 200,
    BadRequest = 400,
    InternalServerError = 500
};
```

###### Body
*Actual response from endpoint in response message. In request message its optional and depends on used method.*

##### Example request

```#000000053{"endpoint":1, "method":1, "payload":{"test":"test"}}```

##### Example response

```#000000095{"endpoint": 1, "status": 200, "body": {"charging": true, "level": 75, "maximumCapacity": 100}}```
