        lw          0       1       n
        lw          0       2       r
        lw          0       4       Faddr        load function address
        jalr        4       7                    call function
        halt
func    beq         0       2       base
        beq         1       2       base        if r==0 or n==r
else    lw          0       6       one
        sw          5       1       Stack
        add         5       6       5
        sw          5       2       Stack
        add         5       6       5
        sw          5       7       Stack
        add         5       6       5           Save inputs and return address to the Stack
        lw          0       6       negOne
        add         1       6       1           n = n-1
        lw          0       4       Faddr
        jalr        4       7
        lw          0       6       negOne
        add         2       6       2           r = r-1
        lw          0       6       one
        sw          5       3       Stack
        add         5       6       5           Save the recursive call output to the stack
        lw          0       4       Faddr
        jalr        4       7
        lw          0       6       negOne
        add         5       6       5
        lw          5       4       Stack
        add         3       4       3
        lw          0       6       negOne
        add         5       6       5
        lw          5       7       Stack
        add         5       6       5   
        lw          5       2       Stack
        add         5       6       5
        lw          5       1       Stack       Reload the inputs and return address
        beq         0       0       return
base    lw          0       3       one
return  jalr        7       4   
n       .fill       7
r       .fill       3
Faddr   .fill       func
one     .fill       1
negOne  .fill       -1
