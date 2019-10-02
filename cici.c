#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

// Exit the program with a given error message
void panic(char const *msg) {
    puts(msg);
    exit(-1);
}

/** LEXING **/

// Represents a type of token our lexer produces.
typedef enum TokenType {
    // Single character tokens
    T_LEFT_PARENS,
    T_RIGHT_PARENS,
    T_LEFT_BRACE,
    T_RIGHT_BRACE,
    T_SEMICOLON,
    // Keywords
    T_INT,
    T_MAIN,
    T_RETURN,
    // Litteral
    T_LITT_NUMBER
} TokenType;

// Represents a type of litteral value we've lexed out
typedef union Litteral {
    // This variant holds a numeric litteral
    int number;
    // This variant can be set to tell us that no litteral is there
    bool empty;
} Litteral;

// Represents a token produced in the lexing phase.
typedef struct Token {
    // Holds which type of token this is.
    TokenType type;
    // Holds information about the litteral contained in this token.
    Litteral litt;
} Token;

int main(int argc, char **argv) {
    if (argc < 2) panic("Must have a file to compile as an argument.");
    char *in_filename = argv[1];
    char *out_filename = "a.s";
    if (argc > 2) out_filename = argv[2];
    printf("Compiling %s into %s\n", in_filename, out_filename);
}
