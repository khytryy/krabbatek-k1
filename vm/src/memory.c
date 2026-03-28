#include <memory.h>

uint32_t ram[RAM_SIZE];

void writeMem(uint32_t addr, uint32_t value) {
    if (addr > RAM_SIZE) {
        fprintf(stderr, "ERROR: Address 0x%X is out of bounds!\n", addr);
        exit(1);
    }

    ram[addr] = value;
}

uint32_t readMem(uint32_t addr, bool fwboot, FILE* fw) {
    if (addr >= RAM_SIZE) {
        fprintf(stderr, "ERROR: Address 0x%X is out of bounds!\n", addr);
        exit(1);
    }

    if (fwboot) {
        uint8_t bytes[4];
        fseek(fw, addr, SEEK_SET);
        fread(bytes, 1, 4, fw);
        return ((uint32_t)bytes[0] << 24) |
               ((uint32_t)bytes[1] << 16) |
               ((uint32_t)bytes[2] <<  8) |
                (uint32_t)bytes[3];
    }

    return ram[addr];
}

uint8_t readByte(uint32_t addr, bool fwboot, FILE* fw) {
    if (addr >= RAM_SIZE) {
        fprintf(stderr, "ERROR: Address 0x%X is out of bounds!\n", addr);
        exit(1);
    }
    if (fwboot) {
        uint8_t tmp;
        fseek(fw, addr, SEEK_SET);
        fread(&tmp, 1, 1, fw);
        return tmp;
    }
    return ((uint8_t*)ram)[addr];
}