#include <iostream>
#include <string>
#include <ctime>

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
