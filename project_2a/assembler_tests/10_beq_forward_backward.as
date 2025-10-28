start   lw      0       1       five
        beq     0       1       Done        ; forward branch to a global label defined later in this file
        beq     0       0       start       ; backward branch to start (offset negative)
        add     1       1       1
Done    halt
five    .fill   5
