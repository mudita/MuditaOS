Messages endpoint (8)
=============================

* [Parameters explanation](#parameters-explanation)
* [Pagination](#pagination)
* [Usage examples](#usage-examples)
    * [Threads](#threads)
        * [Get threads using offset and limit](#get-threads-using-offset-and-limit)
        * [DEPRECATED Get a thread by ID](#deprecated-get-a-thread-by-id)
        * [Set a thread as read / unread](#set-a-thread-as-read--unread)
        * [Delete a thread](#delete-a-thread)
    * [Messages](#messages)
        * [Get messages count](#get-messages-count)
        * [Get messages using offset and limit](#get-messages-using-offset-and-limit)
        * [Get a message by ID](#get-a-message-by-id)
        * [Get messages by thread ID](#get-messages-by-thread-id)
        * [Add a new message](#add-a-new-message)
        * [Update draft message](#update-draft-message)
        * [Delete a message by ID](#delete-a-message-by-ID)
    * [Templates](#templates)
        * [Get a templates count](#get-a-templates-count)
        * [Get templates using offset and limit](#get-templates-using-offset-and-limit)
        * [Get a message template by ID](#get-a-message-template-by-id)
        * [Change a message template body](#change-a-message-template-body)
        * [Change a message template order](#change-a-message-template-order)
        * [Add a message template](#add-a-message-template)
        * [Delete a message template by id](#delete-a-message-template-by-id)

## Parameters explanation

[Common parameters explanation](../../protocol_description/common_parameters_explanation.md)

## Pagination

[Pagination](../../protocol_description/pagination.md)

## Usage examples

## Threads

### Get threads using offset and limit

See: [Pagination](../../protocol_description/pagination.md)
**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 1,
  "uuid": 123,
  "body": {
    "category": "thread",
    "offset": 0,
    "limit": 3
  }
}
```

**Response Payload Structure**

```json
{
  "body": {
    "entries": [
      {
        "isUnread": true,
        "lastUpdatedAt": 1574335694,
        "messageCount": 6,
        "messageSnippet": "Wiadomo<0xc5><0x9b><0xc4><0x87> testowa.",
        "messageType": 1,
        "number": "123456789",
        "numberID": "1",
        "threadID": 1
      },
      {
        "isUnread": true,
        "lastUpdatedAt": 5,
        "messageCount": 1,
        "messageSnippet": "Lorem ipsum dolor sit amet, consectetur adipiscing",
        "messageType": 16,
        "number": "123456789",
        "numberID": "1",
        "threadID": 5
      },
      {
        "isUnread": false,
        "lastUpdatedAt": 5,
        "messageCount": 1,
        "messageSnippet": "cos tam",
        "messageType": 8,
        "number": "123456789",
        "numberID": "1",
        "threadID": 6
      }
    ],
    "nextPage": {
      "limit": 0,
      "offset": 0
    },
    "totalCount": 3
  },
  "endpoint": 8,
  "status": 200,
  "uuid": 123
}
```

Parameters:

- *totalCount*  - total number of threads on the PurePhone side
- *nextPage* - an offset and a limit that should be used to request the next page - in the example first current page
  consists of 3 elements. Because totalCount is 4 nextPage field is available.
- *threadID* - unique ID of the thread
- *lastUpdatedAt* - date of the last modification of the thread, epoch timestamp in seconds
- *messageCount* - number of messages in the thread
- *messageSnippet* - the first row of the last message in a given thread - text is encoded in UTF8 and has up to 45
  characters
- *isUnread* - true, if at least one unread message in the thread
- *messageType* - type of the last SMS in thread. (
  See: [Common parameters explanation](../../protocol_description/common_parameters_explanation.md))
- *number* - phone number associated with the given message thread
- *numberID* - id number corresponded to *number*

In the above example requested 7 elements cannot be met because *totalCount* is 4. Additionally, the response is split
into pages. The nextPage says how many elements (*limit*) and from what offset (*offset*) need to be requested to
reach
the *totalCount* or *limit* (whatever comes first). When field *nextPage* does not exist in the response it means that
the
request was completed within the current page.

### DEPRECATED Get a thread by ID

**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 1,
  "uuid": 123,
  "body": {
    "category": "thread",
    "threadID": 8
  }
}
```

**Response Payload Structure**

```json
{
  "body": {
    "isUnread": false,
    "lastUpdatedAt": 1650656989,
    "messageCount": 1,
    "messageSnippet": "Hello",
    "messageType": 1,
    "threadID": 8
  },
  "endpoint": 8,
  "status": 200,
  "uuid": 123
}
```

### Set a thread as read / unread

**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 3,
  "uuid": 123,
  "body": {
    "category": "thread",
    "threadID": 1,
    "isUnread": true
  }
}
```

Parameters:

- *isUnread* - set to true, if the thread should be marked as unread, or false if it should be marked as read

**Response Payload Structure**

```json
{
  "endpoint": 8,
  "status": 204,
  "uuid": 123
}
```

### Delete a thread

**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 4,
  "uuid": 123,
  "body": {
    "category": "thread",
    "threadID": 1
  }
}
```

**Response Payload Structure**

```json
{
  "endpoint": 8,
  "status": 204,
  "uuid": 123
}
```

## Messages

### Get messages count

**Note: For development/testing purposes only.**
**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 1,
  "uuid": 123,
  "body": {
    "category": "message",
    "count": true
  }
}
```

**Response Payload Structure**

```json
{
  "body": {
    "count": 13
  },
  "endpoint": 8,
  "status": 200,
  "uuid": 123
}
```

### Get messages using offset and limit

See: [Pagination](../../protocol_description/pagination.md)
**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 1,
  "uuid": 123,
  "body": {
    "category": "message",
    "offset": 0,
    "limit": 13
  }
}
```

**Response Payload Structure**

```json
{
  "body": {
    "entries": [
      {
        "messageBody": "nieudane wysy<0xc5><0x82>anie :(",
        "messageID": 6,
        "messageType": 2,
        "createdAt": 1547492320,
        "threadID": 1,
        "number": "345678912"
      },
      {
        "messageBody": "Ci<0xc4><0x99><0xc5><0xbc>ko powiedzie<0xc4><0x87> o czym  ta wiadomo<0xc5><0x9b><0xc4><0x87> jest, ale jest do<0xc5><0x9b><0xc4><0x87> d<0xc5><0x82>uga.",
        "messageID": 7,
        "messageType": 2,
        "createdAt": 1547492320,
        "threadID": 2,
        "number": "456789123"
      },
      {
        "messageBody": "heh?",
        "messageID": 8,
        "messageType": 4,
        "createdAt": 1547492320,
        "threadID": 2,
        "number": "567891234"
      },
      {
        "messageBody": "abc",
        "messageID": 43,
        "messageType": 4,
        "createdAt": 1547492320,
        "threadID": 2,
        "number": "123456789"
      }
    ],
    "nextPage": {
      "limit": 4,
      "offset": 4
    },
    "totalCount": 13
  },
  "endpoint": 8,
  "status": 200,
  "uuid": 123
}
```

Parameters:

- *body*  - actual response from the endpoint
- *totalCount* - total number of messages stored in the PurePhone.
- *nextPage* - an offset and a limit that should be used to request for the next page
- *createdAt* - epoch timestamp in seconds, when message was created
- *threadID* - unique ID of the thread
- *number* - source number of the given message
- *messageID* - self-explanatory
- *messageBody* - text of the message
- *messageType* - SMS message type (
  See: [Common parameters explanation](../../protocol_description/common_parameters_explanation.md))

### Get a message by ID

**Note: For development/testing purposes only.**
**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 1,
  "uuid": 123,
  "body": {
    "category": "message",
    "messageID": 8
  }
}
```

**Response Payload Structure**

```json
{
  "body": {
    "messageBody": "heh?",
    "messageID": 8,
    "messageType": 4,
    "createdAt": 1547492320,
    "threadID": 2,
    "number": "123456789"
  },
  "endpoint": 8,
  "status": 200,
  "uuid": 123
}
```

### Get messages by thread ID

**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 1,
  "uuid": 123,
  "body": {
    "category": "message",
    "threadID": 1,
    "offset": 0,
    "limit": 6
  }
}
```

**Response Payload Structure**

```json
{
  "body": {
    "entries": [
      {
        "messageBody": "najstarsze odb fsjdklafjskldjf",
        "messageID": 1,
        "messageType": 4,
        "createdAt": 1547465101,
        "threadID": 1,
        "number": "123456789"
      },
      {
        "messageBody": "wys",
        "messageID": 2,
        "messageType": 8,
        "createdAt": 1547468701,
        "threadID": 1,
        "number": "123456789"
      },
      {
        "messageBody": "najnowsze odb i ca<0xc5><0x82>kiem 1 1 1 1 d<0xc5><0x82>ugie. d<0xc5><0x82>ugie d<0xc5><0x82>ugie d<0xc5><0x82>ugie, wcale nie kr<0xc3><0xb3>tkie.od nowej lini (\\n); i_teraz_zbyt_d<0xc5><0x82>uga_linia_<0xc5><0xbc>eby_si<0xc4><0x99>_zmie<0xc5><0x9b>ci<0xc4><0x87>",
        "messageID": 4,
        "messageType": 4,
        "createdAt": 1547472320,
        "threadID": 1,
        "number": "123456789"
      }
    ],
    "nextPage": {
      "limit": 2,
      "offset": 4
    },
    "totalCount": 6
  },
  "endpoint": 8,
  "status": 200,
  "uuid": 123
}
```

### Add a new message

**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 2,
  "uuid": 123,
  "body": {
    "category": "message",
    "number": "+48236154528",
    "messageBody": "This is a new message",
    "messageType": 1
  }
}
```

Parameters:

- (optional) *messageType* - SMS message type (
  See: [Common parameters explanation](../../protocol_description/common_parameters_explanation.md)). Needed only if
  adding Draft type
  message (1).

**Success Response Payload Structure**

```json
{
  "body": {
    "messageBody": "This is a new message",
    "messageID": 8,
    "messageType": 4,
    "createdAt": 1547492320,
    "threadID": 2
  },
  "endpoint": 8,
  "status": 200,
  "uuid": 123
}
```

**Failure Response Payload Structure**
E.g. message body too long.

```json
{
  "endpoint": 8,
  "status": 400,
  "uuid": 123
}
```

### Update draft message

**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 3,
  "uuid": 123,
  "body": {
    "category": "message",
    "messageBody": "This is a changed draft message",
    "messageID": 8,
    "messageType": 1
  }
}
```

**Response Payload Structure**

```json
{
  "endpoint": 8,
  "status": 204,
  "uuid": 123
}
```

### Delete a message by ID

**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 4,
  "uuid": 123,
  "body": {
    "category": "message",
    "messageID": 2
  }
}
```

**Response Payload Structure**

```json
{
  "endpoint": 8,
  "status": 204,
  "uuid": 123
}
```

## Templates

### Get a templates count

**Note: For development/testing purposes only.**
**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 1,
  "uuid": 123,
  "body": {
    "category": "template",
    "count": true
  }
}
```

**Response Payload Structure**

```json
{
  "body": {
    "count": 6
  },
  "endpoint": 8,
  "status": 200,
  "uuid": 123
}
```

### Get templates using offset and limit

**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 1,
  "uuid": 123,
  "body": {
    "category": "template",
    "offset": 2,
    "limit": 3
  }
}
```

*Parameters were described in previous messages.*

**Response Payload Structure**

```json
{
  "body": {
    "entries": [
      {
        "lastUsedAt": 2,
        "order": 1,
        "templateBody": "I'll be there in 15 minutes",
        "templateID": 3
      },
      {
        "lastUsedAt": 1,
        "order": 2,
        "templateBody": "Some test tmplate number, which is too long to be displayed.",
        "templateID": 4
      },
      {
        "lastUsedAt": 0,
        "order": 3,
        "templateBody": "No.",
        "templateID": 5
      }
    ],
    "nextPage": {
      "limit": 1,
      "offset": 5
    },
    "totalCount": 6
  },
  "endpoint": 8,
  "status": 200,
  "uuid": 123
}
```

Parameters:

- *lastUsedAt* - epoch timestamp in seconds, when the template was last time used
- *templateID* - unique ID of the template
- *templateBody* - text of the template
- *order* - the order in which the template should be displayed

### Get a message template by ID

**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 1,
  "uuid": 123,
  "body": {
    "category": "template",
    "templateID": 3
  }
}
```

**Response Payload Structure**

```json
{
  "body": {
    "lastUsedAt": 2,
    "order": 1,
    "templateBody": "I'll be there in 15 minutes",
    "templateID": 3
  },
  "endpoint": 8,
  "status": 200,
  "uuid": 123
}
```

### Change a message template body

**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 3,
  "uuid": "123",
  "body": {
    "category": "template",
    "templateID": 2,
    "templateBody": "Changed template."
  }
}
```

**Response Payload Structure**

```json
{
  "endpoint": 8,
  "status": 204,
  "uuid": 123
}
```

### Change a message template order

**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 3,
  "uuid": "123",
  "body": {
    "category": "template",
    "templateID": 2,
    "order": 3
  }
}
```

**Response Payload Structure**

```json
{
  "endpoint": 8,
  "status": 204,
  "uuid": 123
}
```

### Add a message template

**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 2,
  "uuid": 123,
  "body": {
    "category": "template",
    "templateBody": "This is a new message template."
  }
}
```

**Response Payload Structure**

```json
{
  "body": {
    "templateID": 2
  },
  "endpoint": 8,
  "status": 200,
  "uuid": 123
}
```

### Delete a message template by id

**Request Payload Structure**

```json
{
  "endpoint": 8,
  "method": 4,
  "uuid": 123,
  "body": {
    "category": "template",
    "templateID": 2
  }
}
```

**Response Payload Structure**

```json
{
  "endpoint": 8,
  "status": 204,
  "uuid": 123
}
```