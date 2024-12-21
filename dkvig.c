#include <stdio.h>

int main() {
    FILE *file = fopen("dkvig.bin", "wb");
    if (!file) {
        perror("Could not open file");
        return 1;
    }

    int data[][2] = {
        {-115, -115}, {-110, -120}, {-105, -125}, {100, -130}, {105, -135}, {110, -140}, {114, -145}, {120, -154}, {124, -160},
        {130, -166}, {135, -175}, {140, -180}
    };
    for (size_t i = 0; i < sizeof(data); i++) {
        fwrite(data[i], sizeof(int), 2, file);
    }

    fclose(file);
    return 0;
}


// -145/+114 = 31
// -154/+120 = 34
// -160/+124 = 36
// -166/+130 = 36
// -175/+135 = 40
// -180/+140 = 40