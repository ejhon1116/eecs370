        lw      0       1       target
        jalr    1       1
        lw      0       3       one
        halt
target  lw      0       4       two
        halt
one     .fill   1
two     .fill   2
