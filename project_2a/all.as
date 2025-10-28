        lw	0	1	num1
        lw	0	2	num2
        add	1	2	3
        nor	1	2	4
        sw	3	0	result
loop	beq	1	2	end
        jalr	7	6
        noop
end	    halt
num1	.fill	5
num2	.fill	3
result	.fill	0
