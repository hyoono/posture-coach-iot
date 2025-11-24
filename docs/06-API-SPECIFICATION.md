# API Specification Documentation

## Complete REST API Documentation

### Endpoints Summary
| Endpoint               | Method  |
|------------------------|---------|
| `/api/v1/resource`    | GET     |
| `/api/v1/resource`    | POST    |
| `/api/v1/resource/{id}`| PUT     |
| `/api/v1/resource/{id}`| DELETE  |

### Request/Response JSON Schemas
#### GET /api/v1/resource
**Request Schema:**  
None

**Response Schema:**  
```json
[
  {
    "id": "string",
    "name": "string",
    "value": "number"
  }
]
```

#### POST /api/v1/resource
**Request Schema:**  
```json
{
  "name": "string",
  "value": "number"
}
```
**Response Schema:**  
```json
{
  "id": "string",
  "name": "string",
  "value": "number"
}
```

#### PUT /api/v1/resource/{id}
**Request Schema:**  
```json
{
  "name": "string",
  "value": "number"
}
```
**Response Schema:**  
```json
{
  "id": "string",
  "name": "string",
  "value": "number"
}
```

#### DELETE /api/v1/resource/{id}
**Request Schema:**  
None  
**Response Schema:**  
```json
{
  "message": "Resource deleted"
}
```

### HTTP Status Codes
- **200 OK**: Request succeeded
- **201 Created**: Resource created
- **204 No Content**: Resource deleted
- **404 Not Found**: Resource not found
- **400 Bad Request**: Invalid request data
- **500 Internal Server Error**: Unexpected error

### Authentication
- No authentication is required for this project.

### Rate Limiting Considerations
- No rate limiting implemented for this project.

### Error Response Formats
- Standard error response:  
```json
{
  "error": "string",
  "message": "string"
}
```

### Example Requests with curl
- GET Request:  
```bash
curl -X GET https://api.example.com/api/v1/resource
```
- POST Request:  
```bash
curl -X POST https://api.example.com/api/v1/resource -H "Content-Type: application/json" -d '{"name":"example", "value": 10}'
```

### Example Responses
- Successful GET Response:  
```json
[
  {
    "id": "1",
    "name": "example",
    "value": 10
  }
]
```
- Successful POST Response:  
```json
{
  "id": "1",
  "name": "example",
  "value": 10
}
```

### Data Types and Validation Rules
- `string`: Must be a valid string
- `number`: Must be a valid number

### Endpoint Versioning Strategy
- Versioning is handled via the URL (e.g., `/api/v1/`).

### CORS Headers
- CORS is enabled:
  - Access-Control-Allow-Origin: *
  - Access-Control-Allow-Methods: GET, POST, PUT, DELETE

### Testing with Postman/curl
- Can be tested using Postman by importing the above requests, or using curl commands as shown above.

