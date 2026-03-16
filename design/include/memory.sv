`include "flip_flops.sv"
`include "muxes.sv"

module reg1bit
(
    input logic     a,
    input logic     load,
    input logic     clk,

    output logic    s
);

    wire o0;

    mux2to1 u0(s, a, load, o0);
    dFlipFlop u1(o0, clk, s);

endmodule

module reg4bit
(
    input logic[3:0]    a,
    input logic         load,
    input logic         clk,

    output logic[3:0]   s
);

    reg1bit u0(a[0], load, clk, s[0]);
    reg1bit u1(a[1], load, clk, s[1]);
    reg1bit u2(a[2], load, clk, s[2]);
    reg1bit u3(a[3], load, clk, s[3]);

endmodule

module reg8bit
(
    input logic[7:0]    a,
    input logic         load,
    input logic         clk,

    output logic[7:0]   s
);

    reg4bit u0(a[3:0], load, clk, s[3:0]);
    reg4bit u1(a[7:4], load, clk, s[7:4]);

endmodule

module reg16bit
(
    input logic[15:0]    a,
    input logic          load,
    input logic          clk,

    output logic[15:0]   s
);

    reg8bit u0(a[7:0],  load, clk, s[7:0]);
    reg8bit u1(a[15:8], load, clk, s[15:8]);

endmodule

module reg32bit
(
    input logic[31:0]    a,
    input logic         load,
    input logic         clk,

    output logic[31:0]   s
);

    reg16bit u0(a[15:0],  load, clk, s[15:0]);
    reg16bit u1(a[31:16], load, clk, s[31:16]);

endmodule

module programCounter
(
    input logic[31:0]   a,
    input logic         inc,
    input logic         load,
    input logic         reset,
    input logic         clk,

    output logic[31:0]  out
);

    logic[31:0] next, o0;
    logic       reg_load;

    always_comb begin
        if (reset) begin
            next     = 32'b0;
            reg_load = 1;
        end else if (load) begin
            next     = a;
            reg_load = 1;
        end else if (inc) begin
            next     = o0 + 1;
            reg_load = 1;
        end else begin
            next     = o0;
            reg_load = 0;
        end
    end

    reg32bit u0(next, reg_load, clk, o0);

    assign out = o0;

endmodule

module ram #(
    parameter ADDR_WIDTH = 14,
    parameter DATA_WIDTH = 32
)(
    input logic[DATA_WIDTH-1:0]     a,
    input logic[ADDR_WIDTH-1:0]     addr,
    input logic                     load,
    input logic                     clk,

    output logic[DATA_WIDTH-1:0]    out
);

    localparam DEPTH = 2**ADDR_WIDTH;

    logic[DATA_WIDTH-1:0] mem [0:DEPTH-1];

    always @(posedge clk) begin
        if (load)
            mem[addr] <= a;
    end

    assign out = mem[addr];

endmodule