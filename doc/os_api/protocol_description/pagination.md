# Pagination

## Abstract
Because of the low amount of RAM memory, the PurePhone is not able to transfer huge amount of data (messages, contacts, notes, etc.) in a single message. This is the reason why responses to the GET requests need to be paged and shared with the Mudita Center in chunks.

## Request
In order to GET data from the PurePhone in a paged form, Mudita Center should send the following request to an endpoint:
```
#000000069{
  "endpoint": 7,
  "method": 1,
  "uuid": 1103,
  "body": {
    "limit": 9,
    "offset": 0
  }
}
```

Where: 
- *"endpoint"*: 7 means Contacts Endpoint
- *"method"*: 1 means GET method
- *“body.offset”* : 0 and "body.limit”: 9 mean that the elements from 1st to 9th are requested 

**Note: collections are zero-indexed**
If the response doesn’t contain the body.offset value, the PurePhone is still going to respond in the paged form.
The following request:
```

#000000069{
  "endpoint": 7,
  "method": 1,
  "uuid": 1103,
  "body": {
    "limit": 9
  }
}
```
requests for the elements from 1st to 9th, and it may be considered identical to the previous one.

###Get all elements
Sending the following request:

```
#000000069{
  "endpoint": 7,
  "method": 1,
  "uuid": 1103
}
```
will request for all elements in the collection. The elements will also be returned in the paged form.

## Response
**Note:** Let’s assume the Contacts Endpoint is able to return only 3 elements per page at most (It’s configured on the PurePhone side).

### Success
The successful response to the request above looks like:
```
#000000540{
  "endpoint": 7,
  "status": 200,
  "uuid": "1103",
  "body": {
    "totalCount": 4,
    "nextPage": {
      "offset": 3,
      "limit": 1
    },
    "entries": [
      {
        "address": "6 Czeczota St.02600 Warsaw",
        "altName": "Kwasigroch",
        "blocked": false,
        "favourite": true,
        "id": 73,
        "numbers": [
          "500618547"
        ],
        "priName": "Roland"
      },
      {
        "address": "6 Czeczota St.02600 Warsaw",
        "altName": "Małolepszy",
        "blocked": false,
        "favourite": true,
        "id": 13,
        "numbers": [
          "500011043"
        ],
        "priName": "Alek"
      },
      {
        "address": "6 Czeczota St.02600 Warsaw",
        "altName": "Małolepszy",
        "blocked": false,
        "favourite": true,
        "id": 18,
        "numbers": [
          "500656267"
        ],
        "priName": "Mariola"
      }
    ]
  }
}
```

##### Next request
Once the response is received, Mudita Center knows the information about the next page (carried in the ”nextPage” object in JSON). Now, Mudita Center shall request the next page the following way:

```
#000000069{
  "endpoint": 7,
  "method": 1,
  "uuid": 1104,
  "body": {
    "limit": 1,
    "offset": 3
  }
}
```
and it will receive the following response for the last page:

```
#000000540{
  "endpoint": 7,
  "status": 200,
  "uuid": "1104",
  "body": {
    "totalCount": 4,
    "entries": [
      {
        "address": "6 Czeczota St.02600 Warsaw",
        "altName": "Kwasigroch",
        "blocked": false,
        "favourite": true,
        "id": 80,
        "numbers": [
          "500618547"
        ],
        "priName": "Roland"
      }
    ]
  }
}
```
**Note:** If the response points to the last page, it doesn’t carry the nextPage information.

### Failure
TBD

