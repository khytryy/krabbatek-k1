module adder
(
    input logic     a,
    input logic     b,
    input logic     cin,

    output logic    res,
    output logic    cout
);

    wire o0, o1, o3;

    xor     u1(o0, a, b);
    and     u2(o1, a, b);

    and     u3(o3, o0, cin);
    xor     u4(res, o0, cin);

    or      u5(cout, o3, o1);
    
endmodule

module adder4bit
(
    input logic[3:0]    a,
    input logic[3:0]    b,
    input logic         cin,

    output logic[3:0]   res,
    output logic        cout
);

    logic c0, c1, c2;

    adder u0(a[0],  b[0],   cin,    res[0],     c0);
    adder u1(a[1],  b[1],   c0,     res[1],     c1);
    adder u2(a[2],  b[2],   c1,     res[2],     c2);
    adder u3(a[3],  b[3],   c2,     res[3],     cout);

endmodule