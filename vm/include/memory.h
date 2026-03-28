#pragma once

#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define RAM_SIZE    0x10000
extern uint32_t     ram[RAM_SIZE];

#define MMIO_BASE       0x10000000
#define SD_BASE         0x10000000

#define SD_REG_CMD      0x00  // Command register
#define SD_REG_STATUS   0x04  // Status register
#define SD_REG_DATA     0x08  // Data register
#define SD_REG_SECTOR   0x0C  // Sector number to write/read
#define SD_REG_ADDR     0x10

#define SD_CMD_READ     0x1  // Read one sector from SD into RAM at SD_REG_ADDR
#define SD_CMD_WRITE    0x2  // Write one sector from RAM at SD_REG_ADDR to SD
#define SD_CMD_RESET    0x3  // Reset the controller

#define SD_STATUS_READY 0x0
#define SD_STATUS_BUSY  0x1
#define SD_STATUS_ERROR 0x2

#define SECTOR_SIZE     0x200 // 512 bytes

extern uint32_t sd_sector;
extern uint32_t sd_addr;
extern uint32_t sd_status;
extern FILE*    sd;

void        writeMem(uint32_t addr, uint32_t value);
uint32_t    readMem(uint32_t addr, bool fwboot, FILE* fw);
uint8_t     readByte(uint32_t addr, bool fwboot, FILE* fw);

void        writeMMIO(uint32_t addr, uint32_t value);
uint32_t    readMMIO(uint32_t addr);

void        writeSD(uint32_t addr, uint32_t value);
uint32_t    readSD(uint32_t addr);

void sdExecCmd(uint32_t cmd);