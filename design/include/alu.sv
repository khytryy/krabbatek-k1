`include "adders.sv"

module alu
(
    input logic[31:0]   a,
    input logic[31:0]   b,
    
    input logic         za,
    input logic         na,
    input logic         zb,
    input logic         nb,
    input logic[2:0]    op,
    input logic         no,

    output logic        lt,
    output logic        gt,
    output logic        zr,
    output logic        ng,
    output logic[31:0]  out
);

    logic[31:0] temp_a, temp_b, raw_out, add_result;

    adder32bit u0(
        .a   (temp_a),
        .b   (temp_b),
        .cin (1'b0),
        .res (add_result)
    );

    always_comb begin
        // Zero / negate A
        temp_a = za ? 32'b0 : a;
        if (na) temp_a = ~temp_a;

        // Zero / negate B
        temp_b = zb ? 32'b0 : b;
        if (nb) temp_b = ~temp_b;

        // Opcodes
        case (op)
            // ADD, AND, OR, XOR, NOT A, NOT B
            3'b001: raw_out = add_result;
            3'b010: raw_out = temp_a & temp_b;
            3'b011: raw_out = temp_a | temp_b;
            3'b100: raw_out = temp_a ^ temp_b;
            3'b101: raw_out = ~temp_a;
            3'b110: raw_out = ~temp_b;
            default: raw_out = 32'b0;
        endcase

        out = no ? ~raw_out : raw_out;
    end

    assign zr = (out == 32'b0);
    assign lt = out[31];
    assign gt = !zr && !lt;
    assign ng = out[31];

endmodule