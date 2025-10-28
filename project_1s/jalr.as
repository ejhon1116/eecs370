        lw      0       1       skipTo
        jalr    1       2
        lw      0       3       one
        halt
skipTo  lw      0       4       two
        halt
one     .fill   1
two     .fill   2
