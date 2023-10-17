# Error handling strategy
If any error occurs while processing the client’s requests, the server returns an error response in the following form:
```
{
  "endpoint": <endpoint id>,
  "uuid": <unique id>,
  "status": <status code>
}
```
Where:
- *endpoint id* is an identifier of a target endpoint
- *unique id* is a unique identifier for a pair of request-response.
- *status code* is a status code, based on HTTP error codes.

## Error status codes
For all status codes, see: [Common parameters explanation](common_parameters_explanation.md)

Statuses **4XX** and **5XX** are reserved for errors.
- **4xx** - client’s errors.
- **5xx** - internal server’s errors.

## Endpoint-local error codes
Some endpoints may introduce local error codes to indicate the endpoint-specific errors.

For example, the Update endpoint introduced Update errors that indicate the detailed problem that occurred during the update procedure.

Endpoint-local error codes are sent in the message body, and not in the message headers.

```
#000000250
{
"body": {
  "errorCode": 12
}, 
"endpoint": 2, 
"status": 500, 
"uuid": 123
}
```
Where *errorCode: 12* means *CantDeletePreviousOS*.

## Examples
- A message couldn’t be found on GET request
```
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
```
{
  "endpoint": 8,
  "uuid": 123,
  "status": 404,
  "message": "The message does not exist." 
}
```
- Incorrect request JSON (request is ill-formed)
```
{
   "endpoint": 8,
   "method": 1,
   "uuid": 123,
   "body": {
      "category": "msg",
      "messageID": 8
   }
}
```
```
{
  "endpoint": 8,
  "uuid": 123,
  "status": 400,
  "message": "The request is ill-formed." 
}
```
- Server logical error

```
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
```
{
  "endpoint": 8,
  "uuid": 123,
  "status": 500,
  "message": "Internal server error (#code)" 
}
```
**Note:** Additional internal server error code may be added to the message if needed.