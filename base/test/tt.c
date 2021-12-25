#include <stdio.h>
#include "base/hash.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Please input str: \n");
        return 1;
    }

    unsigned int key = RSHash(argv[1]);
    printf("str: %s, key: %u\n", argv[1], key);
    return 0;
}