        lw      0       1       MissingGlobal
        lw      0       2       MissingGlobal
        halt
        .fill   0

; Expect assembler to add MissingGlobal once to the symbol table as U and two relocation entries for the two uses.
