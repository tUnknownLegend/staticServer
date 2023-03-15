#ifndef STATICSERVER_UTILS_H
#define STATICSERVER_UTILS_H

#include <string>

std::string getCurrentTime();

std::string urlDecode(std::string SRC);

const std::string rootDir = "DOCUMENT_ROOT";
const auto rootPath = std::filesystem::current_path().parent_path().append(rootDir);

const size_t MAX_LINE = 16384;
const size_t CHUNK_SIZE = 64;

#endif //STATICSERVER_UTILS_H
