# Methods
Methods indicate the desired action to be performed for a given resource.
See: [Common parameters explanation](common_parameters_explanation.md)

## Get
- Used to retrieve resources, and not to modify it in any way.
- Do not change the state of a resource.

Response codes:
- If a resource is found, the response code is *200*.
- If a resource is not found, the response code is *404*.
- If an internal server error occurred, the response code is *500*.

## Post
- Used to create a new resource into the collection of resources.

Response codes:

- If a resource has been created, and response contains body field, the response code is *200*.
- If a resource has been created, but response contains no body field, the response code is *204*.
- If a request is incorrect, the response code is *400*
- If an internal server error occurred, the response code is *500*.

## Put
- Used to update an existing resource.
- If the resource doesn’t exist, the Put method doesn’t create it.

Response codes:

- If a resource has been modified, and response contains body field, the response code is *200*.
- If a resource has been modified, but response contains no body field, the response code is *204*.
- If a request is incorrect, the response code is *400*.
- If a resource doesn’t exist, the response code is *404*.
- If an internal server error occurred, the response code is *500*.

## Del
- Use to delete a resource.

Response codes:

- If a resource has been deleted, and response contains body field, the response code is 200.
- If a resource has been deleted, but response contains no body field, the response code is 204.
- If a request is incorrect, the response code is 400.
- If a resource doesn’t exist, the response code is 404.
- If an internal server error occurred, the response code is 500.