#include <stdio.h>

int main() {
    FILE *file = fopen("fdnba.bin", "wb");
    if (!file) {
        perror("Could not open file");
        return 1;
    }

    int nbaVig[][2] = {
        {-113, -113}, {-114, -114}, {-112, -118}, {-110, -120}, {-106, -125}, {-108, -122}, {-104, -128}, {-102, -130},
        {100, -132}, {102, -136}, {104, -138}, {106, -140}, {108, -144}, {110, -146}, {112, -148}, {114, -152}, {116, -154},
        {118, -158}, {120, -160}, {122, -162}, {124, -166}, {126, -168}, {128, -172}, {130, -174}, {132, -178}, {134, -180}
    };

    // can be +110/-145 on some NFL lines unfortunately
    // solution: make this one for NBA?

    for (size_t i = 0; i < sizeof(nbaVig); i++) {
        fwrite(nbaVig[i], sizeof(int), 2, file);
    }

    fclose(file);
    return 0;
}