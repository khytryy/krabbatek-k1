#include <stdio.h>
#include <string.h>

#include <core.h>

void printUsage() {
    printf("Usage:\n\n");
    printf("ktek-k1-vm [FW] [DISK]  -   Instance a new VM with provided files\n");
    printf("    - [FW]                  Firmware file\n");
    printf("    - [DISK]                The main disk\n");
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

    FILE* fw = fopen(argv[1], "rb");
    if (!fw) {
        perror("ERROR: Failed to load firmware");
        return 1;
    }

    FILE* disk = fopen(argv[2], "rb+");
    if (!disk) {
        perror("ERROR: Failed to load disk image");
        return 1;
    }

    k1core chip;
    k1coreInit(&chip, fw);

    sd          = disk;
    sd_status   = SD_STATUS_READY;
    k1coreRun(&chip);

    return 0;
}