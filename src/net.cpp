//#include <string>
//#include <sstream>
//#include <map>
//
//const std::string protocol = "HTTP/1.1";
//
//const std::map<std::string, std::string> extensions = {
//        {".html", "text/html"},
//        {".css",  "text/css"},
//        {".js",   "application/javascript"},
//        {".jpg",  "image/jpeg"},
//        {".jpeg", "image/jpeg"},
//        {".png",  "image/png"},
//        {".gif",  "image/gif"},
//        {".swf",  "application/x-shockwave-flash"}
//};
//
//const std::map<short, std::string> codes = {
//        {200, "OK"},
//        {400, "Forbidden"},
//        {404, "NotFound"},
//        {405, "MethodNotAllowed"}
//};
//
//std::string getContentType(const std::string &fileName) {
//    return extensions.at(fileName.substr(fileName.find(':')));
//}
//
//std::string getCodeMeaning(const short code) {
//    return codes.at(code);
//}
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
//
//class Request {
//    Request(){
//
//    }
//    ~Request() = default;
//};