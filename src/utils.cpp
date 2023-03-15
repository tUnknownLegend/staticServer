#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <filesystem>
#include "utils.h"

using namespace std;

string getCurrentTime() {
    time_t now = time(nullptr);

    tm *gmtm = gmtime(&now);
    if (gmtm != nullptr) {
        return asctime(gmtm);
    } else {
        cerr << "Failed to get the UTC date and time" << endl;
        return {};
    }
}

string urlDecode(const string SRC) {
    string ret;
    char ch;
    int i, ii;
    for (i = 0; i < SRC.length(); i++) {
        if (SRC[i] == '%') {
            sscanf(SRC.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        } else {
            ret += SRC[i];
        }
    }
    return (ret);
}

configData inputMatrix() {
    std::ifstream inFile("../conf.txt");
    if (!inFile.is_open()) {
        std::cerr << "error // input.txt open\n";
        return {8080, 4, "DOCUMENT_ROOT", filesystem::current_path().parent_path().append("DOCUMENT_ROOT")};
    }

    unsigned int port = 8080;
    inFile >> port;

    unsigned int processLimit = 4;
    inFile >> processLimit;

    std::string rootFolder;
    inFile >> rootFolder;

    inFile.close();
    return {port, processLimit, rootFolder,
            filesystem::current_path().parent_path().append(rootFolder)};
}
