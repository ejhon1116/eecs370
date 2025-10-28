        lw      0       1       neg
        lw      0       2       five
        add     1       2       3
        nor     1       2       4
        beq     3       2       equal
        halt
equal   lw      0       5       zero
        beq     5       0       suc
        halt
suc     halt
neg     .fill   -1
five    .fill   5
zero    .fill   0
