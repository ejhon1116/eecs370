Start	lw	0	1	data
loop	beq	1	0	End
        lw	0	2	neg
        add	1	2	1
        beq	0	0	loop
End	    halt
data	.fill	5
neg	    .fill	-1
