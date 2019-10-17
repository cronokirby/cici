#include "assert.h"
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
    T_COMMA,
    T_EQUALS,
    T_PLUS,
    T_MINUS,
    T_SLASH,
    T_ASTERISK,
    T_PERCENT,
    T_EXCLAMATION,
    T_TILDE,
    T_AMPERSAND,
    T_VERT_BAR,
    T_CARET,
    // Double character tokens
    T_EQUALS_EQUALS,
    T_EXCLAMATION_EQUALS,
    // Keywords
    T_INT,
    T_RETURN,
    T_IF,
    T_ELSE,
    T_WHILE,
    T_BREAK,
    T_CONTINUE,
    // Litteral
    T_LITT_NUMBER,
    T_IDENTIFIER,
    // Special
    T_START,
    T_EOF
} TokenType;

// Represents the type of data that can be contained in a token
typedef union TokenData {
    // A numeric litteral
    int litt;
    // String data for an identifier, or a string litteral
    char *string;
} TokenData;

// Represents a token produced in the lexing phase.
typedef struct Token {
    // Holds which type of token this is.
    TokenType type;
    // Holds information about the data contained in this token
    TokenData data;
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
    case T_COMMA:
        fputs(",\n", fp);
        break;
    case T_EQUALS:
        fputs("=\n", fp);
        break;
    case T_PLUS:
        fputs("+\n", fp);
        break;
    case T_MINUS:
        fputs("-\n", fp);
        break;
    case T_SLASH:
        fputs("/\n", fp);
        break;
    case T_ASTERISK:
        fputs("*\n", fp);
        break;
    case T_PERCENT:
        fputs("%\n", fp);
        break;
    case T_EXCLAMATION:
        fputs("!\n", fp);
        break;
    case T_TILDE:
        fputs("~\n", fp);
        break;
    case T_VERT_BAR:
        fputs("|\n", fp);
        break;
    case T_CARET:
        fputs("^\n", fp);
        break;
    case T_AMPERSAND:
        fputs("&\n", fp);
        break;
    case T_EQUALS_EQUALS:
        fputs("==\n", fp);
        break;
    case T_EXCLAMATION_EQUALS:
        fputs("!=\n", fp);
        break;
    case T_INT:
        fputs("int\n", fp);
        break;
    case T_RETURN:
        fputs("return\n", fp);
        break;
    case T_IF:
        fputs("if\n", fp);
        break;
    case T_ELSE:
        fputs("else\n", fp);
        break;
    case T_WHILE:
        fputs("while\n", fp);
        break;
    case T_BREAK:
        fputs("break\n", fp);
        break;
    case T_CONTINUE:
        fputs("continue\n", fp);
        break;
    case T_LITT_NUMBER:
        fprintf(fp, "%d\n", t.data.litt);
        break;
    case T_IDENTIFIER:
        fprintf(fp, "%s\n", t.data.string);
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
    Token token = {.type = T_EOF, .data = {.litt = 0}};
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
        } else if (next == ',') {
            st->index++;
            token.type = T_COMMA;
        } else if (next == '=') {
            st->index++;
            next = st->program[st->index];
            if (next == '=') {
                st->index++;
                token.type = T_EQUALS_EQUALS;
            } else {
                token.type = T_EQUALS;
            }
        } else if (next == '+') {
            st->index++;
            token.type = T_PLUS;
        } else if (next == '-') {
            st->index++;
            token.type = T_MINUS;
        } else if (next == '/') {
            st->index++;
            next = st->program[st->index];
            if (next == '/') {
                while (next != '\n') {
                    next = st->program[st->index++];
                }
                continue;
            } else if (next == '*') {
                // This avoids us matching `/*/` as a complete comment
                st->index++;
                int stage = 0;
                for (;;) {
                    next = st->program[st->index++];
                    if (stage == 1 && next == '/') {
                        break;
                    } else if (next == '*') {
                        stage = 1;
                    } else {
                        stage = 0;
                    }
                }
                continue;
            } else {
                token.type = T_SLASH;
            }
        } else if (next == '*') {
            st->index++;
            token.type = T_ASTERISK;
        } else if (next == '%') {
            st->index++;
            token.type = T_PERCENT;
        } else if (next == '!') {
            st->index++;
            next = st->program[st->index];
            if (next == '=') {
                st->index++;
                token.type = T_EXCLAMATION_EQUALS;
            } else {
                token.type = T_EXCLAMATION;
            }
        } else if (next == '~') {
            st->index++;
            token.type = T_TILDE;
        } else if (next == '&') {
            st->index++;
            token.type = T_AMPERSAND;
        } else if (next == '|') {
            st->index++;
            token.type = T_VERT_BAR;
        } else if (next == '^') {
            st->index++;
            token.type = T_CARET;
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
            if (strcmp(buf, "return") == 0) {
                token.type = T_RETURN;
            } else if (strcmp(buf, "int") == 0) {
                token.type = T_INT;
            } else if (strcmp(buf, "if") == 0) {
                token.type = T_IF;
            } else if (strcmp(buf, "else") == 0) {
                token.type = T_ELSE;
            } else if (strcmp(buf, "while") == 0) {
                token.type = T_WHILE;
            } else if (strcmp(buf, "break") == 0) {
                token.type = T_BREAK;
            } else if (strcmp(buf, "continue") == 0) {
                token.type = T_CONTINUE;
            } else {
                token.type = T_IDENTIFIER;
                token.data.string = buf;
            }
        } else if (IS_NUMERIC(next)) {
            int buf = 0;
            for (; IS_NUMERIC(next); next = st->program[st->index]) {
                buf = buf * 10 + next - '0';
                st->index++;
            }
            token.type = T_LITT_NUMBER;
            token.data.litt = buf;
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
    // Holds the top level declarations
    K_TOP_LEVEL,
    // Represents a function with a body
    K_FUNCTION,
    // Represents the inputs to a function call or definition
    K_PARAMS,
    // Represents a function call
    K_CALL,
    // Represents a block containing a series of statements
    K_BLOCK,
    // Represents an expression statement e.g. `foo();`
    K_EXPR_STATEMENT,
    // Represents a declaration statement e.g. `int x = 1;`
    K_DECLARATION,
    // Represents a return statement e.g. `return 1;`
    K_RETURN,
    // Represents a break statement
    K_BREAK,
    // Represents a continue statement
    K_CONTINUE,
    // Represents an if statement e.g. `if (x) return 2;`
    K_IF,
    // Represents a while loop, e.g. `while (x) return 1;`
    K_WHILE,
    // Represents a declaration with initialization
    K_INIT_DECLARATION,
    // Represents a declaration without initialization
    K_NO_INIT_DECLARATION,
    // Represents a top level expression, which can be seperated by commas
    K_TOP_EXPR,
    // x = y
    K_ASSIGN,
    // x == y
    K_EQUALS,
    // x != y
    K_NOT_EQUALS,
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
    // a & b
    K_BIT_AND,
    // a | b
    K_BIT_OR,
    // a ^ b
    K_BIT_XOR,
    // -a
    K_NEGATE,
    // Represents an identifier.
    K_IDENTIFIER,
    // Represents a numeric litteral
    K_NUMBER
} AstKind;

typedef struct AstNode AstNode;

// Holds one of the possible
typedef union AstData {
    // A numeric litteral
    int num;
    // A string litteral or identifier
    char *string;
    // The nodes beneath us
    AstNode *children;
} AstData;

// The root syntax node type
struct AstNode {
    // What kind of node this is
    AstKind kind;
    // If relevant, how many children under us
    unsigned int count;
    // The payload for this node
    AstData data;
};

#define BASE_CHILDREN_SIZE 8

void ast_print_rec(AstNode *node, FILE *fp) {
    if (node == NULL) {
        return;
    }
    char *name;
    switch (node->kind) {
    case K_INT_MAIN:
        name = "int-main";
        break;
    case K_TOP_LEVEL:
        name = "top-level";
        break;
    case K_FUNCTION:
        name = "function";
        break;
    case K_PARAMS:
        name = "params";
        break;
    case K_CALL:
        name = "call";
        break;
    case K_BLOCK:
        name = "block";
        break;
    case K_EXPR_STATEMENT:
        name = "expr-statement";
        break;
    case K_DECLARATION:
        name = "declaration";
        break;
    case K_RETURN:
        name = "return";
        break;
    case K_BREAK:
        name = "break";
        break;
    case K_CONTINUE:
        name = "continue";
        break;
    case K_IF:
        name = "if";
        break;
    case K_WHILE:
        name = "while";
        break;
    case K_NO_INIT_DECLARATION:
        name = "declare";
        break;
    case K_TOP_EXPR:
        name = "top-expr";
        break;
    case K_LOGICAL_NOT:
        name = "!";
        break;
    case K_BIT_NOT:
        name = "~";
        break;
    case K_BIT_AND:
        name = "&";
        break;
    case K_BIT_OR:
        name = "|";
        break;
    case K_BIT_XOR:
        name = "^";
        break;
    case K_NEGATE:
        name = "-";
        break;
    case K_INIT_DECLARATION:
        name = "declare";
        break;
    case K_ASSIGN:
        name = "=";
        break;
    case K_EQUALS:
        name = "==";
        break;
    case K_NOT_EQUALS:
        name = "!=";
        break;
    case K_ADD:
        name = "+";
        break;
    case K_SUB:
        name = "-";
        break;
    case K_MUL:
        name = "*";
        break;
    case K_DIV:
        name = "/";
        break;
    case K_MOD:
        name = "%";
        break;
    // We won't use the name here
    case K_IDENTIFIER:
    case K_NUMBER:
        break;
    }
    int variant;
    switch (node->kind) {
    case K_IDENTIFIER:
        variant = 1;
        break;
    case K_NUMBER:
        variant = 0;
        break;
    default:
        variant = 2;
        break;
    }
    switch (variant) {
    case 0:
        fprintf(fp, "%d", node->data.num);
        break;
    case 1:
        fprintf(fp, "%s", node->data.string);
        break;
    case 2:
        fprintf(fp, "(%s", name);
        for (unsigned int i = 0; i < node->count; ++i) {
            fputc(' ', fp);
            ast_print_rec(node->data.children + i, fp);
        }
        fputc(')', fp);
        break;
    }
}

void ast_print(AstNode *node, FILE *fp) {
    ast_print_rec(node, fp);
    fputs("\n", fp);
}

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
    Token start = {.type = T_START, .data = {.litt = 0}};
    ParseState st = {
        .lex_st = lex_st, .peek = start, .prev = start, .has_peek = false};
    return st;
}

Token parse_peek(ParseState *st) {
    if (!st->has_peek) {
        st->peek = lex_next(&st->lex_st);
        st->has_peek = true;
    }
    return st->peek;
}

bool parse_at_end(ParseState *st) { return parse_peek(st).type == T_EOF; }

void parse_advance(ParseState *st) {
    st->has_peek = false;
    st->prev = st->peek;
}

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

void parse_assignment_expr(ParseState *st, AstNode *node);

// This should be called after accepting the first `(`
void parse_function_call_params(ParseState *st, AstNode *node) {
    node->kind = K_PARAMS;
    node->count = 0;
    unsigned int allocated = BASE_CHILDREN_SIZE;
    node->data.children = malloc(allocated * sizeof(AstNode));
    if (!parse_check(st, T_RIGHT_PARENS)) {
        node->count = 1;
        parse_assignment_expr(st, node->data.children);
    }
    while (parse_check(st, T_COMMA)) {
        parse_advance(st);
        int offset = node->count++;
        if (node->count > allocated) {
            allocated <<= 1;
            size_t size = allocated * sizeof(AstNode);
            node->data.children = realloc(node->data.children, size);
        }
        parse_assignment_expr(st, node->data.children + offset);
    }
    parse_consume(st, T_RIGHT_PARENS, "Expected `)` to end function params");
}

void parse_primary(ParseState *st, AstNode *node) {
    if (parse_check(st, T_LEFT_PARENS)) {
        parse_advance(st);
        parse_assignment_expr(st, node);
        parse_consume(st, T_RIGHT_PARENS, "Expected matching `)`");
    } else if (parse_check(st, T_LITT_NUMBER)) {
        parse_advance(st);
        node->kind = K_NUMBER;
        node->count = 0;
        node->data.num = st->prev.data.litt;
    } else if (parse_check(st, T_IDENTIFIER)) {
        parse_advance(st);
        char *name = st->prev.data.string;
        if (parse_check(st, T_LEFT_PARENS)) {
            parse_advance(st);
            node->kind = K_CALL;
            node->count = 2;
            node->data.children = malloc(2 * sizeof(AstNode));
            AstNode *id = node->data.children;
            id->kind = K_IDENTIFIER;
            id->count = 0;
            id->data.string = name;
            parse_function_call_params(st, node->data.children + 1);
        } else {
            node->kind = K_IDENTIFIER;
            node->count = 0;
            node->data.string = name;
        }
    } else {
        printf("Error at index %ld:\n", st->lex_st.index);
        puts("Unexpected Token:");
        token_print(st->peek, stdout);
        exit(-1);
    }
}

void parse_unary(ParseState *st, AstNode *node) {
    for (;;) {
        if (parse_check(st, T_EXCLAMATION)) {
            parse_advance(st);
            node->kind = K_LOGICAL_NOT;
            node->count = 1;
            node->data.children = malloc(sizeof(AstNode));
            node = node->data.children;
        } else if (parse_check(st, T_TILDE)) {
            parse_advance(st);
            node->kind = K_BIT_NOT;
            node->count = 1;
            node->data.children = malloc(sizeof(AstNode));
            node = node->data.children;
        } else if (parse_check(st, T_MINUS)) {
            parse_advance(st);
            node->kind = K_NEGATE;
            node->count = 1;
            node->data.children = malloc(sizeof(AstNode));
            node = node->data.children;
        } else {
            break;
        }
    }
    parse_primary(st, node);
}

void parse_multiply(ParseState *st, AstNode *node) {
    parse_unary(st, node);
    TokenType operators[] = {T_ASTERISK, T_SLASH, T_PERCENT};
    while (parse_match(st, operators, 3)) {
        AstNode *children = malloc(2 * sizeof(AstNode));
        children[0] = *node;
        TokenType matched = st->prev.type;
        if (matched == T_ASTERISK) {
            node->kind = K_MUL;
        } else if (matched == T_SLASH) {
            node->kind = K_DIV;
        } else {
            node->kind = K_MOD;
        }
        node->count = 2;
        parse_unary(st, children + 1);
        node->data.children = children;
    }
}

void parse_add(ParseState *st, AstNode *node) {
    parse_multiply(st, node);
    TokenType operators[] = {T_PLUS, T_MINUS};
    while (parse_match(st, operators, 2)) {
        AstNode *children = malloc(2 * sizeof(AstNode));
        children[0] = *node;
        TokenType matched = st->prev.type;
        if (matched == T_PLUS) {
            node->kind = K_ADD;
        } else {
            node->kind = K_SUB;
        }
        node->count = 2;
        parse_multiply(st, children + 1);
        node->data.children = children;
    }
}

void parse_equality(ParseState *st, AstNode *node) {
    parse_add(st, node);
    TokenType operators[] = {T_EQUALS_EQUALS, T_EXCLAMATION_EQUALS};
    while (parse_match(st, operators, 2)) {
        AstNode *children = malloc(2 * sizeof(AstNode));
        children[0] = *node;
        TokenType matched = st->prev.type;
        if (matched == T_EQUALS_EQUALS) {
            node->kind = K_EQUALS;
        } else {
            node->kind = K_NOT_EQUALS;
        }
        node->count = 2;
        parse_add(st, children + 1);
        node->data.children = children;
    }
}

void parse_and(ParseState *st, AstNode *node) {
    parse_equality(st, node);
    while (parse_check(st, T_AMPERSAND)) {
        parse_advance(st);
        AstNode *children = malloc(2 * sizeof(AstNode));
        children[0] = *node;
        node->kind = K_BIT_AND;
        node->count = 2;
        parse_equality(st, children + 1);
        node->data.children = children;
    }
}

void parse_exclusive_or(ParseState *st, AstNode *node) {
    parse_and(st, node);
    while (parse_check(st, T_CARET)) {
        parse_advance(st);
        AstNode *children = malloc(2 * sizeof(AstNode));
        children[0] = *node;
        node->kind = K_BIT_XOR;
        node->count = 2;
        parse_and(st, children + 1);
        node->data.children = children;
    }
}

void parse_inclusive_or(ParseState *st, AstNode *node) {
    parse_exclusive_or(st, node);
    while (parse_check(st, T_VERT_BAR)) {
        parse_advance(st);
        AstNode *children = malloc(2 * sizeof(AstNode));
        children[0] = *node;
        node->kind = K_BIT_OR;
        node->count = 2;
        parse_exclusive_or(st, children + 1);
        node->data.children = children;
    }
}

void parse_assignment_expr(ParseState *st, AstNode *node) {
    ParseState rewind = *st;
    if (parse_check(st, T_IDENTIFIER)) {
        parse_advance(st);
        // We need to rewind everything if the next token isn't =
        if (parse_check(st, T_EQUALS)) {
            char *identifier = st->prev.data.string;
            parse_advance(st);
            node->kind = K_ASSIGN;
            node->count = 2;
            AstNode *children = malloc(2 * sizeof(AstNode));
            node->data.children = children;
            children[0].kind = K_IDENTIFIER;
            children[0].count = 0;
            children[0].data.string = identifier;
            parse_assignment_expr(st, children + 1);
        } else {
            *st = rewind;
            parse_inclusive_or(st, node);
        }
    } else {
        parse_inclusive_or(st, node);
    }
}

AstNode *parse_top_expr(ParseState *st) {
    AstNode *node = malloc(sizeof(AstNode));
    node->kind = K_TOP_EXPR;
    node->count = 1;
    unsigned int allocated = BASE_CHILDREN_SIZE;
    node->data.children = malloc(allocated * sizeof(AstNode));
    parse_assignment_expr(st, node->data.children);
    while (parse_check(st, T_COMMA)) {
        parse_advance(st);
        int offset = node->count++;
        if (node->count > allocated) {
            allocated <<= 1;
            size_t size = allocated * sizeof(AstNode);
            node->data.children = realloc(node->data.children, size);
        }
        parse_assignment_expr(st, node->data.children + offset);
    }
    return node;
}

void parse_top_expr_opt(ParseState *st, AstNode *node) {
    if (parse_check(st, T_SEMICOLON)) {
        node->count = 0;
        node->data.children = NULL;
    } else {
        node->count = 1;
        node->data.children = parse_top_expr(st);
    }
}

AstNode *parse_declarator(ParseState *st) {
    AstNode *node = malloc(sizeof(AstNode));
    node->kind = K_IDENTIFIER;
    node->count = 0;
    int parens = 0;
    while (parse_check(st, T_LEFT_PARENS)) {
        parse_advance(st);
        ++parens;
    }
    parse_consume(st, T_IDENTIFIER, "Declarator must contain identifier");
    node->data.string = st->prev.data.string;
    for (; parens > 0; --parens) {
        parse_consume(st, T_RIGHT_PARENS,
                      "Must have matching parens around identifier");
    }
    return node;
}

void parse_declaration(ParseState *st, AstNode *node) {
    AstNode *declarator = parse_declarator(st);
    if (parse_check(st, T_EQUALS)) {
        parse_advance(st);
        node->kind = K_INIT_DECLARATION;
        node->count = 2;
        node->data.children = malloc(2 * sizeof(AstNode));
        node->data.children[0] = *declarator;
        parse_assignment_expr(st, node->data.children + 1);
    } else {
        node->kind = K_NO_INIT_DECLARATION;
        node->count = 1;
        node->data.children = declarator;
    }
}

void parse_block_or_statement(ParseState *st, AstNode *node);

void *parse_statement(ParseState *st, AstNode *node) {
    if (parse_check(st, T_RETURN)) {
        parse_advance(st);
        node->kind = K_RETURN;
        parse_top_expr_opt(st, node);
        parse_consume(st, T_SEMICOLON, "Expected semicolon to end statement");
    } else if (parse_check(st, T_BREAK)) {
        parse_advance(st);
        node->kind = K_BREAK;
        node->count = 0;
        parse_consume(st, T_SEMICOLON, "Expected semicolon to end statement");
    } else if (parse_check(st, T_CONTINUE)) {
        parse_advance(st);
        node->kind = K_CONTINUE;
        node->count = 0;
        parse_consume(st, T_SEMICOLON, "Expected semicolon to end statement");
    } else if (parse_check(st, T_INT)) {
        parse_advance(st);
        node->kind = K_DECLARATION;
        node->count = 1;
        unsigned int allocated = BASE_CHILDREN_SIZE;
        node->data.children = malloc(allocated * sizeof(AstNode));
        parse_declaration(st, node->data.children);
        while (parse_check(st, T_COMMA)) {
            parse_advance(st);
            int offset = node->count++;
            if (node->count > allocated) {
                allocated <<= 1;
                size_t size = allocated * sizeof(AstNode);
                node->data.children = realloc(node->data.children, size);
            }
            parse_declaration(st, node->data.children + offset);
        }
        parse_consume(st, T_SEMICOLON, "Expected semicolon to end statement");
    } else if (parse_check(st, T_IF)) {
        parse_advance(st);
        parse_consume(st, T_LEFT_PARENS, "Expected `(` after `if`");
        node->kind = K_IF;
        node->count = 2;
        node->data.children = malloc(2 * sizeof(AstNode));
        parse_assignment_expr(st, node->data.children);
        parse_consume(st, T_RIGHT_PARENS, "Expected `)` to close `(`");
        parse_block_or_statement(st, node->data.children + 1);
        if (parse_check(st, T_ELSE)) {
            parse_advance(st);
            node->count = 3;
            node->data.children =
                realloc(node->data.children, 3 * sizeof(AstNode));
            parse_block_or_statement(st, node->data.children + 2);
        }
    } else if (parse_check(st, T_WHILE)) {
        parse_advance(st);
        parse_consume(st, T_LEFT_PARENS, "Expected `(` after `while`");
        node->kind = K_WHILE;
        node->count = 2;
        node->data.children = malloc(2 * sizeof(AstNode));
        parse_assignment_expr(st, node->data.children);
        parse_consume(st, T_RIGHT_PARENS, "Expected `)` to close `(`");
        parse_block_or_statement(st, node->data.children + 1);
    } else {
        node->kind = K_EXPR_STATEMENT;
        parse_top_expr_opt(st, node);
        parse_consume(st, T_SEMICOLON, "Expected semicolon to end statement");
    }
    return node;
}

void parse_block(ParseState *st, AstNode *node) {
    parse_consume(st, T_LEFT_BRACE, "Expected `{` to start block");
    node->kind = K_BLOCK;
    node->count = 0;
    unsigned int allocated = BASE_CHILDREN_SIZE;
    node->data.children = malloc(allocated * sizeof(AstNode));
    while (!parse_check(st, T_RIGHT_BRACE) && !parse_at_end(st)) {
        int offset = node->count++;
        if (node->count > allocated) {
            allocated <<= 1;
            size_t size = allocated * sizeof(AstNode);
            node->data.children = realloc(node->data.children, size);
        }
        parse_block_or_statement(st, node->data.children + offset);
    }
    if (parse_at_end(st)) {
        printf("Error at index %ld:\n", st->lex_st.index);
        panic("Unexpected EOF");
    }
    parse_advance(st);
}

void parse_block_or_statement(ParseState *st, AstNode *node) {
    if (parse_check(st, T_LEFT_BRACE)) {
        parse_block(st, node);
    } else {
        parse_statement(st, node);
    }
}

void parse_param_definition(ParseState *st, AstNode *node) {
    parse_consume(st, T_INT, "Expected a param with type int");
    parse_consume(st, T_IDENTIFIER, "Expected a param to have an identifier");
    node->kind = K_IDENTIFIER;
    node->count = 0;
    node->data.string = st->prev.data.string;
}

void parse_params_def(ParseState *st, AstNode *node) {
    parse_consume(st, T_LEFT_PARENS,
                  "Expected `(` to start function param definition");
    node->kind = K_PARAMS;
    node->count = 0;
    unsigned int allocated = BASE_CHILDREN_SIZE;
    node->data.children = malloc(allocated * sizeof(AstNode));
    if (!parse_check(st, T_RIGHT_PARENS)) {
        node->count = 1;
        parse_param_definition(st, node->data.children);
    }
    while (parse_check(st, T_COMMA)) {
        parse_advance(st);
        int offset = node->count++;
        if (node->count > allocated) {
            allocated <<= 1;
            size_t size = allocated * sizeof(AstNode);
            node->data.children = realloc(node->data.children, size);
        }
        parse_param_definition(st, node->data.children + offset);
    }
    parse_consume(st, T_RIGHT_PARENS, "Expected `)` to end function params");
}

void parse_function(ParseState *st, AstNode *node) {
    node->kind = K_FUNCTION;
    node->count = 3;
    node->data.children = malloc(3 * sizeof(AstNode));
    parse_consume(st, T_IDENTIFIER, "Function definition must have identifier");
    node->data.children[0].kind = K_IDENTIFIER;
    node->data.children[0].count = 0;
    node->data.children[0].data.string = st->prev.data.string;
    parse_params_def(st, node->data.children + 1);
    parse_block(st, node->data.children + 2);
}

AstNode *parse_top_level(ParseState *st) {
    AstNode *node = malloc(sizeof(AstNode));
    node->kind = K_TOP_LEVEL;
    node->count = 0;
    unsigned int allocated = BASE_CHILDREN_SIZE;
    node->data.children = malloc(allocated * sizeof(AstNode));
    while (parse_check(st, T_INT)) {
        parse_advance(st);
        int offset = node->count++;
        if (node->count > allocated) {
            allocated <<= 1;
            size_t size = allocated * sizeof(AstNode);
            node->data.children = realloc(node->data.children, size);
        }
        parse_function(st, node->data.children + offset);
    }
    return node;
}

typedef struct Identifiers {
    // An array of strings holding our identifiers
    char **identifiers;
    // The number of slots we've filled
    unsigned int count;
    // The number of slots we have available
    unsigned int capacity;
} Identifiers;

void idents_init(Identifiers *idents) {
    idents->count = 0;
    idents->capacity = 4;
    idents->identifiers = malloc(idents->capacity * sizeof(char *));
}

void idents_insert(Identifiers *idents, char *new) {
    if (idents->count == idents->capacity) {
        idents->capacity <<= 1;
        idents->identifiers =
            realloc(idents->identifiers, idents->capacity * sizeof(char *));
    }
    idents->identifiers[idents->count++] = new;
}

// Returns < 0 for negative indices
int idents_index_of(Identifiers *idents, char *target) {
    for (unsigned int i = 0; i < idents->count; ++i) {
        if (strcmp(idents->identifiers[i], target) == 0) {
            return i;
        }
    }
    return -1;
}

typedef struct Scope {
    // The set of identifiers created inside this scope
    Identifiers identifiers;
    // The number of bytes of stack we've allocated in this scope
    int allocated_stack;
    // The initial offset for this scope
    int initial_offset;
} Scope;

typedef struct Scopes {
    // The stack of scopes
    Scope *scopes;
    // The number of slots filled
    unsigned int count;
    // The number of slots available
    unsigned int capacity;
} Scopes;

void scopes_init(Scopes *new) {
    new->count = 0;
    new->capacity = 2;
    new->scopes = malloc(new->capacity * sizeof(Scope));
}

// Enter a new scope
void scopes_enter(Scopes *scopes) {
    if (scopes->count == scopes->capacity) {
        scopes->capacity <<= 1;
        scopes->scopes =
            realloc(scopes->scopes, scopes->capacity * sizeof(Scope));
    }
    Scope *new = scopes->scopes + scopes->count++;
    // We don't have an old scope to look at
    if (scopes->count == 1) {
        // The first will take stack bytes [-4,0[
        new->initial_offset = 4;
    } else {
        Scope *old = new - 1;
        // 4 bytes for each identifier created in the previous scope
        int previous_var_size = old->identifiers.count << 2;
        new->initial_offset = old->initial_offset + previous_var_size;
    }
    idents_init(&new->identifiers);
    new->allocated_stack = 0;
}

void scopes_exit(Scopes *scopes) {
    --scopes->count;
    free(scopes->scopes[scopes->count].identifiers.identifiers);
}

// Returns < 0 if no identifier found
int scopes_offset_of(Scopes *scopes, char *identifier) {
    for (int i = scopes->count - 1; i >= 0; --i) {
        Scope *scope = scopes->scopes + i;
        int index = idents_index_of(&scope->identifiers, identifier);
        if (index >= 0) {
            return scope->initial_offset + (index << 2);
        }
    }
    return -1;
}

int scopes_total_allocated(Scopes *scopes) {
    int total = 0;
    for (unsigned int i = 0; i < scopes->count; ++i) {
        total += scopes->scopes[i].allocated_stack;
    }
    return total;
}

typedef struct AsmState {
    Scopes scopes;
    // The name of the current function
    char *function_name;
    // The current label index
    int label_index;
    // The stream we're generating to
    FILE *out;
} AsmState;

AsmState *asm_init(FILE *out) {
    AsmState *st = malloc(sizeof(AsmState));
    st->out = out;
    scopes_init(&st->scopes);
    return st;
}

void asm_enter_function(AsmState *st, char *function_name) {
    st->function_name = function_name;
    st->label_index = 0;
    scopes_enter(&st->scopes);
}

void asm_new_ident(AsmState *st, char *new) {
    if (scopes_offset_of(&st->scopes, new) >= 0) {
        Scope *current = st->scopes.scopes + st->scopes.count - 1;
        bool in_current = idents_index_of(&current->identifiers, new) >= 0;
        if (in_current) {
            puts("Error:");
            printf("Attempting to declare identifier %s twice\n", new);
            exit(-1);
        }
    }
    Scope *current = st->scopes.scopes + st->scopes.count - 1;
    idents_insert(&current->identifiers, new);
    int index = current->identifiers.count - 1;
    int offset = current->initial_offset + (index << 2);
    int total_allocated = scopes_total_allocated(&st->scopes);
    if (offset >= total_allocated) {
        current->allocated_stack += 16;
        fputs("\tsub rsp, 16\n", st->out);
    }
}

// We might not need to clear the stack if there was a return
void asm_exit_scope(AsmState *st, bool clear_stack) {
    Scope *current = st->scopes.scopes + st->scopes.count - 1;
    if (clear_stack && current->allocated_stack > 0) {
        fprintf(st->out, "\tadd\trsp, %d\n", current->allocated_stack);
    }
    scopes_exit(&st->scopes);
}

char *asm_reg_for_nth_function_param(bool is64, int n) {
    switch (n) {
    case 0:
        return is64 ? "rdi" : "edi";
    case 1:
        return is64 ? "rsi" : "esi";
    case 2:
        return is64 ? "rdx" : "edx";
    case 3:
        return is64 ? "rcx" : "ecx";
    case 4:
        return is64 ? "r8" : "r8d";
    case 5:
        return is64 ? "r9" : "r9d";
    default:
        puts("Function has more than 6 parameters");
        exit(-1);
    }
}

void asm_expr(AsmState *st, AstNode *node);

void asm_call(AsmState *st, AstNode *node) {
    assert(node->kind == K_CALL);
    AstNode *name = node->data.children;
    AstNode *params = node->data.children + 1;
    assert(name->kind == K_IDENTIFIER);
    assert(params->kind == K_PARAMS);
    for (unsigned int i = 0; i < params->count; ++i) {
        asm_expr(st, params->data.children + i);
        char *reg = asm_reg_for_nth_function_param(true, i);
        fprintf(st->out, "\tpop\t%s\n", reg);
    }
    fprintf(st->out, "\tcall\t%s\n", name->data.string);
    fputs("\tpush\trax\n", st->out);
}

void asm_expr(AsmState *st, AstNode *node) {
    switch (node->kind) {
    case K_NUMBER:
        fprintf(st->out, "\tpush\t%d\n", node->data.num);
        break;
    case K_IDENTIFIER: {
        char *ident = node->data.string;
        int offset = scopes_offset_of(&st->scopes, ident);
        if (offset < 0) {
            printf("Error:\nUse of undeclared identifier %s\n", ident);
            exit(-1);
        }
        fprintf(st->out, "\tmov\teax, DWORD PTR [rbp - %d]\n", offset);
        fputs("\tpush\trax\n", st->out);
    } break;
    case K_CALL:
        asm_call(st, node);
        break;
    case K_ASSIGN:
        asm_expr(st, node->data.children + 1);
        char *ident = node->data.children->data.string;
        int offset = scopes_offset_of(&st->scopes, ident);
        if (offset < 0) {
            printf("Error:\nAssignment to undeclared identifier %s\n", ident);
            exit(-1);
        }
        // We can just keep the top of the stack as our eventual return
        fputs("\tmov\trax, QWORD PTR [rsp]\n", st->out);
        fprintf(st->out, "\tmov\tDWORD PTR [rbp - %d], eax\n", offset);
        break;
    case K_EQUALS:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tcmp\trax, rbx\n", st->out);
        fputs("\tsete\tal\n", st->out);
        fputs("\tmovzx\teax, al\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_NOT_EQUALS:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tcmp\trax, rbx\n", st->out);
        fputs("\tsetne\tal\n", st->out);
        fputs("\tmovzx\teax, al\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_ADD:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tadd\teax, ebx\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_SUB:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tsub\teax, ebx\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_MUL:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\timul\teax, ebx\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_DIV:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tcdq\n", st->out);
        fputs("\tidiv\tebx\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_MOD:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tcdq\n", st->out);
        fputs("\tidiv\tebx\n", st->out);
        fputs("\tpush\trdx\n", st->out);
        break;
    case K_BIT_AND:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tand\teax, ebx\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_BIT_OR:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tor\teax, ebx\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_BIT_XOR:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\txor\teax, ebx\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_BIT_NOT:
        asm_expr(st, node->data.children);
        fputs("\tpop\trax\n", st->out);
        fputs("\tnot\teax\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_NEGATE:
        asm_expr(st, node->data.children);
        fputs("\tpop\trax\n", st->out);
        fputs("\tneg\teax\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_LOGICAL_NOT:
        asm_expr(st, node->data.children);
        fputs("\tpop\trax\n", st->out);
        fputs("\ttest\teax, eax\n", st->out);
        fputs("\tsete\tal\n", st->out);
        fputs("\tmovzx\teax, al\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    default:
        break;
    }
}

void asm_declare(AsmState *st, AstNode *node) {
    if (node->kind == K_NO_INIT_DECLARATION) {
        char *identifier = node->data.children[0].data.string;
        asm_new_ident(st, identifier);
    } else if (node->kind == K_INIT_DECLARATION) {
        char *identifier = node->data.children[0].data.string;
        asm_new_ident(st, identifier);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trax\n", st->out);
        int offset = scopes_offset_of(&st->scopes, identifier);
        if (offset < 0) {
            printf("Error:\nStack offset %d < 0\n", offset);
            exit(-1);
        }
        fprintf(st->out, "\tmov\tDWORD PTR [rbp - %d], eax\n", offset);
    } else {
        panic("Tried to process declaration, but kind was invalid");
    }
}

void asm_top_expr(AsmState *st, AstNode *node) {
    assert(node->kind == K_TOP_EXPR);
    for (unsigned int i = 0; i < node->count; ++i) {
        asm_expr(st, node->data.children + i);
    }
    // This will ignore all of the extra stack items we pushed
    if (node->count > 1) {
        fprintf(st->out, "\tadd\trsp, %d\n", (node->count - 1) << 3);
    }
}

// Return true if code appearing after this statement is unreachable
bool asm_statement(AsmState *st, AstNode *node, int start_label, int end_label) {
    bool after_unreachable = false;
    if (node->kind == K_RETURN) {
        asm_top_expr(st, node->data.children);
        fputs("\tpop\trax\n", st->out);
        fputs("\tmov\trsp, rbp\n", st->out);
        fputs("\tpop\trbp\n", st->out);
        fputs("\tret\n", st->out);
        after_unreachable = true;
    } else if (node->kind == K_EXPR_STATEMENT) {
        if (node->count == 1) {
            asm_top_expr(st, node->data.children);
            fputs("\tadd\trsp, 8\n", st->out);
        }
    } else if (node->kind == K_DECLARATION) {
        for (unsigned int i = 0; i < node->count; ++i) {
            asm_declare(st, node->data.children + i);
        }
    } else if (node->kind == K_IF) {
        int label = st->label_index++;
        asm_expr(st, node->data.children);
        fputs("\tpop\trax\n", st->out);
        fputs("\ttest\teax, eax\n", st->out);
        fprintf(st->out, "\tje\t.%s%d\n", st->function_name, label);
        bool if_returns = asm_statement(st, node->data.children + 1, start_label, end_label);
        fprintf(st->out, ".%s%d:\n", st->function_name, label);
        bool else_returns = false;
        if (node->count == 3) {
            else_returns = asm_statement(st, node->data.children + 2, start_label, end_label);
        }
        after_unreachable = if_returns && else_returns;
    } else if (node->kind == K_WHILE) {
        int start_label = st->label_index++;
        int end_label = st->label_index++;
        fprintf(st->out, ".%s%d:\n", st->function_name, start_label);
        asm_expr(st, node->data.children);
        fputs("\tpop\trax\n", st->out);
        fputs("\ttest\teax, eax\n", st->out);
        fprintf(st->out, "\tje\t.%s%d\n", st->function_name, end_label);
        asm_statement(st, node->data.children + 1, start_label, end_label);
        fprintf(st->out, "\tjmp\t.%s%d\n", st->function_name, start_label);
        fprintf(st->out, ".%s%d:\n", st->function_name, end_label);
    } else if (node->kind == K_BLOCK) {
        scopes_enter(&st->scopes);
        for (unsigned int i = 0; i < node->count; ++i) {
            if (asm_statement(st, node->data.children + i, start_label, end_label)) {
                asm_exit_scope(st, false);
                return true;
            }
        }
        asm_exit_scope(st, true);
    } else if (node->kind == K_BREAK) {
        fprintf(st->out, "\tjmp\t.%s%d\n", st->function_name, end_label);
    } else if (node->kind == K_CONTINUE) {
        fprintf(st->out, "\tjmp\t.%s%d\n", st->function_name, start_label);
    } else {
        panic("Unable to handle statement type");
    }
    return after_unreachable;
}

void asm_function(AsmState *st, AstNode *node) {
    assert(node->kind == K_FUNCTION);
    AstNode *name = node->data.children;
    assert(name->kind == K_IDENTIFIER);
    asm_enter_function(st, name->data.string);
    fprintf(st->out, "\t.globl %s\n", name->data.string);
    fprintf(st->out, "%s:\n", name->data.string);
    fputs("\tpush\trbp\n", st->out);
    fputs("\tmov\trbp, rsp\n", st->out);
    AstNode *params = node->data.children + 1;
    assert(params->kind == K_PARAMS);
    for (unsigned int i = 0; i < params->count; ++i) {
        assert(params->data.children[i].kind == K_IDENTIFIER);
        char *param_id = params->data.children[i].data.string;
        asm_new_ident(st, param_id);
        int offset = scopes_offset_of(&st->scopes, param_id);
        if (offset < 0) {
            printf("Error:\nStack offset %d < 0\n", offset);
            exit(-1);
        }
        char *reg = asm_reg_for_nth_function_param(false, i);
        fprintf(st->out, "\tmov\tDWORD PTR [rbp - %d], %s\n", offset, reg);
    }
    AstNode *block = node->data.children + 2;
    assert(block->kind == K_BLOCK);
    for (unsigned int i = 0; i < block->count; ++i) {
        if (asm_statement(st, block->data.children + i, -1, -1)) {
            asm_exit_scope(st, false);
            return;
        }
    }
    asm_exit_scope(st, true);
}

void asm_gen(AsmState *st, AstNode *root) {
    fputs("\t.intel_syntax noprefix\n", st->out);
    assert(root->kind == K_TOP_LEVEL);
    for (unsigned int i = 0; i < root->count; ++i) {
        asm_function(st, root->data.children + i);
    }
}

typedef enum CompileStage {
    STAGE_LEX,
    STAGE_PARSE,
    STAGE_COMPILE
} CompileStage;

int main(int argc, char **argv) {
    if (argc < 2) {
        panic("Must have a file to compile as an argument.");
    }
    char *in_filename = argv[1];
    char *out_filename = "a.s";
    if (argc > 2) {
        out_filename = argv[2];
    }
    CompileStage stage = STAGE_COMPILE;
    if (argc > 3) {
        char *stage_str = argv[3];
        if (strcmp(stage_str, "lex") == 0) {
            stage = STAGE_LEX;
        } else if (strcmp(stage_str, "parse") == 0) {
            stage = STAGE_PARSE;
        } else if (strcmp(stage_str, "compile") == 0) {
            stage = STAGE_COMPILE;
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
    AstNode *root = parse_top_level(&parser);
    if (stage == STAGE_PARSE) {
        ast_print(root, out);
        return 0;
    }
    AsmState *generator = asm_init(out);
    asm_gen(generator, root);
    return 0;
}
