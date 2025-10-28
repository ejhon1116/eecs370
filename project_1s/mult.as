        lw      0       1       mcand
        lw      0       2       mplier
        lw      0       3       zero
        lw      0       4       zero    counter
        lw      0       5       fteen
        lw      0       6       one     ~mask
loop    beq     4       5       done
        nor     6       6       6       get mask from ~mask temporarily
        nor     2       6       7       if its odd, then 2 nor 6 = 0, if even then 2 nor 6 = mask
        nor     6       6       6       get ~mask back
        beq     7       6       even
        add     1       3       3       add the multiplicand if its odd
even    add     1       1       1
        add     6       6       6
        lw      0       7       one
        add     4       7       4
        beq     0       0       loop
done    halt
mcand   .fill   6203
mplier  .fill   1429 
one     .fill   1
zero    .fill   0
fteen   .fill   15
