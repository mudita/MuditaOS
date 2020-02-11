# Service Desktop
=================

This service is handling communication between Mudita Deskatop App and PurePhone.

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
Invalid = '"'
Endpoint = '#'
RawData = '$'
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
     { "body", requestBodyJson }
}
```
```
responsePayloadJson:
{
     { "endpoint", endpointNumber },
     { "status", statusCode}, 
     { "body", responseBodyJson }
}
```
###### Endpoint
*Each endpoint has its unique number.*

```
enum Endpoint: uint8_t { Battery, Backups, DeviceInfo, Network, Storage };
```

###### Method
*HTTP - like methods. Each has different number. Only in request message.*

```
    enum class Method: uint8_t
    {
        Get,
        Post,
        Put,
        Delete
    };
```

###### Status
*HTTP status codes. Only in response message.*


###### Body
*Actual response from endpoint in response message. In request message its optional and depends on used method.*

##### Example request

```#000000053{"endpoint":0, "method":0, "payload":{"test":"test"}}```

##### Example response

```#000000095{"endpoint": 0, "status": 200, "body": {"charging": true, "level": 75, "maximumCapacity": 100}}```