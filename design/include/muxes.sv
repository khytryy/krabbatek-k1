module mux2to1
(
    input logic     a,
    input logic     b,
    input logic     s,

    output wire     out
);

    wire o0, o1, o2;

    not u0(o1, s);

    and u1(o0, a, o1);
    and u2(o2, b, s);

    or  u3(out, o0, o2);

endmodule