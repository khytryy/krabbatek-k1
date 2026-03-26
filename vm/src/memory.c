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
    if (addr > RAM_SIZE) {
        fprintf(stderr, "ERROR: Address 0x%X is out of bounds!\n", addr);
        exit(1);
    }

    if (fwboot) {
        uint32_t tmp;
        fseek(fw, addr, SEEK_SET);
        fread(&tmp, sizeof(uint32_t), 1, fw);

        return tmp;
    }

    return ram[addr];
}