        lw      0   1   poo
        lw      0   2   glub
        add     1   2   1
        noop
        sw      0   1   poo
        sw      0   2   glub
        lw      0   1   stoo
        lw      0   2   stoo
        noop
        halt
poo     .fill   3
glub    .fill   4
stoo    .fill   5

