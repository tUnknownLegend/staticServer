#pragma once

#ifndef STATICSERVER_NET_H
#define STATICSERVER_NET_H

#include <string>
#include <sstream>
#include <map>
#include <filesystem>

const std::string protocol = "HTTP/1.1";

const std::map<std::string, std::string> extensions = {
        {".html", "text/html"},
        {".css",  "text/css"},
        {".js",   "application/javascript"},
        {".jpg",  "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".png",  "image/png"},
        {".gif",  "image/gif"},
        {".swf",  "application/x-shockwave-flash"},
        {".txt",  "text/plain"}
};

const std::map<short, std::string> codes = {
        {200, "OK"},
        {400, "Bad Request"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {405, "Method Not Allowed"}
};

struct plainResponse {
    short responseCode;
    std::filesystem::path path;
    std::string data;
    size_t dataLength;
};

class Response {
private:
    short responseCode;
    std::stringstream headers;
    std::string body;

public:
    explicit Response(const plainResponse &response);

    std::string get() const;
};

#endif //STATICSERVER_NET_H
