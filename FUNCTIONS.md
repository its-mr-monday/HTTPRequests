## Function Definitions

### downloadFile

```cpp
void downloadFile(HTTPResponse response, std::string outfile);
```

**Parameters:**
- `response` (`HTTPResponse`): The HTTP response object containing the file data.
- `outfile` (`std::string`): The path to the output file.

**Description:**
Downloads the file content from the `HTTPResponse` object to the specified `outfile`. If the `outfile` already exists, the file will not be written.

---

### addHeader (HTTPGetRequest)

```cpp
void addHeader(HTTPGetRequest &request, std::string key, std::string value);
```

**Parameters:**
- `request` (`HTTPGetRequest&`): The HTTP GET request object.
- `key` (`std::string`): The header key.
- `value` (`std::string`): The header value.

**Description:**
Adds or sets a header with the specified `key` and `value` to the `HTTPGetRequest` object.

---

### addHeader (HTTPPostRequest)

```cpp
void addHeader(HTTPPostRequest &request, std::string key, std::string value);
```

**Parameters:**
- `request` (`HTTPPostRequest&`): The HTTP POST request object.
- `key` (`std::string`): The header key.
- `value` (`std::string`): The header value.

**Description:**
Adds or sets a header with the specified `key` and `value` to the `HTTPPostRequest` object.

---

### getHeader (HTTPGetRequest)

```cpp
std::string getHeader(HTTPGetRequest &request, std::string key);
```

**Parameters:**
- `request` (`HTTPGetRequest&`): The HTTP GET request object.
- `key` (`std::string`): The header key.

**Returns:**
- `std::string`: The value of the specified header key.

**Description:**
Retrieves the value of the specified header `key` from the `HTTPGetRequest` object.

---

### getHeader (HTTPPostRequest)

```cpp
std::string getHeader(HTTPPostRequest &request, std::string key);
```

**Parameters:**
- `request` (`HTTPPostRequest&`): The HTTP POST request object.
- `key` (`std::string`): The header key.

**Returns:**
- `std::string`: The value of the specified header key.

**Description:**
Retrieves the value of the specified header `key` from the `HTTPPostRequest` object.

---

### getHeader (HTTPResponse)

```cpp
std::string getHeader(HTTPResponse &response, std::string key);
```

**Parameters:**
- `response` (`HTTPResponse&`): The HTTP response object.
- `key` (`std::string`): The header key.

**Returns:**
- `std::string`: The value of the specified header key.

**Description:**
Retrieves the value of the specified header `key` from the `HTTPResponse` object.

---

### HTTPGet

```cpp
HTTPResponse HTTPGet(HTTPGetRequest request);
```

**Parameters:**
- `request` (`HTTPGetRequest`): The HTTP GET request object.

**Returns:**
- `HTTPResponse`: The HTTP response object.

**Description:**
Dispatches the `HTTPGetRequest` to the server and returns the `HTTPResponse`.

---

### HTTPPost

```cpp
HTTPResponse HTTPPost(HTTPPostRequest request);
```

**Parameters:**
- `request` (`HTTPPostRequest`): The HTTP POST request object.

**Returns:**
- `HTTPResponse`: The HTTP response object.

**Description:**
Dispatches the `HTTPPostRequest` to its server and returns the `HTTPResponse`.

---

### CreateGetRequest

```cpp
HTTPGetRequest CreateGetRequest(std::string url, bool acceptJson = false);
```

**Parameters:**
- `url` (`std::string`): The URL for the GET request.
- `acceptJson` (bool, optional): Indicates whether to accept JSON responses (default is `false`).

**Returns:**
- `HTTPGetRequest`: The created HTTP GET request object.

**Description:**
Creates a `HTTPGetRequest` struct with the specified URL and optional JSON acceptance.

---

### CreateJsonPostRequest

```cpp
HTTPPostRequest CreateJsonPostRequest(std::string url, std::string jsonpayload);
```

**Parameters:**
- `url` (`std::string`): The URL for the POST request.
- `jsonpayload` (`std::string`): The JSON payload for the POST request.

**Returns:**
- `HTTPPostRequest`: The created HTTP POST request object.

**Description:**
Creates a `HTTPPostRequest` struct with the specified URL and JSON payl