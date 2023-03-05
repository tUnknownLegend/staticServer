#include <cstdio>
#include "server.h"

int
main(int c, char **v) {
    setvbuf(stdout, nullptr, _IONBF, 0);

    run();
    return 0;
}