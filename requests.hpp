/*
    Copyright 2024 by Zackary Morvan, Cyber M Technologies.
    All rights reserved
    This file is part of the HTTPRequests C++ Library, and is released 
    under the "MIT License Agreement". Please see the LICENSE file that 
    should have been included as part of this package
*/

#include <string>
#include <vector>
#include <map>
#include <string.h>
#include <stdio.h>
#ifdef __unix__
#include <unistd.h>
#endif

std::vector<std::string> split(std::string str, char delimiter);

//Struct defining a HTTPGetRequest
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

//Struct defining a HTTPPostRequest
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

//Struct defining a HTTPResponse
struct HTTPResponse {
    std::string body;
    std::map<std::string, std::string> headers;
    int status_code;
};

//downloads a file to outfile from the HTTPResponse object
//if outfile exists no file will be written
void downloadFile(HTTPResponse response, std::string outfile);

//Will add/set a "key" header with "value" to a HTTPGetRequest struct
void addHeader(HTTPGetRequest &request, std::string key, std::string value);

//Will add/set a "key" header with "value" to a HTTPPostRequest struct
void addHeader(HTTPPostRequest &request, std::string key, std::string value);
//Function for getting headers
std::string getHeader(HTTPGetRequest &request, std::string key);
//Function for getting headers
std::string getHeader(HTTPPostRequest &request, std::string key);
//Function for getting headers
std::string getHeader(HTTPResponse &response, std::string key);

//Will dispatch a HTTPGetRequest to the server and return a HTTPResponse
HTTPResponse HTTPGet(HTTPGetRequest request);
//Will dispatch a HTTPPostRequest to its server and return a HTTPResponse
HTTPResponse HTTPPost(HTTPPostRequest request);

//Will create a HTTPGetRequest struct
HTTPGetRequest CreateGetRequest(std::string url, bool acceptJson = false);

//Will create a HTTPPostRequest struct
HTTPPostRequest CreateJsonPostRequest(std::string url, std::string jsonpayload);

//Will create a HTTPPostRequest struct with a multipart/form-data body file
HTTPPostRequest CreateMimePostRequest(std::string url, std::string filename, std::string filedata);

//Will encode a HTTPGetRequest struct to a payload string
std::string encode_payload(HTTPGetRequest request);

//Will encode a HTTPPostRequest struct to a payload string
std::string encode_payload(HTTPPostRequest request);

//Will decode a HTTP response string to a HTTPResponse struct
HTTPResponse decodePacket(std::string packet);

//Will send a raw http packet and return a raw response
//Host must be resolved AF_INET
std::string send_payload(std::string host, int port, std::string packet);

//Will send a raw https packet and return a raw response
//Host must be resolved AF_INET
std::string send_ssl_payload(std::string host, int port, std::string packet, bool verify = true);

//resolves dnsnames to ip addresses
std::string resolvdnsname(std::string dnsname);

//Validates string "ip" is a valid ip address
bool is_ip_address(std::string ip);

void test_get_google();
