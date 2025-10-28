        lw	    0	1	num1
        lw	    0	2   Five
        add	    1	2	3
        nor	    1	2	4
        sw	    3	0	result
loop	beq	    1	2	end
        jalr	7	6
        noop
end	    sw      5   7   Stack
        lw      0   6   one
        add     5   6   5
        lw      0   4   loop
        lw      0   4   Start
        jalr    4   7
        halt
one     .fill   1
num1	.fill	3
result	.fill	0
