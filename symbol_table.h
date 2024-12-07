#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enum to represent data types
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR
} DataType;

// Struct to represent a symbol table entry
typedef struct {
    char name[100];
    DataType type;
    union {
        int i_val;
        float f_val;
        char c_val;
    } value;
} SymbolTableEntry;

#define MAX_VARS 100
extern SymbolTableEntry symbol_table[MAX_VARS]; // array to store symbol table entries
extern int var_count; // to keep track of how many variables we've added

// Function to get the value of a variable
int get_var_value(char *var_name);

// Function to assign a value to a variable
void assign_var(char *var_name, int value, DataType type);

#endif // SYMBOL_TABLE_H