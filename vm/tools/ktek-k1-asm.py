#!/usr/bin/env python3

import sys
import struct

BEGIN_ADDR = 0x0

INSTRUCTIONS = {
    "NOP":   (0x09, 0),
    "LOAD":  (0x0A, 2),  # imm32, reg
    "ADDR":  (0x0B, 2),  # reg, reg
    "SUBR":  (0x0C, 2),  # reg, reg
    "MOVM":  (0x0D, 2),  # reg, addr
    "ADD":   (0x0E, 2),  # imm, imm
    "SUB":   (0x0F, 2),  # imm, imm
    "JMP":   (0x10, 1),  # addr
    "CMPM":  (0x11, 2),  # addr, addr
    "CMP":   (0x12, 2),  # imm, imm
    "JMPL":  (0x13, 1),  # addr
    "JMPG":  (0x14, 1),  # addr
    "JMPZ":  (0x15, 1),  # addr
    "JMPE":  (0x16, 1),  # addr
    "JMPN":  (0x17, 1),  # addr
    "JMPNE": (0x18, 1),  # addr
    "CMPR":  (0x19, 2),  # reg, reg
    "AND":   (0x1A, 2),  # imm, imm
    "ANDR":  (0x1B, 2),  # reg, reg
    "PUSH":  (0x1C, 1),  # imm
    "POP":   (0x1D, 0),
    "PUSHM": (0x1E, 1),  # addr
    "PUSHR": (0x1F, 1),  # reg
    "ANDM":  (0x20, 2),  # addr, addr
    "OR":    (0x21, 2),  # imm, imm
    "ORR":   (0x22, 2),  # reg, reg
    "ORM":   (0x23, 2),  # addr, addr
    "XOR":   (0x24, 2),  # imm, imm
    "XORR":  (0x25, 2),  # reg, reg
    "XORM":  (0x26, 2),  # addr, addr
    "NOT":   (0x27, 1),  # imm
    "NOTR":  (0x28, 1),  # reg
    "NOTM":  (0x29, 1),  # addr
    "POPR":  (0x2A, 1),  # reg
    "POPM":  (0x2B, 1),  # addr
    "CALL":  (0x2C, 1),  # addr
    "RET":   (0x2D, 0),
    "FWON":  (0x2E, 0),
    "FWOFF": (0x2F, 0),
    "INT":    (0x30, 1), # imm
    "PUSHAR": (0x31, 0),
    "JMPU":   (0x32, 1),  # addr
    "LOADSP": (0x33, 1),  # reg

}

REGISTERS = {f"r{i}": i for i in range(16)}

def parse_arg(token, line_num):
    token = token.lower()
    if token in REGISTERS:
        return REGISTERS[token]
    try:
        if token.startswith("0x"):
            return int(token, 16)
        return int(token)
    except ValueError:
        print(f"ERROR line {line_num}: Unknown argument '{token}'")
        sys.exit(1)


def assemble(src_path, out_path):
    labels = {}
    address = BEGIN_ADDR

    with open(src_path) as f:
        lines = f.readlines()

    # First pass: collect labels
    for line_num, line in enumerate(lines, 1):
        line = line.split(";")[0].strip()
        if not line:
            continue

        if line.startswith(".org"):
            new_addr = int(line.split()[1], 16)
            if new_addr < address:
                print(f"ERROR line {line_num}: .org 0x{new_addr:X} is behind current address 0x{address:X}")
                sys.exit(1)
            address = new_addr
            continue

        if line.endswith(":"):
            labels[line[:-1].strip()] = address
            continue

        tokens = line.split()
        mnemonic = tokens[0].upper()
        if mnemonic not in INSTRUCTIONS:
            print(f"ERROR line {line_num}: Unknown mnemonic '{mnemonic}'")
            sys.exit(1)

        _, argc = INSTRUCTIONS[mnemonic]
        address += 2 + argc * 4

    # Second pass: emit binary
    out = bytearray()
    out += b'\x00' * BEGIN_ADDR
    address = BEGIN_ADDR

    for line_num, line in enumerate(lines, 1):
        line = line.split(";")[0].strip()
        if not line or line.endswith(":"):
            continue

        if line.startswith(".org"):
            new_addr = int(line.split()[1], 16)
            out += b'\x00' * (new_addr - address)  # pad to new address
            address = new_addr
            continue

        tokens = line.split()
        mnemonic = tokens[0].upper()
        opcode, argc = INSTRUCTIONS[mnemonic]
        arg_tokens = [t.rstrip(",") for t in tokens[1:]]

        if len(arg_tokens) != argc:
            print(f"ERROR line {line_num}: '{mnemonic}' expects {argc} args, got {len(arg_tokens)}")
            sys.exit(1)

        args = []
        for t in arg_tokens:
            if t in labels:
                args.append(labels[t])
            else:
                args.append(parse_arg(t, line_num))

        out += bytes([argc, opcode])
        for a in args:
            out += struct.pack(">I", a)
        address += 2 + argc * 4

    with open(out_path, "wb") as f:
        f.write(out)

    print(f"Wrote {len(out)} bytes to {out_path}")
    for name, addr in labels.items():
        print(f"  {name} located at:    0x{addr:X}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <input.k1asm> <output.bin/fw>")
        sys.exit(1)

    if sys.argv[1] == "version":
        print("KrabbaTek K1 Assembler (ktek-k1-asm) 1.0.0-dev")
        print("Copyright © 2026 KrabbaTek Group")
        sys.exit(0)

    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input.k1asm> <output.bin/fw>")
        sys.exit(1)

    assemble(sys.argv[1], sys.argv[2])