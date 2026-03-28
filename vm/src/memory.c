#include <memory.h>

uint32_t ram[RAM_SIZE];

uint32_t sd_sector;
uint32_t sd_addr;
uint32_t sd_status;
FILE*    sd;

void writeMem(uint32_t addr, uint32_t value) {
    if (addr >= MMIO_BASE) {
        writeMMIO(addr, value);
        return;
    }

    if (addr >= RAM_SIZE) {
        fprintf(stderr, "ERROR: Address 0x%X is out of bounds!\n", addr);
        exit(1);
    }

    ram[addr] = value;
}

uint32_t readMem(uint32_t addr, bool fwboot, FILE* fw) {
    if (addr >= MMIO_BASE) {
        return readMMIO(addr);
    }

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
    if (addr >= MMIO_BASE) {
        uint32_t    word        = readMMIO(addr & ~0x3);
        uint8_t     byte_offset = addr & 0x3;

        return (word >> ((3 - byte_offset) * 8)) & 0xFF;
    }

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

void writeMMIO(uint32_t addr, uint32_t value) {
    // SD Card
    if (addr >= SD_BASE && addr < SD_BASE + 0x100) {
        writeSD(addr, value);
        return;
    }

    fprintf(stderr, "ERROR: MMIO Address 0x%X is out of bounds!\n", addr);
    exit(1);
}

uint32_t readMMIO(uint32_t addr) {
    // SD Card
    if (addr >= SD_BASE && addr < SD_BASE + 0x100) {
        return readSD(addr);
    }

    fprintf(stderr, "ERROR: MMIO Address 0x%X is out of bounds!\n", addr);
    exit(1);
}

void writeSD(uint32_t addr, uint32_t value) {
    if      (addr == SD_BASE + SD_REG_CMD)      sdExecCmd(value);
    else if (addr == SD_BASE + SD_REG_SECTOR)   sd_sector = value;
    else if (addr == SD_BASE + SD_REG_ADDR)     sd_addr   = value;
    else {
        fprintf(stderr, "ERROR: SD Address 0x%X is out of bounds!\n", addr);
        exit(1);
    }
}

uint32_t readSD(uint32_t addr) {
    if      (addr == SD_BASE + SD_REG_STATUS)      return sd_status;
    else if (addr == SD_BASE + SD_REG_DATA) {
        uint32_t buffer;
        fread(&buffer, sizeof(uint32_t), 1, sd);

        return buffer;
    }
    else {
        return 0;
    }
}

void sdExecCmd(uint32_t cmd) {
    switch (cmd) {
        case SD_CMD_READ:
            sd_status = SD_STATUS_BUSY;
            fseek(sd, sd_sector * SECTOR_SIZE, SEEK_SET);
            fread(&ram[sd_addr], 1, SECTOR_SIZE, sd);
            sd_status = SD_STATUS_READY;
            break;

        case SD_CMD_WRITE:
            sd_status = SD_STATUS_BUSY;
            fseek(sd, sd_sector * SECTOR_SIZE, SEEK_SET);
            fwrite(&ram[sd_addr], 1, SECTOR_SIZE, sd);
            sd_status = SD_STATUS_READY;
            break;

        case SD_CMD_RESET:
            sd_sector = 0;
            sd_addr   = 0;
            sd_status = SD_STATUS_READY;
            break;

        default:
            fprintf(stderr, "ERROR: Unknown SD command 0x%X\n", cmd);
            sd_status = SD_STATUS_ERROR;
            break;
    }
}