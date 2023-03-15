#include <string>
#include <filesystem>
#include "utils.h"
#include "net.h"

std::string getCodeMeaning(const short code) {
    return codes.at(code);
}

Response::Response(const plainResponse &response) {
    responseCode = response.responseCode;
    body = response.data;
    headers << "Server: test-libevent\r\n" << "Date: " << getCurrentTime() << "Connection: Closed";

    if (response.path.string().length()) {
        headers << "\r\nContent-Type: " <<
                extensions.at(response.path.extension().string().length() ?
                              response.path.extension().string() : "text/plain");
    }
    headers <<
            "\r\nContent-Length: " << response.dataLength;
}

std::string Response::get() const {
    return protocol + " " +
           std::to_string(responseCode) + " " +
           getCodeMeaning(responseCode) + "\r\n" +
           headers.str() + "\r\n\r\n" +
           body;
}
