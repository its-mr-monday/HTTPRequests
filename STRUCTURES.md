# HTTPRequests Structure Definitions

## HTTPGetRequest

| Field | Type | Description |
|-------|------|-------------|
| url | `std::string` | The full URL of the GET request |
| headers | `std::map<std::string, std::string>` | Key-value pairs of HTTP headers |
| method | `std::string` | The HTTP method (GET) |
| host | `std::string` | The target host for the request |
| path | `std::string` | The path component of the URL |
| ipaddr | `std::string` | The IP address of the target server |
| port | `int` | The port number for the connection |
| protocol | `std::string` | The protocol used (e.g., HTTP/1.1) |
| isSsl | `bool` | Indicates whether SSL/TLS is used |
| sslVerify | `bool` | Indicates whether to verify SSL certificates |

```cpp
struct HTTPGetRequest {
    std::string url;
    std::map<std::string, std::string> headers;
    std::string method;
    std::string host;
    std::string path;
    std::string ipaddr;
    int port;
    std::string protocol;
    bool isSsl;
    bool sslVerify;
};
```

## HTTPPostRequest

| Field | Type | Description |
|-------|------|-------------|
| url | `std::string` | The full URL of the POST request |
| headers | `std::map<std::string, std::string>` | Key-value pairs of HTTP headers |
| body | `std::string` | The body content of the POST request |
| method | `std::string` | The HTTP method (POST) |
| host | `std::string` | The target host for the request |
| path | `std::string` | The path component of the URL |
| ipaddr | `std::string` | The IP address of the target server |
| port | `int` | The port number for the connection |
| protocol | `std::string` | The protocol used (e.g., HTTP/1.1) |
| isSsl | `bool` | Indicates whether SSL/TLS is used |
| sslVerify | `bool` | Indicates whether to verify SSL certificates |

```cpp
struct HTTPPostRequest {
    std::string url;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string method;
    std::string host;
    std::string path;
    std::string ipaddr;
    int port;
    std::string protocol;
    bool isSsl;
    bool sslVerify;
};
```

## HTTPResponse

| Field | Type | Description |
|-------|------|-------------|
| body | `std::string` | The response body content |
| headers | `std::map<std::string, std::string>` | Key-value pairs of response headers |
| status_code | `int` | The HTTP status code of the response |

```cpp
struct HTTPResponse {
    std::string body;
    std::map<std::string, std::string> headers;
    int status_code;
};
```