        lw      0       1       start
        halt
start   add     1       1       1
        .fill   start    ; data .fill referencing a text label -> relocation entry should use D/T? spec says symbol type depends where defined
