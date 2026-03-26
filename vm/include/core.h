#pragma once

#include <memory.h>
#include <stdbool.h>

typedef struct {
    reg32bit    regs[15];
    reg32bit    sp, pc, ar;
    bool        reset, fwboot;

    FILE*       fw;
} k1core;

void k1coreInit(k1core* chip, FILE* fw);
void k1coreRun(k1core* chip);

void k1coreDecodeAndRun(k1core* chip, uint32_t ins);