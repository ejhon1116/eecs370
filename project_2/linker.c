/**
 * Project 2
 * LC-2K Linker
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSIZE 500
#define MAXLINELENGTH 1000
#define MAXFILES 6

static inline void printHexToFile(FILE *, int);

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry {
    char label[7];
    char location;
    unsigned int offset;
};

static const SymbolTableEntry UNKNOWN_SYMBOL = (SymbolTableEntry){"?", '?', 0};

static SymbolTableEntry
getGlobalSymbol(char *label, SymbolTableEntry *symbolTable, int size);

struct RelocationTableEntry {
    unsigned int file;
    unsigned int offset;
    char inst[6];
    char label[7];
};

struct FileData {
    unsigned int textSize;
    unsigned int dataSize;
    unsigned int symbolTableSize;
    unsigned int relocationTableSize;
    unsigned int textStartingLine; // in final executable
    unsigned int dataStartingLine; // in final executable
    int text[MAXSIZE];
    int data[MAXSIZE];
    SymbolTableEntry symbolTable[MAXSIZE];
    RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles {
    unsigned int textSize;
    unsigned int dataSize;
    unsigned int symbolTableSize;
    unsigned int relocationTableSize;
    int text[MAXSIZE * MAXFILES];
    int data[MAXSIZE * MAXFILES];
    SymbolTableEntry symbolTable[MAXSIZE * MAXFILES];
    RelocationTableEntry relocTable[MAXSIZE * MAXFILES];
};

int main(int argc, char *argv[]) {
    char *inFileStr, *outFileStr;
    FILE *inFilePtr, *outFilePtr;
    unsigned int i, j, k;

    if (argc <= 2 || argc > 8) {
        printf("error: usage: %s <MAIN-object-file> ... <object-file> ... "
               "<output-exe-file>, with at most 5 object files\n",
               argv[0]);
        exit(1);
    }

    outFileStr = argv[argc - 1];

    outFilePtr = fopen(outFileStr, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileStr);
        exit(1);
    }

    FileData files[MAXFILES];

    // read in all files and combine into a "master" file
    for (i = 0; i < argc - 2; ++i) {
        inFileStr = argv[i + 1];

        inFilePtr = fopen(inFileStr, "r");
        printf("opening %s\n", inFileStr);

        if (inFilePtr == NULL) {
            printf("error in opening %s\n", inFileStr);
            exit(1);
        }

        char line[MAXLINELENGTH];
        unsigned int textSize, dataSize, symbolTableSize, relocationTableSize;

        // parse first line of file
        fgets(line, MAXSIZE, inFilePtr);
        sscanf(line, "%d %d %d %d", &textSize, &dataSize, &symbolTableSize,
               &relocationTableSize);

        files[i].textSize = textSize;
        files[i].dataSize = dataSize;
        files[i].symbolTableSize = symbolTableSize;
        files[i].relocationTableSize = relocationTableSize;

        // read in text section
        int instr;
        for (j = 0; j < textSize; ++j) {
            fgets(line, MAXLINELENGTH, inFilePtr);
            instr = strtol(line, NULL, 0);
            files[i].text[j] = instr;
        }

        // read in data section
        int data;
        for (j = 0; j < dataSize; ++j) {
            fgets(line, MAXLINELENGTH, inFilePtr);
            data = strtol(line, NULL, 0);
            files[i].data[j] = data;
        }

        // read in the symbol table
        char label[7];
        char type;
        unsigned int addr;
        for (j = 0; j < symbolTableSize; ++j) {
            fgets(line, MAXLINELENGTH, inFilePtr);
            sscanf(line, "%s %c %d", label, &type, &addr);
            files[i].symbolTable[j].offset = addr;
            strcpy(files[i].symbolTable[j].label, label);
            files[i].symbolTable[j].location = type;
        }

        // read in relocation table
        char opcode[7];
        for (j = 0; j < relocationTableSize; ++j) {
            fgets(line, MAXLINELENGTH, inFilePtr);
            sscanf(line, "%d %s %s", &addr, opcode, label);
            files[i].relocTable[j].offset = addr;
            strcpy(files[i].relocTable[j].inst, opcode);
            strcpy(files[i].relocTable[j].label, label);
            files[i].relocTable[j].file = i;
        }
        fclose(inFilePtr);
    } // end reading files

    // *** INSERT YOUR CODE BELOW ***
    //    Begin the linking process
    //    Happy coding!!!
    CombinedFiles cf = {0, 0, 0, 0};
    for (i = 0; i < argc - 2; i++) {
        FileData file = files[i];
        for (j = 0; j < file.textSize; j++) {
            cf.text[j + cf.textSize] = file.text[j];
        }

        for (j = 0; j < file.dataSize; j++) {
            cf.data[j + cf.dataSize] = file.data[j];
        }

        for (j = 0, k = cf.symbolTableSize; j < file.symbolTableSize; j++) {
            if (file.symbolTable[j].location != 'U') {
                SymbolTableEntry globalSymbol = file.symbolTable[j];

                if (!strcmp("Stack", globalSymbol.label)) {
                    printf("Reserved Stack label was defined\n");
                    exit(1);
                }
                
                SymbolTableEntry temp = getGlobalSymbol(globalSymbol.label, cf.symbolTable, cf.symbolTableSize);
                if(temp.location != '?') exit(1);

                // Set the true address of the symbol(relative to the beginning
                // of the text/data section)
                if (globalSymbol.location == 'T')
                    globalSymbol.offset += cf.textSize;
                else if (globalSymbol.location == 'D')
                    globalSymbol.offset += cf.dataSize;

                cf.symbolTable[k++] = globalSymbol;
                cf.symbolTableSize++;
            }
        }
        cf.textSize += file.textSize;
        cf.dataSize += file.dataSize;
    }

    int textPos = 0, dataPos = 0;
    for (i = 0; i < argc - 2; i++) {
        FileData file = files[i];
        int newAddress = -1;
        for (j = 0; j < file.relocationTableSize; j++) {
            RelocationTableEntry relocEntry = file.relocTable[j];

            // If the relocation entry points to a global symbol
            if (isupper(relocEntry.label[0])) {
                if (!strcmp("Stack", relocEntry.label)) {
                    newAddress = cf.textSize + cf.dataSize;
                } else {
                    SymbolTableEntry globalSymbol = getGlobalSymbol(
                        relocEntry.label, cf.symbolTable, cf.symbolTableSize);
                    if (globalSymbol.location != '?') {
                        newAddress = globalSymbol.location == 'T' ? globalSymbol.offset : cf.textSize + globalSymbol.offset;
                    } else { // Global symbol could not be found
                        printf("Error: global symbol %s could not be found\n",
                               relocEntry.label);
                        exit(1);
                    }
                }
            } else {
                int oldAddress = -1;
                bool oldAddressInText = false;
                if (!strcmp(".fill", relocEntry.inst))
                    oldAddress = file.data[relocEntry.offset];
                else
                    oldAddress = file.text[relocEntry.offset] & 0xFFFF;

                if (oldAddress < file.textSize)
                    oldAddressInText = true;

                if (oldAddressInText)
                    newAddress = textPos + oldAddress;
                else
                    newAddress = cf.textSize + dataPos + oldAddress - file.textSize;
            }

            // Use the new runtime address we have calculated
            if (!strcmp(".fill", relocEntry.inst)) {
                cf.data[dataPos + relocEntry.offset] = newAddress;
            } else {
                cf.text[textPos + relocEntry.offset] =
                    cf.text[textPos + relocEntry.offset] & 0xFFFF0000;
                cf.text[textPos + relocEntry.offset] += newAddress;
            }
        }
        textPos += file.textSize;
        dataPos += file.dataSize;
    }

    for (i = 0; i < cf.textSize; i++) {
        printHexToFile(outFilePtr, cf.text[i]);
    }
    for (i = 0; i < cf.dataSize; i++) {
        printHexToFile(outFilePtr, cf.data[i]);
    }

    /* here is an example of using printHexToFile. This will print a
       machine code word / number in the proper hex format to the output file */
    // printHexToFile(outFilePtr, 123);

} // main

// Prints a machine code word in the proper hex format to the file
static inline void printHexToFile(FILE *outFilePtr, int word) {
    fprintf(outFilePtr, "0x%08X\n", word);
}

static SymbolTableEntry
getGlobalSymbol(char *label, SymbolTableEntry *symbolTable, int size) {
    for (int i = 0; i < size; i++) {
        if (!strcmp(label, symbolTable[i].label)) {
            return symbolTable[i];
        }
    }
    return UNKNOWN_SYMBOL;
}
