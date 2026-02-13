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

module adder8bit
(
    input logic[7:0]    a,
    input logic[7:0]    b,
    input logic         cin,

    output logic[7:0]   res,
    output logic        cout
);

    logic c0;

    adder4bit u0(a[3:0],    b[3:0],     cin,    res[3:0],   c0);
    adder4bit u1(a[7:4],    b[7:4],     c0,     res[7:4],   cout);

endmodule

module adder16bit
(
    input logic[15:0]   a,
    input logic[15:0]   b,
    input logic         cin,

    output logic[15:0]  res,
    output logic        cout
);

    logic c0;

    adder8bit u0(a[7:0],    b[7:0],     cin,    res[7:0],   c0);
    adder8bit u1(a[15:8],   b[15:8],    c0,     res[15:8]   cout);

endmodule

module adder32bit
()
endmodule