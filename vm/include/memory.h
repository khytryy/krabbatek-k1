#pragma once

#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define RAM_SIZE    0x10000
extern uint32_t     ram[RAM_SIZE];

void        writeMem(uint32_t addr, uint32_t value);
uint32_t    readMem(uint32_t addr, bool fwboot, FILE* fw);
uint8_t     readByte(uint32_t addr, bool fwboot, FILE* fw);