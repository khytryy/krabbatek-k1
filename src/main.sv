`include "alu.sv"

module alu_tb;

    logic[31:0] a, b;
    logic       za, na, zb, nb, no;
    logic[2:0]  op;
    logic[31:0] out;
    logic       lt, gt, zr;

    int passed = 0;
    int failed = 0;

    alu dut (.*);

    task apply(
        input logic[31:0] _a, _b,
        input logic _za, _na, _zb, _nb,
        input logic[2:0] _op,
        input logic _no
    );
        a = _a; b = _b;
        za = _za; na = _na;
        zb = _zb; nb = _nb;
        op = _op; no = _no;
        #20;
    endtask

    task check(input logic[31:0] expected, input string label);
        if (out !== expected) begin
            $display("FAIL [%s]: got %0d, expected %0d", label, out, expected);
            failed++;
        end else begin
            $display("PASS [%s]: out = %0d", label, out);
            passed++;
        end
    endtask

    initial begin
        $display("=== ALU Test Suite ===");

        // ADD
        apply(32'd10, 32'd20, 0,0,0,0, 3'b001, 0);
        check(32'd30, "ADD 10+20");

        apply(32'd0, 32'd0, 0,0,0,0, 3'b001, 0);
        check(32'd0, "ADD 0+0");

        apply(32'hFFFFFFFF, 32'd1, 0,0,0,0, 3'b001, 0);
        check(32'd0, "ADD overflow wrap");

        // AND
        apply(32'hFF00FF00, 32'h0F0F0F0F, 0,0,0,0, 3'b010, 0);
        check(32'h0F000F00, "AND");

        apply(32'hFFFFFFFF, 32'h00000000, 0,0,0,0, 3'b010, 0);
        check(32'h00000000, "AND with zero");

        // OR
        apply(32'hF0F0F0F0, 32'h0F0F0F0F, 0,0,0,0, 3'b011, 0);
        check(32'hFFFFFFFF, "OR");

        apply(32'h00000000, 32'h00000000, 0,0,0,0, 3'b011, 0);
        check(32'h00000000, "OR zeros");

        // XOR
        apply(32'hFFFFFFFF, 32'hFFFFFFFF, 0,0,0,0, 3'b100, 0);
        check(32'h00000000, "XOR same");

        apply(32'hAAAAAAAA, 32'h55555555, 0,0,0,0, 3'b100, 0);
        check(32'hFFFFFFFF, "XOR alternating");

        // NOT A
        apply(32'h00000000, 32'h0, 0,0,0,0, 3'b101, 0);
        check(32'hFFFFFFFF, "NOT A of 0");

        apply(32'hFFFFFFFF, 32'h0, 0,0,0,0, 3'b101, 0);
        check(32'h00000000, "NOT A of all 1s");

        // NOT B
        apply(32'h0, 32'h00000000, 0,0,0,0, 3'b110, 0);
        check(32'hFFFFFFFF, "NOT B of 0");

        apply(32'h0, 32'hFFFFFFFF, 0,0,0,0, 3'b110, 0);
        check(32'h00000000, "NOT B of all 1s");

        // za/zb
        apply(32'hDEADBEEF, 32'hCAFEBABE, 1,0,0,0, 3'b001, 0);
        check(32'hCAFEBABE, "za: A zeroed, result = B");

        apply(32'hDEADBEEF, 32'hCAFEBABE, 0,0,1,0, 3'b001, 0);
        check(32'hDEADBEEF, "zb: B zeroed, result = A");

        // na/nb
        apply(32'h0000000F, 32'h0, 0,1,0,0, 3'b101, 0);
        check(32'h0000000F, "na+NOTA: ~(~A) = A");

        apply(32'h0, 32'h0000000F, 0,0,0,1, 3'b110, 0);
        check(32'h0000000F, "nb+NOTB: ~(~B) = B");

        // no
        apply(32'h00000000, 32'h0, 0,0,0,0, 3'b010, 1);
        check(32'hFFFFFFFF, "no: ~AND(0,0)");

        apply(32'hFFFFFFFF, 32'hFFFFFFFF, 0,0,0,0, 3'b010, 1);
        check(32'h00000000, "no: ~AND(all1s,all1s)");

        // Flags
        apply(32'd0, 32'd0, 0,0,0,0, 3'b001, 0);
        if (zr) begin $display("PASS [flag zr]: zero flag set"); passed++; end
        else    begin $display("FAIL [flag zr]: expected zr=1"); failed++; end

        apply(32'hFFFFFFFF, 32'd0, 0,0,0,0, 3'b001, 0);
        if (lt) begin $display("PASS [flag lt]: lt set for negative"); passed++; end
        else    begin $display("FAIL [flag lt]: expected lt=1, out=%h", out); failed++; end

        apply(32'd1, 32'd0, 0,0,0,0, 3'b001, 0);
        if (gt) begin $display("PASS [flag gt]: gt set for positive"); passed++; end
        else    begin $display("FAIL [flag gt]: expected gt=1, out=%h", out); failed++; end

        // NOP
        apply(32'hDEADBEEF, 32'hCAFEBABE, 0,0,0,0, 3'b000, 0);
        check(32'h00000000, "NOP default");

        $display("=== Results: %0d passed, %0d failed ===", passed, failed);
        $display("=== Done ===");
        $finish;
    end

endmodule