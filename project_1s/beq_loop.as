        lw      0       1       i
        lw      0       2       zero
loop    beq     1       2       done
        add     1       2       1
        beq     0       0       loop
done    halt
i       .fill   5
zero    .fill   0
