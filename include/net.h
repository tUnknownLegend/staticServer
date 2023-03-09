#ifndef STATICSERVER_NET_H
#define STATICSERVER_NET_H

#include <string>
#include <sstream>
#include <map>

const std::map<std::string, std::string> extensions;

std::string getContentType(const std::string &fileName);

const std::map<short, std::string> codes;

std::string getCodeMeaning(const short code);

class Response {
private:
    short responseCode;
    std::stringstream headers;
    std::string body;

public:
    explicit Response(short responseCode);

    Response(short responseCode, const std::string &body);

    std::string get() const;

    ~Response() = default;
};

class Request;

#endif //STATICSERVER_NET_H
