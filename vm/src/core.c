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

    chip->lt        = false;
    chip->gt        = false;
    chip->zr        = false;
    chip->eq        = false;

    chip->fw            = fw;
    chip->privileged    = true;
    
}

void k1coreRun(k1core* chip) {
    if (chip->reset) {
        chip->fwboot = true;
    }

    while (true) {
        // Fetch the header
        uint8_t len = readByte(chip->pc.v,     chip->fwboot, chip->fw);
        uint8_t op  = readByte(chip->pc.v + 1, chip->fwboot, chip->fw);
        chip->pc.v += 2;

        uint32_t args[255] = {0};
        for (uint8_t i = 0; i < len; i++) {
            args[i] = readMem(chip->pc.v, chip->fwboot, chip->fw);
            chip->pc.v += 4;
        }

        k1coreDecodeAndRun(chip, op, args);
        k1coreDump(chip);
    }
}

void k1coreReset(k1core* chip) {
    chip->reset = true;
}

void k1coreDecodeAndRun(k1core* chip, uint8_t op, uint32_t* args) {
    switch (op) {
        // NOP
        case 0x09:
            break;

        // LOAD — args: [imm32, reg]
        case 0x0A:
            if (args[1] > 0xF) {
                fprintf(stderr, "ERROR: Unknown register 0x%X\n", args[1]);
                exit(1);
            }
            chip->regs[args[1]].v = args[0];
            break;

        // ADDR — args: [reg1, reg2]
        case 0x0B:
            if (args[0] > 0xF || args[1] > 0xF) {
                fprintf(stderr, "ERROR: Unknown register 0x%X or 0x%X\n", args[0], args[1]);
                exit(1);
            }
            chip->zr = chip->ng = false;
            chip->ar.v = chip->regs[args[0]].v + chip->regs[args[1]].v;
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;

        // SUBR — args: [reg1, reg2]
        case 0x0C:
            if (args[0] > 0xF || args[1] > 0xF) {
                fprintf(stderr, "ERROR: Unknown register 0x%X or 0x%X\n", args[0], args[1]);
                exit(1);
            }
            chip->zr = chip->ng = false;
            chip->ar.v = chip->regs[args[0]].v - chip->regs[args[1]].v;
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;

        // MOVM — args: [reg, addr]
        case 0x0D: {
            if (args[0] > 0xF) {
                fprintf(stderr, "ERROR: Unknown register 0x%X\n", args[0]);
                exit(1);
            }
            uint32_t addr = args[1];
            if (addr >= RAM_SIZE) {
                fprintf(stderr, "ERROR: Address 0x%X out of bounds\n", addr);
                exit(1);
            }
            ram[addr] = chip->regs[args[0]].v;
            break;
        }

        // ADD — args: [imm1, imm2]
        case 0x0E:
            chip->zr = chip->ng = false;
            chip->ar.v = args[0] + args[1];
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;

        // SUB — args: [imm1, imm2]
        case 0x0F:
            chip->zr = chip->ng = false;
            chip->ar.v = args[0] - args[1];
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;

        // JMP — args: [addr]
        case 0x10:
            chip->pc.v = args[0];
            break;

        // CMPM — args: [addr1, addr2]
        case 0x11: {
            if (args[0] >= RAM_SIZE || args[1] >= RAM_SIZE) {
                fprintf(stderr, "ERROR: Address 0x%X or 0x%X out of bounds\n", args[0], args[1]);
                exit(1);
            }
            chip->lt = chip->gt = chip->eq = false;
            int32_t a = (int32_t)readMem(args[0], chip->fwboot, chip->fw);
            int32_t b = (int32_t)readMem(args[1], chip->fwboot, chip->fw);
            if      (a < b) chip->lt = true;
            else if (a > b) chip->gt = true;
            else            chip->eq = true;
            break;
        }

        // CMP — args: [imm1, imm2]
        case 0x12:
            chip->lt = chip->gt = chip->eq = false;
            if      ((int32_t)args[0] < (int32_t)args[1]) chip->lt = true;
            else if ((int32_t)args[0] > (int32_t)args[1]) chip->gt = true;
            else                                           chip->eq = true;
            break;

        // JMPL — args: [addr]
        case 0x13:
            if (chip->lt) chip->pc.v = args[0];
            chip->lt = false;
            break;

        // JMPG — args: [addr]
        case 0x14:
            if (chip->gt) chip->pc.v = args[0];
            chip->gt = false;
            break;

        // JMPZ — args: [addr]
        case 0x15:
            if (chip->zr) chip->pc.v = args[0];
            chip->zr = false;
            break;

        // JMPE — args: [addr]
        case 0x16:
            if (chip->eq) chip->pc.v = args[0];
            chip->eq = false;
            break;

        // JMPN — args: [addr]
        case 0x17:
            if (chip->ng) chip->pc.v = args[0];
            chip->ng = false;
            break;

        // JMPNE — args: [addr]
        case 0x18:
            if (!chip->eq) chip->pc.v = args[0];
            chip->eq = false;
            break;

        // CMPR — args: [reg1, reg2]
        case 0x19: {
            if (args[0] > 0xF || args[1] > 0xF) {
                fprintf(stderr, "ERROR: Unknown register 0x%X or 0x%X\n", args[0], args[1]);
                exit(1);
            }
            chip->lt = chip->gt = chip->eq = false;
            int32_t a = (int32_t)chip->regs[args[0]].v;
            int32_t b = (int32_t)chip->regs[args[1]].v;
            if      (a < b) chip->lt = true;
            else if (a > b) chip->gt = true;
            else            chip->eq = true;
            break;
        }

        // AND — args: [imm32, imm32]
        case 0x1A:
            chip->zr = chip->ng = false;
            chip->ar.v = args[0] & args[1];
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;

        // ANDR — args: [reg1, reg2]
        case 0x1B:
            if (args[0] > 0xF || args[1] > 0xF) {
                fprintf(stderr, "ERROR: Unknown register 0x%X or 0x%X\n", args[0], args[1]);
                exit(1);
            }
            chip->zr = chip->ng = false;
            chip->ar.v = chip->regs[args[0]].v & chip->regs[args[1]].v;
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;

        // PUSH — args: [imm32]
        case 0x1C:
            pushStack(chip, args[0]);
            break;

        // POP — args: [none]
        case 0x1D:
            popStack(chip);
            break;

        // PUSHM — args: [addr]
        case 0x1E:
            if (args[0] >= RAM_SIZE) {
                fprintf(stderr, "ERROR: Address 0x%X out of bounds\n", args[0]);
                exit(1);
            }
            pushStack(chip, readMem(args[0], chip->fwboot, chip->fw));
            break;

        // PUSHR — args: [reg]
        case 0x1F:
            if (args[0] > 0xF) {
                fprintf(stderr, "ERROR: Unknown register 0x%X\n", args[0]);
                exit(1);
            }
            pushStack(chip, chip->regs[args[0]].v);
            break;

        // ANDM — args: [addr1, addr2]
        case 0x20: {
            if (args[0] >= RAM_SIZE || args[1] >= RAM_SIZE) {
                fprintf(stderr, "ERROR: Address 0x%X or 0x%X out of bounds\n", args[0], args[1]);
                exit(1);
            }
            chip->zr = chip->ng = false;
            uint32_t a = readMem(args[0], chip->fwboot, chip->fw);
            uint32_t b = readMem(args[1], chip->fwboot, chip->fw);
            chip->ar.v = a & b;
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;
        }

        // OR — args: [imm32, imm32]
        case 0x21:
            chip->zr = chip->ng = false;
            chip->ar.v = args[0] | args[1];
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;

        // ORR — args: [reg1, reg2]
        case 0x22:
            if (args[0] > 0xF || args[1] > 0xF) {
                fprintf(stderr, "ERROR: Unknown register 0x%X or 0x%X\n", args[0], args[1]);
                exit(1);
            }
            chip->zr = chip->ng = false;
            chip->ar.v = chip->regs[args[0]].v | chip->regs[args[1]].v;
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;

        // ORM — args: [addr1, addr2]
        case 0x23: {
            if (args[0] >= RAM_SIZE || args[1] >= RAM_SIZE) {
                fprintf(stderr, "ERROR: Address 0x%X or 0x%X out of bounds\n", args[0], args[1]);
                exit(1);
            }
            chip->zr = chip->ng = false;
            uint32_t a = readMem(args[0], chip->fwboot, chip->fw);
            uint32_t b = readMem(args[1], chip->fwboot, chip->fw);
            chip->ar.v = a | b;
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;
        }

        // XOR — args: [imm32, imm32]
        case 0x24:
            chip->zr = chip->ng = false;
            chip->ar.v = args[0] ^ args[1];
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;

        // XORR — args: [reg1, reg2]
        case 0x25:
            if (args[0] > 0xF || args[1] > 0xF) {
                fprintf(stderr, "ERROR: Unknown register 0x%X or 0x%X\n", args[0], args[1]);
                exit(1);
            }
            chip->zr = chip->ng = false;
            chip->ar.v = chip->regs[args[0]].v ^ chip->regs[args[1]].v;
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;

        // XORM — args: [addr1, addr2]
        case 0x26: {
            if (args[0] >= RAM_SIZE || args[1] >= RAM_SIZE) {
                fprintf(stderr, "ERROR: Address 0x%X or 0x%X out of bounds\n", args[0], args[1]);
                exit(1);
            }
            chip->zr = chip->ng = false;
            uint32_t a = readMem(args[0], chip->fwboot, chip->fw);
            uint32_t b = readMem(args[1], chip->fwboot, chip->fw);
            chip->ar.v = a ^ b;
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;
        }

        // NOT — args: [imm32]
        case 0x27:
            chip->zr = chip->ng = false;
            chip->ar.v = ~args[0];
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;

        // NOTR — args: [reg1]
        case 0x28:
            if (args[0] > 0xF) {
                fprintf(stderr, "ERROR: Unknown register 0x%X\n", args[0]);
                exit(1);
            }
            chip->zr = chip->ng = false;
            chip->ar.v = ~chip->regs[args[0]].v;
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;

        // NOTM — args: [addr]
        case 0x29: {
            if (args[0] >= RAM_SIZE) {
                fprintf(stderr, "ERROR: Address 0x%X out of bounds\n", args[0]);
                exit(1);
            }
            chip->zr = chip->ng = false;
            uint32_t a = readMem(args[0], chip->fwboot, chip->fw);
            chip->ar.v = ~a;
            if (chip->ar.v == 0)  chip->zr = true;
            if (chip->ar.v >> 31) chip->ng = true;
            break;
        }

        // POPR — args: [reg]
        case 0x2A:
            if (args[0] > 0xF) {
                fprintf(stderr, "ERROR: Unknown register 0x%X\n", args[0]);
                exit(1);
            }
            chip->regs[args[0]].v = popStack(chip);
            break;

        // POPM — args: [addr]
        case 0x2B:
            if (args[0] >= RAM_SIZE) {
                fprintf(stderr, "ERROR: Address 0x%X out of bounds\n", args[0]);
                exit(1);
            }
            writeMem(args[0], popStack(chip));
            break;

        // CALL — args: [addr]
        case 0x2C:
            pushStack(chip, chip->pc.v);
            chip->pc.v = args[0];
            break;

        // RET — args: [none]
        case 0x2D:
            chip->pc.v = popStack(chip);
            break;

        // FWON - args: [none]
        case 0x2E:
            if (!chip->privileged) {

            }

            break;
        
        // FWOFF - args: [none]
        case 0x2F:
            
            break;

        // INT - args: [imm32]
        case 0x30:
            
            break;

        default:
            fprintf(stderr, "ERROR: Unknown opcode 0x%X\n", op);
            exit(1);
    }
}

void pushStack(k1core* chip, uint32_t value) {
    if (chip->sp.v >= STACK_SIZE) {
        fprintf(stderr, "ERROR: Stack overflow\n");
        exit(1);
    }
    chip->stack[chip->sp.v++] = value;
}

uint32_t popStack(k1core* chip) {
    if (chip->sp.v == 0) {
        fprintf(stderr, "ERROR: Stack underflow\n");
        exit(1);
    }
    chip->sp.v--;
    uint32_t val = chip->stack[chip->sp.v];
    chip->stack[chip->sp.v] = 0;
    return val;
}

void k1coreDump(k1core* chip) {
    printf("K1 CPU STATE:\n");
    printf("PC: 0x%X  AR: 0x%X\n", chip->pc.v, chip->ar.v);
    for (int i = 0; i < 16; i++)
        printf("r%d: 0x%X\n", i, chip->regs[i].v);
    printf("zr=%d ng=%d lt=%d gt=%d eq=%d\n",
        chip->zr, chip->ng, chip->lt, chip->gt, chip->eq);
    printf("\n");
}