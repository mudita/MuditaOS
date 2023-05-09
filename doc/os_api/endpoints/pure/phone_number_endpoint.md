Phone Number endpoint (15)
=============================

* [Parameters explanation](#parameters-explanation)
* [Usage examples](#usage-examples)
    * [Get Number by NumberId](#Get-number-by-numberId)

## Parameters explanation

[Common parameters explanation](../../protocol_description/common_parameters_explanation.md)

## Usage examples

### Get Number by NumberId

**Request Payload Structure**

```json
#000000036
{
"endpoint": 15,
"method": 1,
"uuid": 123,
"body": {
"numberIDs": [1, 2, 3]
}
}
```

Parameters:

- *numberIDs* - array of unique ID number of specific number in database

**Response Payload Structure**

```json
{
  "body": [
    {
      "numberID": 1,
      "number": "123456789"
    },
    {
      "numberID": 2,
      "number": "223456789"
    },
    {
      "numberID": 3,
      "number": "323456789"
    }
  ],
  "endpoint": 15,
  "status": 200,
  "uuid": 123
}
```

Parameters:
Result is an array of numberID and phone number correspond to it
- *numberID*  - unique ID number of specific phone number in database
- *number* - phone number
