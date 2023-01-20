Contacts endpoint (7)
=============================

* [Parameters explanation](#parameters-explanation)
* [Pagination](#pagination)
* [Usage examples](#usage-examples)
    * [Get contacts count](#get-contacts-count)
    * [Get contacts using offset and limit](#get-contacts-using-offset-and-limit)
    * [Get a contact by ID](#get-a-contact-by-id)
    * [Add a new contact](#add-a-new-contact)
    * [Update a contact](#update-a-contact)
    * [Delete a contact](#delete-a-contact)

## Parameters explanation

[Common parameters explanation](../../protocol_description/common_parameters_explanation.md)

## Pagination

[Pagination](../../protocol_description/pagination.md)

## Usage examples

### Get contacts count

**Note: For development/testing purposes only.**
**Request Payload Structure**

```json
{
  "endpoint": 7,
  "method": 1,
  "uuid": 1,
  "body": {
    "count": true
  }
}
```

**Response Payload Structure**

```json
{
  "body": {
    "count": 76
  },
  "endpoint": 7,
  "status": 200,
  "uuid": 1
}
```

### Get contacts using offset and limit

See: [Pagination](../../protocol_description/pagination.md)
**Request Payload Structure**

```json
{
  "endpoint": 7,
  "method": 1,
  "uuid": 123,
  "body": {
    "offset": 0,
    "limit": 2
  }
}
```

**Response Payload Structure**

```json
{
  "body": {
    "entries": [
      {
        "address": "Czeczota 6, 02-600 Warsaw, Poland",
        "altName": "Abraham",
        "email": "Abraham@example.com",
        "blocked": false,
        "favourite": true,
        "ice": true,
        "id": 1,
        "numbers": [
          "123456789"
        ],
        "numbersIDs": [
          "1"
        ],
        "speedDial": "",
        "priName": "Olivia",
        "note": ""
      },
      {
        "address": "Czeczota 6, 02-600 Warsaw, Poland",
        "altName": "Alsop",
        "email": "Alsop@example.com",
        "blocked": false,
        "favourite": true,
        "ice": true,
        "id": 3,
        "numbers": [
          "123456789"
        ],
        "numbersIDs": [
          "1"
        ],
        "speedDial": "7",
        "priName": "Charles",
        "note": ""
      }
    ],
    "totalCount": 76
  },
  "endpoint": 7,
  "status": 200,
  "uuid": 123
}
```

Parameters:

- *address*  - contact address
- *altName* - secondary name of contact
- *email* - contact’s email address
- *blocked* - true if contact is blocked
- *favourite* - true if contact is on favorites list
- *ice* - true if contact is on in case of emergency list
- *id* - contact id
- *numbers* - list of contact numbers
- *numbersIDs* - list of numbers ids corresponded to *numbers*
- *speedDial* - speed dial number
- *priName* - primary name of contact
- *note* - note attached to contact

### Get a contact by ID

**Note: For development/testing purposes only.**
**Request Payload Structure**

```json
{
  "endpoint": 7,
  "method": 1,
  "uuid": 1,
  "body": {
    "id": 5
  }
}
```

**Response Payload Structure**

```json
{
  "body": {
    "address": "Czeczota 6, 02-600 Warsaw, Poland",
    "altName": "Arnold",
    "email": "Arnold@example.com",
    "blocked": false,
    "favourite": false,
    "ice": true,
    "id": 5,
    "numbers": [
      "123456789"
    ],
    "numbersIDs": [
      "1"
    ],
    "speedDial": "6",
    "priName": "Donna",
    "note": "likes oranges"
  },
  "endpoint": 7,
  "status": 200,
  "uuid": 1
}
```

### Add a new contact

**Request Payload Structure**

```json
{
  "endpoint": 7,
  "method": 2,
  "uuid": 123,
  "body": {
    "address": "Czeczota 6, 02-600 Warsaw, Poland",
    "altName": "Turk",
    "email": "Turk@example.com",
    "blocked": false,
    "favourite": true,
    "ice": false,
    "numbers": [
      "123456789"
    ],
    "speedDial": "",
    "priName": "Tolek",
    "note": "has a cat"
  }
}
```

**Response Payload Structure when contact is successfully added**

```json
{
  "body": {
    "id": 77
  },
  "endpoint": 7,
  "status": 200,
  "uuid": 123
}
```

Parameters:

- *id*  - id of newly created contact

**Response Payload Structure when contact is matched as a duplicate**

```json
{
  "body": {
    "id": 76
  },
  "endpoint": 7,
  "status": 409,
  "uuid": 123
}
```

Parameters:

- *id*  - id of duplicated contact

### Update a contact

**Request Payload Structure**

```json
{
  "endpoint": 7,
  "method": 3,
  "uuid": 123,
  "body": {
    "id": 77,
    "address": "Czeczota 6, 02-600 Warsaw, Poland",
    "altName": "Turk",
    "email": "Turk@example.com",
    "blocked": false,
    "favourite": true,
    "ice": false,
    "numbers": [
      "123456789"
    ],
    "speedDial": "",
    "priName": "Tolek",
    "note": "has a cat"
  }
}
```

**Response Payload Structure when contact is successfully updated**

```json
{
  "endpoint": 7,
  "status": 204,
  "uuid": 123
}
```

Warning:
In the current implementation, there is no duplicate detection while updating contact.
Example:
We have contact A with number 123 and contact B with number 456.
Contact A is updated with a new number - 456. Number 456 is silently unattached from contact B and assigned to contact
A. Finally, we have contact A with number 456 and contact B with no number.

### Delete a contact

**Request Payload Structure**

```json
{
  "endpoint": 7,
  "method": 4,
  "uuid": 1,
  "body": {
    "id": 77
  }
}
```

**Response Payload Structure**

```json
{
  "endpoint": 7,
  "status": 204,
  "uuid": 1
}
```