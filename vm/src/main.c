#include <stdio.h>
#include <string.h>

void printUsage() {
    printf("Usage:\n\n");
    printf("ktek-k1-vm [BIN] [FW]   -   Instance a new VM with provided files\n");
    printf("    - [BIN]                 Binary file\n");
    printf("    - [FW]                  Firmware file\n");
    printf("ktek-k1-vm help         -   Prints this message\n\n");
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage();
        return 0;
    }

    if (strcmp(argv[1], "help") == 0) {
        printUsage();
        return 0;
    }

    FILE* prog = fopen(argv[1], "rb");
    if (!prog) {
        perror("ERROR: Failed to load binary");
        return 1;
    }

    FILE* fw = fopen(argv[2], "rb");
    if (!fw) {
        perror("ERROR: Failed to load firmware");
        return 1;
    }

    return 0;
}