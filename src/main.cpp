#include <cstdio>
#include "server.h"

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);

    runServer();
    return 0;
}