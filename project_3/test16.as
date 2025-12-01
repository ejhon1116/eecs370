        lw      0   1   zero
        lw      0   4   zero
        lw      0   3   max
        lw      0   2   one
loop    beq     1   3   fin
        add     1   2   1
        beq     1   2   loop
        add     4   2   4
        nor     4   2   4
        noop
        add     4   2   4
        beq     0   0   loop
fin     halt
max     .fill   10
one     .fill   1
zero    .fill   0
