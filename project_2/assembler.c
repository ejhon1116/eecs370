/**
 * Project 1
 * Assembler code fragment for LC-2K
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Every LC2K file will contain less than 1000 lines of assembly.
#define MAXLINELENGTH 1000

typedef struct {
    bool used;
    char name[7];
    int address;
} Label;

typedef struct {
    char name[7];
    char type;
    int address;
} Symbol;

typedef struct {
    char instruction[6];
    char name[7];
    int address;
} Relocation;

int readAndParse(FILE *, char *, char *, char *, char *, char *);
static void checkForBlankLinesInCode(FILE *inFilePtr);
static inline int isNumber(char *);
static inline void printHexToFile(FILE *, int);
static Label lookupLabel(char *, bool);
static inline void checkRegisterErrors(char *, char *);
static inline void check3RegisterErrors(char *, char *, char *);

static int add_instr = 0b000 << 22;
static int nor_instr = 0b001 << 22;
static int lw_instr = 0b010 << 22;
static int sw_instr = 0b011 << 22;
static int beq_instr = 0b100 << 22;
static int jalr_instr = 0b101 << 22;
static int halt_instr = 0b110 << 22;
static int noop_instr = 0b111 << 22;

static Label labelTable[MAXLINELENGTH] = {(Label){false, "", -1}};

int main(int argc, char **argv) {
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
        arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    Symbol symbols[MAXLINELENGTH] = {0};
    Relocation relocations[MAXLINELENGTH] = {0};

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
               argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }

    // Check for blank lines in the middle of the code.
    checkForBlankLinesInCode(inFilePtr);

    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    int address = 0;
    int data_start = -1;
    int t = 0;
    int d = 0;
    int s = 0;
    int r = 0;

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        if (!strcmp(opcode, ".fill")) {
            if (data_start == -1)
                data_start = address;
        }

        if (strlen(label) > 0) {
            if (lookupLabel(label, false).used) {
                printf("error: multiple labels defined: %s\n", label);
                exit(1);
            }
            strncpy(labelTable[address].name, label, 7);
            if (isupper(labelTable[address].name[0])) {
                strncpy(symbols[s].name, label, 7);
                if (!strcmp(opcode, ".fill")) {
                    symbols[s].type = 'D';
                    symbols[s].address = address - data_start;
                } else {
                    symbols[s].type = 'T';
                    symbols[s].address = address;
                }
                s++;
            }
            labelTable[address].address = address;
            labelTable[address].used = true;
        }
        address++;
    }

    rewind(inFilePtr);
    address = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        if (!strcmp(opcode, "lw")) {
            checkRegisterErrors(arg0, arg1);
            if (!isNumber(arg2)) {
                Label l = lookupLabel(arg2, true);

                strncpy(relocations[r].name, arg2, 7);
                strcpy(relocations[r].instruction, opcode);
                relocations[r].address = address;
                r++;

                if (!l.used) {
                    bool already_in_table = false;
                    for (int i = 0; i < MAXLINELENGTH; i++) {
                        if (!strcmp(symbols[i].name, arg2)) {
                            already_in_table = true;
                            break;
                        }
                    }
                    if (!already_in_table) {
                        strncpy(symbols[s].name, arg2, 7);
                        symbols[s].type = 'U';
                        symbols[s].address = 0;
                        s++;
                    }
                }
            }
        } else if (!strcmp(opcode, "sw")) {
            checkRegisterErrors(arg0, arg1);
            if (!isNumber(arg2)) {
                Label l = lookupLabel(arg2, true);

                strncpy(relocations[r].name, arg2, 7);
                strcpy(relocations[r].instruction, opcode);
                relocations[r].address = address;
                r++;

                if (!l.used) {
                    bool already_in_table = false;
                    for (int i = 0; i < MAXLINELENGTH; i++) {
                        if (!strcmp(symbols[i].name, arg2)) {
                            already_in_table = true;
                            break;
                        }
                    }
                    if (!already_in_table) {
                        strncpy(symbols[s].name, arg2, 7);
                        symbols[s].type = 'U';
                        symbols[s].address = 0;
                        s++;
                    }
                }
            }
        }
        if (!strcmp(opcode, ".fill")) {
            d++;
            if (!isNumber(arg0)) {
                Label l = lookupLabel(arg0, true);

                strncpy(relocations[r].name, arg0, 7);
                strcpy(relocations[r].instruction, opcode);
                relocations[r].address = address - data_start;
                r++;

                if (!l.used) {
                    bool already_in_table = false;
                    for (int i = 0; i < MAXLINELENGTH; i++) {
                        if (!strcmp(symbols[i].name, arg0)) {
                            already_in_table = true;
                            break;
                        }
                    }
                    if (!already_in_table) {
                        strncpy(symbols[s].name, arg0, 7);
                        symbols[s].type = 'U';
                        symbols[s].address = 0;
                        s++;
                    }
                }
            }
        } else
            t++;
        address++;
    }

    fprintf(outFilePtr, "%d %d %d %d\n", t, d, s, r);

    rewind(inFilePtr);
    address = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        int opcode_instr, regA_instr, regB_instr, destReg_instr,
            offsetField_instr;
        destReg_instr = regA_instr = regB_instr = offsetField_instr = 0;
        if (!strcmp(opcode, "add")) {
            check3RegisterErrors(arg0, arg1, arg2);
            opcode_instr = add_instr;
            regA_instr = atoi(arg0) << 19;
            regB_instr = atoi(arg1) << 16;
            destReg_instr = atoi(arg2);
        } else if (!strcmp(opcode, "nor")) {
            check3RegisterErrors(arg0, arg1, arg2);
            opcode_instr = nor_instr;
            regA_instr = atoi(arg0) << 19;
            regB_instr = atoi(arg1) << 16;
            destReg_instr = atoi(arg2);
        } else if (!strcmp(opcode, "lw")) {
            checkRegisterErrors(arg0, arg1);
            opcode_instr = lw_instr;
            regA_instr = atoi(arg0) << 19;
            regB_instr = atoi(arg1) << 16;
            if (isNumber(arg2)) {
                offsetField_instr = atoi(arg2);
            } else {
                int label_address = lookupLabel(arg2, true).address;
                offsetField_instr = label_address;
            }
        } else if (!strcmp(opcode, "sw")) {
            checkRegisterErrors(arg0, arg1);
            opcode_instr = sw_instr;
            regA_instr = atoi(arg0) << 19;
            regB_instr = atoi(arg1) << 16;
            if (isNumber(arg2)) {
                offsetField_instr = atoi(arg2);
            } else {
                int label_address = lookupLabel(arg2, true).address;
                offsetField_instr = label_address;
            }
        } else if (!strcmp(opcode, "beq")) {
            checkRegisterErrors(arg0, arg1);
            opcode_instr = beq_instr;
            regA_instr = atoi(arg0) << 19;
            regB_instr = atoi(arg1) << 16;
            if (isNumber(arg2)) {
                offsetField_instr = (atoi(arg2));
            } else {
                Label l = lookupLabel(arg2, true);
                if (!l.used) {
                    printf("beq used an undefined symbolic address\n");
                    exit(1);
                }
                int label_address = l.address;
                offsetField_instr = (label_address - address - 1);
            }
        } else if (!strcmp(opcode, "jalr")) {
            checkRegisterErrors(arg0, arg1);
            opcode_instr = jalr_instr;
            regA_instr = atoi(arg0) << 19;
            regB_instr = atoi(arg1) << 16;
        } else if (!strcmp(opcode, "halt")) {
            opcode_instr = halt_instr;
        } else if (!strcmp(opcode, "noop")) {
            opcode_instr = noop_instr;
        } else if (!strcmp(opcode, ".fill")) {
            // if we are filling with another label
            if (!isNumber(arg0)) {
                int label_address = lookupLabel(arg0, true).address;
                printHexToFile(outFilePtr, label_address);
            } else {
                printHexToFile(outFilePtr, atoi(arg0));
            }
            address++;
            continue;
        } else {
            printf("error: unrecognized opcode %s\n", opcode);
            exit(1);
        }
        if (offsetField_instr < -32768 || offsetField_instr > 32767) {
            printf("error: offsetField %d out of range\n", offsetField_instr);
            exit(1);
        }
        uint16_t offsetField_instr_u16 = (uint16_t)offsetField_instr;
        int machine_code = opcode_instr | regA_instr | regB_instr |
                           destReg_instr | offsetField_instr_u16;
        printHexToFile(outFilePtr, machine_code);
        address++;
    }

    for (int i = 0; i < MAXLINELENGTH; i++) {
        if (symbols[i].name[0] != '\0')
            fprintf(outFilePtr, "%s %c %d\n", symbols[i].name, symbols[i].type,
                    symbols[i].address);
    }

    for (int i = 0; i < MAXLINELENGTH; i++) {
        if (relocations[i].name[0] != '\0')
            fprintf(outFilePtr, "%d %s %s\n", relocations[i].address,
                    relocations[i].instruction, relocations[i].name);
    }
    return (0);
}

static Label lookupLabel(char *label, bool exitOnNotFound) {
    for (int i = 0; i < MAXLINELENGTH; ++i) {
        if (labelTable[i].used && !strcmp(labelTable[i].name, label)) {
            return labelTable[i];
        }
    }
    if (!exitOnNotFound || isupper(label[0])) {
        return (Label){false, "", 0};
    }
    printf("error: undefined label %s\n", label);
    exit(1);
}

// Returns non-zero if the line contains only whitespace.
static int lineIsBlank(char *line) {
    char whitespace[4] = {'\t', '\n', '\r', ' '};
    int nonempty_line = 0;
    for (int line_idx = 0; line_idx < strlen(line); ++line_idx) {
        int line_char_is_whitespace = 0;
        for (int whitespace_idx = 0; whitespace_idx < 4; ++whitespace_idx) {
            if (line[line_idx] == whitespace[whitespace_idx]) {
                line_char_is_whitespace = 1;
                break;
            }
        }
        if (!line_char_is_whitespace) {
            nonempty_line = 1;
            break;
        }
    }
    return !nonempty_line;
}

static inline void checkRegisterErrors(char *arg0, char *arg1) {
    if (!isNumber(arg0) || !isNumber(arg1)) {
        printf("error: register argument not a number\n");
        exit(1);
    }

    if (atoi(arg0) < 0 || atoi(arg0) > 7 || atoi(arg1) < 0 || atoi(arg1) > 7) {
        printf("error: register argument out of range\n");
        exit(1);
    }
}

static inline void check3RegisterErrors(char *arg0, char *arg1, char *arg2) {
    checkRegisterErrors(arg0, arg1);
    checkRegisterErrors(arg1, arg2);
}

// Exits 2 if file contains an empty line anywhere other than at the end of the
// file. Note calling this function rewinds inFilePtr.
static void checkForBlankLinesInCode(FILE *inFilePtr) {
    char line[MAXLINELENGTH];
    int blank_line_encountered = 0;
    int address_of_blank_line = 0;
    rewind(inFilePtr);

    for (int address = 0; fgets(line, MAXLINELENGTH, inFilePtr) != NULL;
         ++address) {
        // Check for line too long
        if (strlen(line) >= MAXLINELENGTH - 1) {
            printf("error: line too long\n");
            exit(1);
        }

        // Check for blank line.
        if (lineIsBlank(line)) {
            if (!blank_line_encountered) {
                blank_line_encountered = 1;
                address_of_blank_line = address;
            }
        } else {
            if (blank_line_encountered) {
                printf("Invalid Assembly: Empty line at address %d\n",
                       address_of_blank_line);
                exit(1);
            }
        }
    }
    rewind(inFilePtr);
}

/*
 * NOTE: The code defined below is not to be modifed as it is implemented
 * correctly.
 */

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
                 char *arg1, char *arg2) {
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
        /* reached end of file */
        return (0);
    }

    /* check for line too long */
    if (strlen(line) == MAXLINELENGTH - 1) {
        printf("error: line too long\n");
        exit(1);
    }

    // Ignore blank lines at the end of the file.
    if (lineIsBlank(line)) {
        return 0;
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
        /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr,
           "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r "
           "]%*[\t\n\r ]%[^\t\n\r ]",
           opcode, arg0, arg1, arg2);

    return (1);
}

static inline int isNumber(char *string) {
    int num;
    char c;
    return ((sscanf(string, "%d%c", &num, &c)) == 1);
}

// Prints a machine code word in the proper hex format to the file
static inline void printHexToFile(FILE *outFilePtr, int word) {
    fprintf(outFilePtr, "0x%08X\n", word);
}
