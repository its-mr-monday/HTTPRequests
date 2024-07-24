# HTTPRequests C++
![C++ Logo](https://upload.wikimedia.org/wikipedia/commons/thumb/1/18/ISO_C%2B%2B_Logo.svg/50px-ISO_C%2B%2B_Logo.svg.png)

Easy to use Cross-Platform library that can handle a variety of HTTP client side communication within Windows and POSIX C++ applications

**Requires the OpenSSL library to be installed on the system**

# Quick Links

 - [Function Declarations](FUNCTIONS.md)
 - [Structure Declarations](STRUCTURES.md)

# How do I import this?

There are two methods of using the library, in it's header only form or by using the HPP and CPP files within your project,

For Header Only (Easiest Solution) simply include the header file within your build process

For the other solution simply just copy the requests.hpp and requests.cpp files into your C++ project

In order to use the library, simply #include "requests.hpp" in any file you are making Web Requests from

Please ensure to link OpenSSL

You must also link ws2_32, mswsock, shlwapi, advapi32, dnsapi, for Windows systems

# Basic Get Request Example
```cpp
#include "requests.hpp"

//Will get the route https://www.google.com
void get_google() {
  HTTPGetRequest request = CreateGetRequest("https://www.google.com");
  HTTPResponse response = HTTPGet(request);
  //To see the status code
  std::cout << response.status_code << std::endl;
  //To see the body
  std::cout << response.body << std::endl;
}
```
# Basic JSON Post Request Example
```cpp
#include "requests.hpp"

//Will POST to the route http://example.com/api/v1/example
//The following data: { 'message': 'Hello World!' }
void post_example() {
  std::string encodedJson = "{ 'message': 'Hello World!' }";
  HTTPPostRequest request = CreateJsonPostRequest("http://example.com/api/v1/example", encodedJson);
  HTTPResponse response = HTTPPost(request);
  //To see the status code
  std::cout << response.status_code << std::endl;
  //To see the body
  std::cout << response.body << std::endl;
}
```
# Get Request with JWT token
```cpp
#include "requests.hpp"

//Will GET the route https://google.com with a JWT auth token "TEST_TOKEN"
void get_google_jwt() {
  HTTPGetRequest request = CreateGetRequest("https://www.google.com");
  addHeader(request, "Authorization", "TEST_TOKEN");
  HTTPResponse response = HTTPGet(request);
  //To see the status code
  std::cout << response.status_code << std::endl;
  //To see the body
  std::cout << response.body << std::endl;
}
```
# Post Request with JWT token
```cpp
#include "requests.hpp"

//Will POST to the route http://example.com/api/v1/example with a JWT auth token "TEST_TOKEN"
//The following data: { 'message': 'Hello World!' }
void post_example_jwt() {
  std::string encodedJson = "{ 'message': 'Hello World!' }";
  HTTPPostRequest request = CreateJsonPostRequest("http://example.com/api/v1/example", encodedJson);
  addHeader(request, "Authorization", "TEST_TOKEN");
  HTTPResponse response = HTTPPost(request);
  //To see the status code
  std::cout << response.status_code << std::endl;
  //To see the body
  std::cout << response.body << std::endl;
}
```
# Downloading a file via GET request
```cpp
#include "requests.hpp"

//Will download a file hosted at https://example.com/download/test.txt
void download_example() {
  std::string outfile = "/home/test.txt";
  HTTPGetRequest request = CreateGetRequest("https://example.com/download/test.txt");
  HTTPResponse response = HTTPGet(request);
  downloadFile(response, outfile);  //Will attempt to download the file within the body to the outfile location
}
```

# Uploading a file via POST request **ONLY MIME FORMAT**
```cpp
#include "requests.hpp"

//Will upload a file in MIME format to the route https://example.com/upload
//Must have the inner data of the file saved to a std::string
void upload_example(std::string fileData, std::string fileName) {
  HTTPPostRequest request = CreateMimePostRequest("https://example.com/upload", fileName, fileData);
  HTTPResponse response = HTTPPost(request);
  //To see the status code
  std::cout << response.status_code << std::endl;
  //To see the body
  std::cout << response.body << std::endl;
}
```
