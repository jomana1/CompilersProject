
#include "symbol_table.h"

Scope *currentScope = NULL;  // Pointer to the current (innermost) scope
extern int tempCounter;
Scope *allScopes[1000];  // Array to keep track of all scopes
int scopeCount = 0;    

void enterScope() {
    printf("Entering new scope...\n");
    Scope *newScope = (Scope *)malloc(sizeof(Scope));
    newScope->symbols = NULL;
    newScope->parent = currentScope;
    currentScope = newScope;

    allScopes[scopeCount] = newScope;
    scopeCount++;
    printf("Scope count inside enterScope: %d\n", scopeCount);
}

SymbolTableEntry *addSymbol(char *name, char *type, bool isConst,bool isTemp ) {
    SymbolTableEntry *entry = (SymbolTableEntry *)malloc(sizeof(SymbolTableEntry));
    entry->name = strdup(name);
    entry->type = strdup(type);
    entry->isConst = isConst;
    entry->isInitialized = 0;
    entry->isTemp = isTemp;
    entry->isUsed = 0;
    entry->value = 0.0;  // Initialize value to 0
    entry->next = currentScope->symbols;
    currentScope->symbols = entry;

    printf("Added symbol '%s' with type: %s isConst = %i\n", name, type, isConst);
    return entry;
}

SymbolTableEntry *lookupSymbol(char *name) {
    Scope *scope = currentScope;
    while (scope) {
        SymbolTableEntry *entry = scope->symbols;
        while (entry) {
            if (strcmp(entry->name, name) == 0) return entry;
            entry = entry->next;
        }

        scope = scope->parent;  // Go to parent scope
    }
    return NULL;  // Symbol not found
}

bool isSymbolDeclaredInCurrentScope(char *name) {
    SymbolTableEntry *entry = currentScope->symbols;
    while (entry) {
        if (strcmp(entry->name, name) == 0) return true;
        entry = entry->next;
    }
    return false;
}

int updateSymbolValue(char *name, float value) {
    
    SymbolTableEntry *entry = lookupSymbol(name);
    if (entry == NULL) {
        printf("Error: Symbol '%s' not found.\n", name);
        return 0;  // Symbol not found
    }
    // printf("/////// is init %d\n", entry->isInitialized);
    if (entry->isConst && entry->isInitialized) {
        printf("Error: Cannot update value of constant symbol '%s'.\n", name);
        return 0;  // Cannot update value of constant symbol
    }
    entry->value = value;  // Update value
    entry->isInitialized = 1;  // Mark as initialized
    printf("Updated symbol '%s' with value: %f\n", name, value);
    return 1;  // Successfully updated
}

void displayScope() {
    SymbolTableEntry *entry = currentScope->symbols;
    while (entry) {
        printf("Symbol: %s, Type: %s, Initialized: %d, Value: %f\n", 
               entry->name, entry->type, entry->isInitialized, entry->value);
        entry = entry->next;
    }
}

void writeSymbolTableOfCurrentScopeToFile()
{
    FILE *file = fopen("symbol_table.txt", "a");
    if (!file) {
        fprintf(stderr, "Error: Could not open file for writing.\n");
        return;
    }
    SymbolTableEntry *entry = currentScope->symbols;
    while (entry) {
        fprintf(file, "Symbol: %s, Type: %s, Initialized: %d, Value: %f\n", 
               entry->name, entry->type, entry->isInitialized, entry->value);
        entry = entry->next;
    }
    fclose(file);
}
// write the symbol table of all scopes to a file with the number of the scope
void writeSymbolTableOfAllScopesToFile()
{
    FILE *file = fopen("symbol_table.txt", "w");
    if (!file) {
        fprintf(stderr, "Error: Could not open file for writing.\n");
        return;
    }
    for (int i = 0; i < scopeCount; i++) {
        // fprintf(file, "Scope %d\n", i);
        SymbolTableEntry *entry = allScopes[i]->symbols;
        while (entry) {
            fprintf(file, "Scope: %d, Symbol: %s, Type: %s, Initialized: %d, Value: %f\n", 
                i,
                   entry->name, entry->type, entry->isInitialized, entry->value);
            entry = entry->next;
        }
    }
    fclose(file);
}

void exitScope() {
    Scope *oldScope = currentScope;
    // writeSymbolTableOfCurrentScopeToFile();
    currentScope = currentScope->parent;
    //free(oldScope);
    //oldScope = NULL;
}
void checkUnusedVariables() {
    FILE *file = fopen("semantic_err.txt", "a");
    if (!file) {
        fprintf(stderr, "Error: Could not open file for writing.\n");
        return;
    }
    printf("Checking for unused variables...\n");
    printf("scope count: %d\n", scopeCount);
    for (int i = 0; i < scopeCount; i++) {
        SymbolTableEntry *entry = allScopes[i]->symbols;
        while (entry) {
            if (!entry->isTemp&&!entry->isUsed) {
               fprintf(file, "Unused variable: %s in scope %d\n", entry->name, i);
                printf("Unused variable: %s in scope %d\n", entry->name, i);
            }
            entry = entry->next;
        }
    }

    fclose(file);
}