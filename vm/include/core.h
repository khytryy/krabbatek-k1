#pragma once

#include <memory.h>
#include <stdbool.h>

#define STACK_SIZE  16384

typedef struct {
    uint32_t    v;
} reg32bit;

typedef struct {
    reg32bit    regs[15];
    reg32bit    sp, pc, ar;
    bool        reset, fwboot, privileged;
    bool        lt, gt, zr, eq, ng;

    uint32_t    stack[STACK_SIZE];

    FILE*       fw;
} k1core;

typedef enum {
    TRAP_
} trap;

void        k1coreInit(k1core* chip, FILE* fw);
void        k1coreRun(k1core* chip);

void        pushStack(k1core* chip, uint32_t value);
uint32_t    popStack(k1core* chip);

void        k1coreDecodeAndRun(k1core* chip, uint8_t op, uint32_t* args);

void        k1coreTrap(k1core* chip);

void        k1coreDump(k1core* chip);