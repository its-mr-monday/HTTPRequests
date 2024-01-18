/*
    Copyright 2024 by Zackary Morvan, Cyber M Technologies.
    All rights reserved
    This file is part of the HTTPRequests C++ Library, and is released 
    under the "MIT License Agreement". Please see the LICENSE file that 
    should have been included as part of this package
*/

#include "requests.hpp"
#include <iostream>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <openssl/ssl.h>
#include <openssl/err.h>
#ifdef __unix__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <shlwapi.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#endif

typedef std::string string;

static int always_true_callback(X509_STORE_CTX *ctx, void *arg)
{
    return 1;
}

std::vector<std::string> split(std::string str, char delimiter) {
  std::vector<std::string> internal;
  std::stringstream ss(str); // Turn the string into a stream.
  std::string tok;
  
  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  
  return internal;
}

bool is_number(std::string s)
{
    int dotCount = 0;
    if (s.empty())
       return false;
    for (char c : s )
    {
       if ( !(std::isdigit(c) || c == '.' ) && dotCount > 1 )
       {
          return false;
       }
       dotCount += (c == '.');
    }
    return true;
}

bool is_ip_address(string ip) {
#ifdef __unix__
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
    return result != 0;
#else
    std::vector<string> ip_split = split(ip, '.');
    if (ip_split.size() != 4) {
        return false;
    }
    for (string s : ip_split) {
        if (!is_number(s)) {
            return false;
        }
    }
    return true;
#endif
}

string resolvdnsname(string dnsname) {
#ifdef __unix__
    struct hostent *host;
    host = gethostbyname(dnsname.c_str());
    if (host == NULL) {
        return "";
    }
    char *ip = inet_ntoa(*(struct in_addr *)host->h_addr);
    return string(ip);
#else
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    struct hostent *he = gethostbyname(dnsname.c_str());
    if (he == NULL) {
        return "";
    }
    char *ip = inet_ntoa(*(struct in_addr *)he->h_addr_list[0]);
    return string(ip);
#endif
}

void downloadFile(HTTPResponse response, string outfile) {
#ifdef __unix__
    if (std::filesystem::exists(outfile)) {
        return;
    }
#else
    if (PathFileExistsA(outfile.c_str()) == TRUE) {
        return;
    }
#endif
    std::ofstream outfile_stream(outfile, std::ios::out | std::ios::binary);
    outfile_stream.write(response.body.c_str(), response.body.length());
    outfile_stream.close();
    return;
}

SSL_CTX *initSSL(bool verify) {
    SSL_library_init();
    SSL_CTX *ctx;
#ifdef __unix__
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(TLS_client_method());
    if (!verify) {
        SSL_CTX_set_verify_callback(ctx, always_true_callback, NULL);
    }
#else
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    ctx = SSL_CTX_new(TLS_client_method());
    if (!verify) {
        SSL_CTX_set_cert_verify_callback(ctx, always_true_callback, NULL);
    }
#endif
    return ctx;
}

#ifdef __unix__
void CloseSocket(int socket) {
    close(socket);
}
#else
void CloseSocket(SOCKET socket) {
    closesocket(socket);
}
#endif

string send_ssl_payload(string host, int port, string packet, bool verify) {
    if (!is_ip_address(host)) {
        host = resolvdnsname(host);
    }
#ifdef __unix__
    string result;
    int sockfd;
    int sslsockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return "";
    }
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = inet_addr(host.c_str());
    socklen_tr socklen = sizeof(sa);
    if (connect(sockfd, (struct sockaddr *)&sa, socklen) < 0) {
        return "";
    }
    SSL_CTX *ctx = initSSL(verify);
    SSL *ssl = SSL_new(ctx);
    if (!ssl) {
        return "";
    }
    sslsockfd = SSL_get_fd(ssl);
    SSL_set_fd(ssl, sockfd);
    if (SSL_connect(ssl) < 0) {
        return "";
    }
    int bytesSent = SSL_write(ssl, packet.c_str(), packet.size());
    if (bytesSent <= 0) {
        return "";
    }

    char buffer[4096];
    int size = 0;
    while (true) {
        int read = SSL_read(ssl, buffer, 4096);
        size += read;
        result += string(buffer, read);
        if (read < 4096) {
            break;
        }
    }

    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sockfd);
    return result;
#else
    
    SSL_CTX *ctx = initSSL(verify);
    SSL *ssl;
    BIO* bio = BIO_new_ssl_connect(ctx);
    if (bio == NULL) {
        return "";
    }
    BIO_get_ssl(bio, &ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    BIO_set_conn_hostname(bio, (host + ":" + std::to_string(port)).c_str());
    if (BIO_do_connect(bio) <= 0) {
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return "";
    }
    if (BIO_do_handshake(bio) <= 0) {
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return "";
    }

    //Send over socket
    int bytesSent = SSL_write(ssl, packet.c_str(), packet.size());
    if (bytesSent <= 0) {
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return "";
    }
    if (bytesSent < packet.size()) {
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return "";
    }
    char buf[4096];
    string response = "";
    int bytesReceived = 0;
    while (true) {
        bytesReceived = SSL_read(ssl, buf, 4096);
        if (bytesReceived < 1) {
            break;
        }
        response += string(buf, bytesReceived);
        if (SSL_pending(ssl) < 1) {
            break;
        }
    }

    if (bytesReceived < 0) {
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return "";
    }
    BIO_free_all(bio);
    SSL_CTX_free(ctx);
    return response;
#endif
}

string send_payload(string host, int port, string packet) {
    if (!is_ip_address(host)) {
        host = resolvdnsname(host);
    }
#ifdef __unix__
    string result;
    int sockfd;
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = inet_addr(host.c_str());
    socklen_t socklen = sizeof(sa);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return "";
    }
    if (connect(sockfd, (struct sockaddr *)&sa , socklen) < 0) {
        return "";
    }
    int sent = send(sockfd, packet.c_str(), packet.length(), 0);
    if (sent < 0) {
        return "";
    }
    char buffer[4096];
    while (true) {
        int read = recv(sockfd, buffer, 4096, 0);
        result += string(buffer, read);
        if (read < 4096) {
            break;
        }
    }
    close(sockfd);
    return result;
#else
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    int iResult;
    int recvbuflen = 4096;
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        return "";
    }
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &result);
    if ( iResult != 0 ) {
        WSACleanup();
        return "";
    }
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
        sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sock == INVALID_SOCKET) {
            WSACleanup();
            return "";
        }
        iResult = connect( sock, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(sock);
            sock = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return "";
    }
    iResult = send(sock, packet.c_str(), packet.size(), 0);
    if (iResult == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return "";
    }
    char recvbuf[4096];
    string response = "";
    int received = 0;
    while ((received = recv(sock, recvbuf, recvbuflen, 0)) > 0) {
        response += string(recvbuf, received);
    }
    closesocket(sock);
    WSACleanup();
    return response;
#endif
}

//Will encode a HTTPGetRequest struct to a payload string
string encode_payload(HTTPGetRequest request) {
    string result;
    result += "GET " + request.path + " HTTP/1.1\r\n";
    result += "Host: " + request.host + "\r\n";
    //Go through each header
    for (auto header : request.headers) {
        result += header.first + ": " + header.second + "\r\n";
    }
    result += "\r\n";
    return result;
}

//Will decode a HTTP response string to a HTTPResponse struct
HTTPResponse decodePacket(string packet) {
    HTTPResponse response;
    response.headers = std::map<string, string>();
    //Split packet into lines
    std::vector<string> lines = split(packet, '\n');
    //Get first line
    string firstline = lines[0];
    //Split first line into parts
    std::vector<string> firstlineparts = split(firstline, ' ');
    //Get status code
    response.status_code = stoi(firstlineparts[1]);

    
    int currentIndex = 1;
    for (int i = 1; i < lines.size(); i++) {
        currentIndex +=1;
        if (lines[i] == "\r") {
            break;
        }
        std::vector<string> headerparts = split(lines[i], ':');
        response.headers[headerparts[0]] = headerparts[1];
    }
    //Get body
    string body = "";
    for (int i = currentIndex; i < lines.size(); i++) {
        body += lines[i];
    }
    response.body = body;
    return response;

}

//Will encode a HTTPPostRequest struct to a payload string
string encode_payload(HTTPPostRequest request) {
    string result;
    std::stringstream ss;
    ss << ("POST " + request.path + " HTTP/1.1\r\n");
    ss << ("Host: " + request.host + "\r\n");
    //Go through each header
    for (auto header : request.headers) {
        ss << (header.first + ": " + header.second + "\r\n");
    }
    ss << ("Content-Length: " + std::to_string(request.body.length()) + "\r\n");
    ss << "\r\n";
    ss << request.body;
    return ss.str();
}

//Will add/set a "key" header with "value" to a HTTPGetRequest struct
void addHeader(HTTPGetRequest &request, string key, string value) {
    request.headers[key] = value;
}
//Will add/set a "key" header with "value" to a HTTPPostRequest struct
void addHeader(HTTPPostRequest &request, string key, string value) {
    request.headers[key] = value;
}

//Will create a HTTPGetRequest struct
HTTPGetRequest CreateGetRequest(string url, bool acceptJson) {
    HTTPGetRequest request;
    request.headers = std::map<string, string>();
    request.sslVerify = true;
    request.url = url;
    bool isSsl = false;
    string protocol = "http";
    int port;
    string urltoparse = url;
    
    if (urltoparse.find("https://") == 0) {
        isSsl = true;
        protocol = "https";
        urltoparse = urltoparse.substr(8);
        port = 443;
    } else {
        urltoparse = urltoparse.substr(7);
        port = 80;
    }
    request.isSsl = isSsl;
    string hostname;
    string subroute;
    string ipaddr;

    if (urltoparse.find(":") != -1) {
        if (urltoparse.find("/") == -1) {
            urltoparse = urltoparse + "/";
        }
        int slashIndex = urltoparse.find("/");
        subroute = urltoparse.substr(slashIndex);
        int colonIndex = urltoparse.find(":");
        port = stoi(urltoparse.substr(colonIndex + 1, slashIndex - colonIndex - 1));
        hostname = urltoparse.substr(0, colonIndex);
    } else {
        if (urltoparse.find("/") == -1) {
            urltoparse = urltoparse + "/";
        }
        int slashIndex = urltoparse.find("/");
        subroute = urltoparse.substr(slashIndex);
        hostname = urltoparse.substr(0, slashIndex);
    }
    request.port = port;
    request.host = hostname;
    request.path = subroute;

    if (!is_ip_address(hostname)) {
        request.ipaddr = resolvdnsname(hostname);
    } else {
        request.ipaddr = hostname;
    }

    addHeader(request, "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.102 Safari/537.36");
    if (acceptJson == true) {
        addHeader(request, "Accept", "application/json");
    }
    request.method = "GET";
    return request;
}

//Will create a HTTPPostRequest struct
HTTPPostRequest CreateJsonPostRequest(string url, string jsonpayload) {
    HTTPPostRequest request;
    request.headers = std::map<string, string>();
    bool isSsl = false;
    request.sslVerify = true;
    request.url = url;
    string protocol = "http";
    int port;
    string urltoparse = url;
    
    if (urltoparse.find("https://") == 0) {
        isSsl = true;
        protocol = "https";
        urltoparse = urltoparse.substr(8);
        port = 443;
    } else {
        urltoparse = urltoparse.substr(7);
        port = 80;
    }
    request.isSsl = isSsl;
    string hostname;
    string subroute;
    string ipaddr;

    if (urltoparse.find(":") != -1) {
        if (urltoparse.find("/") == -1) {
            urltoparse = urltoparse + "/";
        }
        int slashIndex = urltoparse.find("/");
        subroute = urltoparse.substr(slashIndex);
        int colonIndex = urltoparse.find(":");
        port = stoi(urltoparse.substr(colonIndex + 1, slashIndex - colonIndex - 1));
        hostname = urltoparse.substr(0, colonIndex);
    } else {
        if (urltoparse.find("/") == -1) {
            urltoparse = urltoparse + "/";
        }
        int slashIndex = urltoparse.find("/");
        subroute = urltoparse.substr(slashIndex);
        hostname = urltoparse.substr(0, slashIndex);
    }
    request.port = port;
    request.host = hostname;
    request.path = subroute;

    if (!is_ip_address(hostname)) {
        request.ipaddr = resolvdnsname(hostname);
    } else {
        request.ipaddr = hostname;
    }

    addHeader(request, "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.102 Safari/537.36");
    addHeader(request, "Content-Type", "application/json");
    addHeader(request, "Accept", "application/json");
    
    request.body = jsonpayload;
    request.method = "POST";
    return request;
}

// Generate a random string of numbers and letters "len" long
string rand_string(int len) {
    string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    srand(time(0));
    string result = "";
    for(int i=0;i<len;i++) {
        result += str[rand()%str.size()];
    }
    return result;
}
//Generate a random HTTP Multipart boundary
string generateBoundary() {
    return rand_string(8) + "-"+rand_string(4)+"-"+rand_string(4)+"-"+rand_string(4)+"-"+rand_string(8);
}

//Will create a HTTPPostRequest struct with a multipart/form-data body file
HTTPPostRequest CreateMimePostRequest(string url, string filename, string filedata) {
    //Create a random string that looks like: db54202a-dd6f-48e5-a433-0bf5805d201b
    HTTPPostRequest request;
    request.headers = std::map<string, string>();
    request.sslVerify = true;
    request.url = url;
    bool isSsl = false;
    string protocol = "http";
    int port;
    string urltoparse = url;
    
    if (urltoparse.find("https://") == 0) {
        isSsl = true;
        protocol = "https";
        urltoparse = urltoparse.substr(8);
        port = 443;
    } else {
        urltoparse = urltoparse.substr(7);
        port = 80;
    }
    request.isSsl = isSsl;
    string hostname;
    string subroute;
    string ipaddr;

    if (urltoparse.find(":") != -1) {
        if (urltoparse.find("/") == -1) {
            urltoparse = urltoparse + "/";
        }
        int slashIndex = urltoparse.find("/");
        subroute = urltoparse.substr(slashIndex);
        int colonIndex = urltoparse.find(":");
        port = stoi(urltoparse.substr(colonIndex + 1, slashIndex - colonIndex - 1));
        hostname = urltoparse.substr(0, colonIndex);
    } else {
        if (urltoparse.find("/") == -1) {
            urltoparse = urltoparse + "/";
        }
        int slashIndex = urltoparse.find("/");
        subroute = urltoparse.substr(slashIndex);
        hostname = urltoparse.substr(0, slashIndex);
    }
    request.port = port;
    request.host = hostname;
    request.path = subroute;

    if (!is_ip_address(hostname)) {
        request.ipaddr = resolvdnsname(hostname);
    } else {
        request.ipaddr = hostname;
    }

    string boundary = generateBoundary();
    addHeader(request, "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.102 Safari/537.36");
    addHeader(request, "Content-Type", "multipart/form-data; boundary=\"" + boundary + "\"");
    
    string body = "--" + boundary + "\r\n";
    body += "Content-Disposition: form-data; name=\"file\"; filename=\"" + filename + "\"\r\n\r\n";
    body += filedata;
    body += "\r\n--" + boundary + "--\r\n";

    request.body = body;
    request.method = "POST";
    return request;
}

//Function for getting headers
string getHeader(HTTPGetRequest &request, string key) {
    return request.headers[key];
}
string getHeader(HTTPPostRequest &request, string key) {
    return request.headers[key];
}
string getHeader(HTTPResponse &response, string key) {
    return response.headers[key];
}

//Will dispatch a HTTPGetRequest to the server and return a HTTPResponse
HTTPResponse HTTPGet(HTTPGetRequest request) {
    string payload = encode_payload(request);
    string raw_response;

    if (request.isSsl) {
        raw_response = send_ssl_payload(request.ipaddr, request.port, payload, request.sslVerify);
    } else {
        raw_response = send_payload(request.ipaddr, request.port, payload);
    }
    return decodePacket(raw_response);
}

//Will dispatch a HTTPPostRequest to its server and return a HTTPResponse
HTTPResponse HTTPPost(HTTPPostRequest request) {
    string payload = encode_payload(request);
    string raw_response;

    if (request.isSsl) {
        raw_response = send_ssl_payload(request.ipaddr, request.port, payload, request.sslVerify);
    } else {
        raw_response = send_payload(request.ipaddr, request.port, payload);
    }
    return decodePacket(raw_response);
}

void test_get_google() {
    HTTPGetRequest request = CreateGetRequest("https://www.google.com");
    HTTPResponse response = HTTPGet(request);
    std::cout << response.status_code << std::endl;
    std::cout << response.body << std::endl;
    if (response.status_code == 200) {
        std::cout << "Success" << std::endl;
    } else {
        std::cout << "Failure" << std::endl;
    }
}