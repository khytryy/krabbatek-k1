module adder
(
    input logic a,
    input logic b,
    input logic cin,

    output logic res,
    output logic cout
);

    wire o0;
    wire o1;
    wire o3;

    xor u1(o0, a, b);
    and u2(o1, a, b);

    and
    
endmodule