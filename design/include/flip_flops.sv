module dFlipFlop
(
    input logic     d,
    input logic     clk,

    output logic    q
);

    always @(posedge clk) begin
        q <= d;
    end

endmodule