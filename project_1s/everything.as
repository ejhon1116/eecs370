        lw      0       1       d1
        lw      0       2       d2
        add     1       2       3
        nor     1       2       4
        sw      0       3       res1
        lw      0       5       res1
        beq     3       5       cont
        halt
cont    noop
        lw      0       6       target
        jalr    6       7
        halt
target  lw      0       8       final
        halt
d1      .fill   10
d2      .fill   20
res1    .fill   0
final   .fill   99
