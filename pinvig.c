#include <stdio.h>

int main() {
    FILE *pinnba = fopen("pinnba.bin", "wb");
    // alternate spread
    // player props
    int nbaVig[][2] = {
        {-108, -123}, {128, -171}, {133, -179}
    };
    for (size_t i = 0; i < sizeof(nbaVig) / sizeof(int*); i++) {
        fwrite(nbaVig[i], sizeof(int), 2, pinnba);
    }
    fclose(pinnba);

    return 0;
}
