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
    T_LITT_NUMBER,
    // Special
    T_EOF
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

typedef struct LexState {
    // The underlying program (we don't own)
    char const *program;
    // The index we're currently at in the program
    long index;
} LexState;

LexState lex_init(char const *program) {
    LexState ret = {.program = program, .index = 0};
    return ret;
}

Token lex_next(LexState *st) {
    Token token;
    token.litt.empty = true;
    switch (st->program[st->index]) {
    case '(':
        st->index++;
        token.type = T_LEFT_PARENS;
        break;
    case ')':
        st->index++;
        token.type = T_RIGHT_PARENS;
        break;
    case '{':
        st->index++;
        token.type = T_LEFT_BRACE;
        break;
    case '}':
        st->index++;
        token.type = T_RIGHT_BRACE;
        break;
    default:
        token.type = T_EOF;
        break;
    }
    return token;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        panic("Must have a file to compile as an argument.");
    }
    char *in_filename = argv[1];
    char *out_filename = "a.s";
    if (argc > 2)
        out_filename = argv[2];
    FILE *in = fopen(in_filename, "r");
    if (in == NULL) {
        panic("Failed to open the input file.");
    }
    size_t length;
    if (fseek(in, 0, SEEK_END)) {
        panic("Failed to seek to the end of the input file");
    }
    length = ftell(in);
    if (fseek(in, 0, SEEK_SET)) {
        panic("Failed to rewind input file");
    }
    char *in_data = malloc(length);
    if (in_data == NULL) {
        panic("Failed to allocate input buffer.");
    }
    if (!fread(in_data, sizeof(char), length, in)) {
        panic("Failed to read into input buffer.");
    }
    fclose(in);
    printf("%s\n", in_data);
}
