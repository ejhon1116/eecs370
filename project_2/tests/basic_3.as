Start   add     1       2       1       decrement reg1
        beq     0       1       2       goto end of program when reg1==0
        beq     0       0       Start   go back to the beginning of the loop
        noop
        lw      0       1       one
        lw      0       2       Potato
        nor     1       2       3
        noop
        noop
        noop
done    halt                            end of program
one     .fill   1
