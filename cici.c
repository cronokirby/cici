#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

// Exit the program with a given error message
void panic(char const *msg) {
    puts(msg);
    exit(-1);
}

/** CHARACTER UTILITIES **/
// The first size of string we try and allocate
#define BASE_STRING_SIZE 16
// Check whether this character is alpha
#define IS_ALPHA(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
// Check whether this character is numeric
#define IS_NUMERIC(c) ((c) >= '0' && (c) <= '9')
// Check whether this character is alphanumeric
#define IS_ALPHA_NUMERIC(c) (IS_ALPHA(c) || IS_NUMERIC(c))

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
    T_START,
    T_EOF
} TokenType;

// Represents a token produced in the lexing phase.
typedef struct Token {
    // Holds which type of token this is.
    TokenType type;
    // Holds information about the litteral contained in this token.
    int litt;
} Token;

// Print out a representation of a token to a stream
void token_print(Token t, FILE *fp) {
    switch (t.type) {
    case T_LEFT_PARENS:
        fputs("(\n", fp);
        break;
    case T_RIGHT_PARENS:
        fputs(")\n", fp);
        break;
    case T_LEFT_BRACE:
        fputs("{\n", fp);
        break;
    case T_RIGHT_BRACE:
        fputs("}\n", fp);
        break;
    case T_SEMICOLON:
        fputs(";\n", fp);
        break;
    case T_INT:
        fputs("int\n", fp);
        break;
    case T_MAIN:
        fputs("main\n", fp);
        break;
    case T_RETURN:
        fputs("return\n", fp);
        break;
    case T_LITT_NUMBER:
        fprintf(fp, "Litt(%d)\n", t.litt);
        break;
    case T_START:
        fputs("START\n", fp);
        break;
    case T_EOF:
        fputs("EOF\n", fp);
        break;
    }
}

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
    Token token = {.type = T_EOF, .litt = 0};
    // We always return unless we continue
    for (;;) {
        char next = st->program[st->index];
        if (next == 0) {
            token.type = T_EOF;
        } else if (next == '(') {
            st->index++;
            token.type = T_LEFT_PARENS;
        } else if (next == ')') {
            st->index++;
            token.type = T_RIGHT_PARENS;
        } else if (next == '{') {
            st->index++;
            token.type = T_LEFT_BRACE;
        } else if (next == '}') {
            st->index++;
            token.type = T_RIGHT_BRACE;
        } else if (next == ';') {
            st->index++;
            token.type = T_SEMICOLON;
        } else if (IS_ALPHA(next)) {
            size_t size = BASE_STRING_SIZE;
            char *buf = malloc(size);
            unsigned int index = 0;
            for (; IS_ALPHA_NUMERIC(next); next = st->program[st->index]) {
                // Leave space for the last byte
                if (index >= size - 1) {
                    size <<= 1;
                    buf = realloc(buf, size);
                }
                buf[index++] = next;
                st->index++;
            }
            buf[index] = 0;
            if (strcmp(buf, "main") == 0) {
                token.type = T_MAIN;
            } else if (strcmp(buf, "return") == 0) {
                token.type = T_RETURN;
            } else if (strcmp(buf, "int") == 0) {
                token.type = T_INT;
            } else {
                printf("Unknown keyword %s\n", buf);
                exit(-1);
            }
        } else if (IS_NUMERIC(next)) {
            int buf = 0;
            for (; IS_NUMERIC(next); next = st->program[st->index]) {
                buf = buf * 10 + next - '0';
                st->index++;
            }
            token.type = T_LITT_NUMBER;
            token.litt = buf;
        } else {
            st->index++;
            continue;
        }
        return token;
    }
}

// This enum identifies what kind of node we're dealing with in a tre
typedef enum AstKind {
    // Represents an int main function with a sequence of statements
    K_INT_MAIN,
    // Represents an expression statement e.g. `foo();`
    K_EXPR_STATEMENT,
    // Represents a declaration statement e.g. `int x = 1;`
    K_DECLARATION,
    // Represents a return statement e.g. `return 1;`
    K_RETURN,
    // Represents a declaration with initialization
    K_INIT_DECLARATION,
    // Represents a declaration without initialization
    K_NO_INIT_DECLARATION,
    // Represents a top level expression, which can be seperated by commas
    K_TOP_EXPR,
    // x = y
    K_ASSIGN,
    // a + b
    K_ADD,
    // a - b
    K_SUB,
    // a * b
    K_MUL,
    // a / b
    K_DIV,
    // a % b
    K_MOD,
    // !a
    K_LOGICAL_NOT,
    // ~a
    K_BIT_NOT,
    // -a
    K_NEGATE,
    // Represents an identifier.
    K_IDENTIFIER,
    // Represents a numeric litteral
    K_NUMBER
} AstKind;

typedef struct AstNode AstNode;

// Holds multiple children for a given node.
typedef struct AstChildren {
    // The number of nodes under us
    unsigned int count;
    // An array containing how many nodes we need
    AstNode *nodes;
} AstChildren;

// Holds one of the possible
typedef union AstData {
    // A numeric litteral
    int num;
    // A string litteral or identifier
    char *string;
    // A direct next node
    AstNode *next;
    // Multiple next nodes
    AstChildren children;
} AstData;

// The root syntax node type
struct AstNode {
    // What kind of node this is
    AstKind kind;
    // The payload for this node
    AstData data;
};

typedef struct ParseState {
    // Holds the state of the lexer
    LexState lex_st;
    // The next token
    Token peek;
    // This holds the previous token we parsed
    Token prev;
    // Whether or not the head has been initialized
    bool has_peek;
} ParseState;

ParseState parse_init(LexState lex_st) {
    Token start = {.type = T_START, .litt = 0};
    ParseState st = {
        .lex_st = lex_st, .peek = start, .prev = start, .has_peek = false};
    return st;
}

Token parse_peek(ParseState *st) {
    if (!st->has_peek) {
        st->prev = st->peek;
        st->peek = lex_next(&st->lex_st);
        st->has_peek = true;
    }
    return st->peek;
}

bool parse_at_end(ParseState *st) { return parse_peek(st).type == T_EOF; }

void parse_advance(ParseState *st) { st->has_peek = false; }

bool parse_check(ParseState *st, TokenType type) {
    if (parse_at_end(st)) {
        return false;
    }
    return parse_peek(st).type == type;
}

bool parse_match(ParseState *st, TokenType *types, unsigned int type_count) {
    for (unsigned int i = 0; i < type_count; ++i) {
        if (parse_check(st, types[i])) {
            parse_advance(st);
            return true;
        }
    }
    return false;
}

void parse_consume(ParseState *st, TokenType type, const char *msg) {
    if (parse_check(st, type)) {
        parse_advance(st);
        return;
    }
    printf("Error at index %ld:\n", st->lex_st.index);
    panic(msg);
}

AstNode *parse_int_main(ParseState *st) {
    parse_consume(st, T_INT, "Expected int token at start of program");
    parse_consume(st, T_MAIN, "Expected `main` identifier");
    parse_consume(st, T_LEFT_PARENS, "Expected `(` after function name");
    parse_consume(st, T_RIGHT_PARENS, "Expected `)` after `(`");
    parse_consume(st, T_LEFT_BRACE, "Expected `{` before function statements");
    return NULL;
}

typedef enum CompileStage { STAGE_LEX, STAGE_PARSE } CompileStage;

int main(int argc, char **argv) {
    if (argc < 2) {
        panic("Must have a file to compile as an argument.");
    }
    char *in_filename = argv[1];
    char *out_filename = "a.s";
    if (argc > 2) {
        out_filename = argv[2];
    }
    CompileStage stage = STAGE_PARSE;
    if (argc > 3) {
        char *stage_str = argv[3];
        if (strcmp(stage_str, "lex") == 0) {
            stage = STAGE_LEX;
        } else if (strcmp(stage_str, "parse") == 0) {
            stage = STAGE_PARSE;
        }
    }
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
    FILE *out;
    if (strcmp(out_filename, "stdout") == 0) {
        out = stdout;
    } else {
        out = fopen(out_filename, "w");
        if (out == NULL) {
            panic("Failed to open output file");
        }
    }
    LexState lexer = lex_init(in_data);
    if (stage == STAGE_LEX) {
        for (Token t = lex_next(&lexer); t.type != T_EOF;
             t = lex_next(&lexer)) {
            token_print(t, out);
        }
        return 0;
    }
    ParseState parser = parse_init(lexer);
    AstNode *root = parse_int_main(&parser);
    return 0;
}
