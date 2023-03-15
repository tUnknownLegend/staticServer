#ifndef STATICSERVER_UTILS_H
#define STATICSERVER_UTILS_H

#include <string>

std::string getCurrentTime();

std::string urlDecode(std::string SRC);

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

#endif //STATICSERVER_UTILS_H
