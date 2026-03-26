#include <core.h>

void k1coreInit(k1core* chip, FILE* fw) {
    for (size_t i = 0; i < 15; i++) {
        chip->regs[i].v = 0;
    }
    chip->sp.v      = 0;
    chip->pc.v      = 0x6B31; // reset vector
    chip->reset     = true;
    chip->fwboot    = false;
    chip->ar.v      = 0;

    chip->fw        = fw;
    
}

void k1coreRun(k1core* chip) {
    if (chip->reset) {
        // FWBOOT must be pulled high to access the firmware ROM
        chip->fwboot = true;
    }

    // Fetch the instruction from firmware ROM
    uint32_t ins = readMem(chip->pc.v, chip->fwboot, chip->fw);

    k1coreDecodeAndRun(chip, ins);
}

void k1coreReset(k1core* chip) {
    chip->reset = true;
}

void k1coreDecodeAndRun(k1core* chip, uint32_t ins) {
    uint8_t op = (ins >> 24) & 0xFF;
    
    uint8_t a1  = (ins >> 16) & 0xFF;
    uint8_t a2  = (ins >> 8)  & 0xFF;
    uint8_t a3  = ins & 0xFF;

    switch (op) {
        // NOP
        case 0x9:
            break;
        // LOAD
        case 0xA:
            if (a2 > 0xF) {
                fprintf(stderr, "ERROR: Unknown register 0x%X\n", a2);
                exit(1);
            }

            chip->regs[a2].v = a1;
            break;
        // ADDR
        case 0xB:
            if (a1 || a2 > 0xF) {
                fprintf(stderr, "ERROR: Can't add registers 0x%X and 0x%X\n", a1, a2);
                exit(1);
            }
            chip->ar.v = chip->regs[a1].v + chip->regs[a2].v;
            break;
        
        // SUBR
        case 0xC:
            if (a1 || a2 > 0xF) {
                fprintf(stderr, "ERROR: Can't add registers 0x%X and 0x%X\n", a1, a2);
                exit(1);
            }
            chip->ar.v = chip->regs[a1].v - chip->regs[a2].v;
            break;

        // MOVM
        case 0xD:
            if (a1 > 0xF) {
                fprintf(stderr, "ERROR: Unknown register 0x%X\n", a1);
                exit(1);
            }

            if (a2 > RAM_SIZE) {
                
            }
            break;
        
        default:
        
    }
}