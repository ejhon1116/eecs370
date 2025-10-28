        lw      0       1       ten
        lw      0       2       ten
        beq     1       2       skip
        lw      0       3       one
        halt
skip    lw      0       3       two
        halt
ten     .fill   10
one     .fill   1
two     .fill   2
