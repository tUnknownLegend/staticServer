#include <cstdio>
#include "server.h"

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);

    run();
    return 0;
}