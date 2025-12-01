        lw      0       1       one
        beq     0       0       first
        noop
        noop
first   beq     0       0       second
        add     1       1       2
second  add     1       1       3
        halt
one     .fill   1
