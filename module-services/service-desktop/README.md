Service Desktop
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

Testing App
=================
The testing app for this service is written in electron/JS and located
at https://github.com/muditacom/PureNode.
To use the code you need a working npm from https://www.npmjs.com/

```
# git clone git@github.com:muditacom/PureNode.git
# cd PureNode
# npm install
# npm start
```

In case starting fails with permission problems for the chrome-sandbox binary you need to fix
the permissions as stated in the error, this is for Linux.

Testing on PureOS
=================
At the moment the service is disabled due to USB driver issues. To enable the
service on the faulty driver you need to uncomment the relevant line in main.cpp

````
ret |= sys::SystemManager::CreateService(std::make_shared<ServiceDesktop>(), sysmgr.get());

````
Once enabled, load the OS and connect the USB3 cable to the phone, ypu should see
a new serial CDC/ACM device in your OS, it should look like below:
````asm
# lsusb
Bus 003 Device 060: ID 1fc9:0094 NXP Semiconductors 

# lsusb -v
Bus 003 Device 060: ID 1fc9:0094 NXP Semiconductors 
Device Descriptor:
  bLength                18
  bDescriptorType         1
  bcdUSB               2.00
  bDeviceClass            2 Communications
  bDeviceSubClass         0 
  bDeviceProtocol         0 
  bMaxPacketSize0        64
  idVendor           0x1fc9 NXP Semiconductors
  idProduct          0x0094 
  bcdDevice            1.01
  iManufacturer           1 MUDITA
  iProduct                2 PURE
  iSerial                 3 1040
  bNumConfigurations      1
  Configuration Descriptor:
    bLength                 9
    bDescriptorType         2
    wTotalLength       0x0043
    bNumInterfaces          2
    bConfigurationValue     1
    iConfiguration          0 
    bmAttributes         0xc0
      Self Powered
    MaxPower              100mA

````

if that works the PureNode app should list the phone and be able to connect to it.

![screenshot when phone detected](/module-services/service-desktop/doc/pure_update_manual_0.jpg)
