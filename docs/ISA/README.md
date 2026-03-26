# ktek-k1 ISA

## Instruction Encoding

| Bits  | 31–24  | 23–16 | 15–8 | 7–0  |
|-------|--------|-------|------|------|
| Field | OPCODE | ARG1  | ARG2 | ARG3 |

## Opcode Table

| Opcode | Mnemonic | ARG1    | ARG2    | ARG3 | Operation                        |
|--------|----------|---------|---------|------|----------------------------------|
| `0x09` | `NOP`    | —       | —       | —    | No operation                     |
| `0x0A` | `LOAD`   | `imm8`  | `reg`   | —    | `reg[ARG2] = ARG1`               |
| `0x0B` | `ADDR`   | `reg`   | `reg`   | —    | `ar = reg[ARG1] + reg[ARG2]`     |
| `0x0C` | `SUBR`   | `reg`   | `reg`   | —    | `ar = reg[ARG1] - reg[ARG2]`     |
| `0x0D` | `MOVM`   | `reg`   | `addr`  | —    | `mem[ARG2] = reg[ARG1]`          |