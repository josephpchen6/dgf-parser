#include <stdio.h>

int main() {
    FILE *dknba = fopen("dknba.bin", "wb");
    int nbaVig[][2] = {
        {-108, -112}, {-115, -115}, {-110, -120}, {-105, -125}, {100, -130}, {105, -135}, {110, -140}, {114, -145}, {120, -154},
        {124, -160}, {130, -166}, {135, -175}, {140, -180}, {145, -188}, {150, -195}, {154, -200}, {155, -205}, {160, -210},
        {165, -215}, {170, -220}, {175, -230}, {180, -238}, {185, -245}, {190, -250}, {195, -260}, {205, -275}, {210, -280}
    }; // NBA moneyline markets are generally tighter, but they never have value anyway
    for (size_t i = 0; i < sizeof(nbaVig); i++) {
        fwrite(nbaVig[i], sizeof(int), 2, dknba);
    }
    fclose(dknba);

    FILE *dknfl = fopen("dknfl.bin", "wb");
    int nflVig[][2] = {
        {-108, -112}, {-115, -115}, {-110, -120}, {-650, 390}, {-670, 400}, {-980, 515}
    };
    for (size_t i = 0; i < sizeof(nflVig); i++) {
        fwrite(nflVig[i], sizeof(int), 2, dknfl);
    }
    fclose(dknfl);
    return 0;
}