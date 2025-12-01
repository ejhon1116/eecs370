        lw      0   1   poo
        beq     0   1   1
        beq     0   0   there
        nor     0   1   1
        nor     1   1   1
        add     3   2   1
there   lw      0   1   poo2
        add     1   1   1
        lw      0   2   poo3
        add     1   2   3
        halt
poo     .fill   3
poo2    .fill   2
poo3    .fill   100
