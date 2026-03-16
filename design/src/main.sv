`include "memory.sv"

module ram_tb;

    logic[31:0] a;
    logic[13:0] addr;
    logic       load, clk;
    logic[31:0] out;

    int passed = 0;
    int failed = 0;

    ram #(.ADDR_WIDTH(14), .DATA_WIDTH(32)) dut (.*);

    initial clk = 0;
    always #5 clk = ~clk;

    task tick();
        @(posedge clk);
        #2;
    endtask

    task check(input logic[31:0] expected, input string label);
        if (out !== expected) begin
            $display("FAIL [%s]: got %0h, expected %0h", label, out, expected);
            failed++;
        end else begin
            $display("PASS [%s]: out = %0h", label, out);
            passed++;
        end
    endtask

    task write(input logic[13:0] _addr, input logic[31:0] _a);
        addr = _addr; a = _a; load = 1;
        tick();
        load = 0;
    endtask

    task read(input logic[13:0] _addr);
        addr = _addr; load = 0;
        #2;
    endtask

    initial begin
        $display("=== RAM Test Suite ===");

        a = 0; addr = 0; load = 0;

        // Basic write and read
        write(14'h0000, 32'hDEADBEEF);
        read(14'h0000);
        check(32'hDEADBEEF, "write/read 0000");

        write(14'h0001, 32'hCAFEBABE);
        read(14'h0001);
        check(32'hCAFEBABE, "write/read 0001");

        // Different addresses don't interfere
        read(14'h0000);
        check(32'hDEADBEEF, "addr 0000 still holds after writing 0001");

        // Write to last address
        write(14'h3FFF, 32'hFFFFFFFF);
        read(14'h3FFF);
        check(32'hFFFFFFFF, "write/read last addr 3FFF");

        // First address unaffected
        read(14'h0000);
        check(32'hDEADBEEF, "addr 0000 unaffected by last addr write");

        // Overwrite existing value
        write(14'h0000, 32'h12345678);
        read(14'h0000);
        check(32'h12345678, "overwrite 0000");

        // Write zero
        write(14'h0002, 32'h00000000);
        read(14'h0002);
        check(32'h00000000, "write/read zero");

        // Write all ones
        write(14'h0003, 32'hFFFFFFFF);
        read(14'h0003);
        check(32'hFFFFFFFF, "write/read all 1s");

        // Write alternating patterns
        write(14'h0004, 32'hAAAAAAAA);
        read(14'h0004);
        check(32'hAAAAAAAA, "write/read AAAAAAAA");

        write(14'h0005, 32'h55555555);
        read(14'h0005);
        check(32'h55555555, "write/read 55555555");

        // Sequential write then read back
        write(14'h0010, 32'h00000001);
        write(14'h0011, 32'h00000002);
        write(14'h0012, 32'h00000003);
        write(14'h0013, 32'h00000004);

        read(14'h0010); check(32'h00000001, "sequential read 0010");
        read(14'h0011); check(32'h00000002, "sequential read 0011");
        read(14'h0012); check(32'h00000003, "sequential read 0012");
        read(14'h0013); check(32'h00000004, "sequential read 0013");

        // No write when load=0
        a = 32'hDEADBEEF; addr = 14'h0020; load = 0;
        tick();
        read(14'h0020);
        check(32'h00000000, "no write when load=0");

        $display("=== Results: %0d passed, %0d failed ===", passed, failed);
        $display("=== Done ===");
        $finish;
    end

endmodule