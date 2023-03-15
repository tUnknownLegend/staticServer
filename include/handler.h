#ifndef STATICSERVER_HANDLER_H
#define STATICSERVER_HANDLER_H

#include <string>
#include "net.h"

class Handler {
private:
    std::string method;
    std::string requestPath;

    bool checkMethod();

public:
    explicit Handler(const std::string &request);

    ~Handler() = default;

    plainResponse handle();
};

#endif //STATICSERVER_HANDLER_H
