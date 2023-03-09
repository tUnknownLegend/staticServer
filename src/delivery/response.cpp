//#include "contentType.h"
//#include "responseCodes.h"
//#include <string>
//#include <sstream>
//
//const std::string protocol = "HTTP/1.1";
//
//class Response {
//private:
//    short responseCode;
//    std::stringstream headers;
//    std::string body;
//
//public:
//    explicit Response(const short responseCode) : responseCode(responseCode) {
//        headers << "Connection: close";
//    }
//
//    Response(const short responseCode, const std::string &body) : responseCode(responseCode) {
//        headers << "Connection: close";
//    }
//
//    std::string get() const {
//        return protocol + " " +
//               std::to_string(responseCode) + " " +
//               getCodeMeaning(responseCode) + "\n" +
//               headers.str() + "\n" +
//               body;
//    }
//};