#ifndef STATICSERVER_UTILS_H
#define STATICSERVER_UTILS_H

#include <string>
#include <filesystem>

std::string getCurrentTime();

std::string urlDecode(std::string SRC);

struct configData {
    unsigned int port;
    unsigned int processLimit;
    std::string rootFolder;
    std::filesystem::path rootPath;
};

configData inputMatrix();

auto const conf = inputMatrix();

const size_t MAX_LINE = 16384;
const size_t CHUNK_SIZE = 64;

#endif //STATICSERVER_UTILS_H
