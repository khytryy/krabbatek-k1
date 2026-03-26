#pragma once

#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define RAM_SIZE    16384
extern uint32_t     ram[RAM_SIZE];

typedef struct {
    uint32_t    v;
} reg32bit;

void writeMem(uint32_t addr, uint32_t value);
uint32_t readMem(uint32_t addr, bool fwboot, FILE* fw);