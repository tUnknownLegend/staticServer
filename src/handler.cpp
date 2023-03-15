#include <filesystem>
#include <fstream>

#include "handler.h"
#include "utils.h"


bool Handler::checkMethod() {
    if (method == "GET" || method == "HEAD") {
        return true;
    } else {
        return false;
    }
}


Handler::Handler(const std::string &request) {
    method = request.substr(0, request.find(' '));
    requestPath = request.substr(method.length() + 1,
                                 request.rfind(' ') - (method.length() + 1));
}

plainResponse Handler::handle() {
    if (!checkMethod()) {
        return {405, {}, {}, {}};
    }

    if (requestPath.find("/../") != std::string::npos) {
        return {400, {}, {}, {}};
    }

    auto path = rootPath;
    path.append(rootPath.string() + requestPath);

    if (!path.has_filename()) {
        path.replace_filename("index.html");
    }

    if (!std::filesystem::exists(path)) {
        path.assign(urlDecode(path.string()));

        path.assign(path.string().substr(0, path.string().find('?')));
        path.assign(path.string().substr(0, path.string().find('#')));

        if (!std::filesystem::exists(path)) {
            if (requestPath.find('.') != std::string::npos) {
                return {404, {}, {}, {}};
            } else {
                return {403, {}, {}, {}};
            }
        }
    }

    if (std::filesystem::is_directory(path)) {
        return {403, {}, {}, {}};
    }

    std::ifstream input(path.string(), std::ios::binary);
    std::string str = {std::istreambuf_iterator<char>(input), {}};

    if (method == "HEAD") {
        return {200, {}, {}, str.length()};
    }

    return {200, path, str, str.length()};
}