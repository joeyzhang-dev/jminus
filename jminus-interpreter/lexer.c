#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

// Uncomment this to enable debug logging
// #define DEBUG

#define MAX_TOKENS 1024

typedef struct {
    char ch;
    int line;
} UnexpectedToken;

// Max number of unexpected tokens we'll track
#define MAX_UNEXPECTED 128

static UnexpectedToken unexpected_tokens[MAX_UNEXPECTED];
static int unexpected_count = 0;


typedef struct {
  const char* keyword;
  TokenType type;
} Keyword;

Keyword keywords[] = {
    { "let", TOKEN_LET },
    { "fn", TOKEN_FN },
    { "yap", TOKEN_YAP },
    { "return", TOKEN_RETURN },
    { "if", TOKEN_IF },
    { "else", TOKEN_ELSE },      
    { "while", TOKEN_WHILE },
    { NULL, TOKEN_UNKNOWN }
  };

TokenType check_keyword(const char* str) {
  for (int i = 0; keywords[i].keyword != NULL; i++) {
    if (strcmp(str, keywords[i].keyword) == 0) {
      return keywords[i].type;
    }
  }
  return TOKEN_IDENTIFIER;
}

Token make_token(TokenType type, const char* start, int length, int line) {
  Token token;
  token.type = type;
  token.lexeme = (char*)malloc(length + 1);
  strncpy(token.lexeme, start, length);
  token.lexeme[length] = '\0';
  token.line = line;
  return token;
}

Token* tokenize(const char* src, int* token_count) {
  Token* tokens = malloc(sizeof(Token) * MAX_TOKENS);
  int count = 0;
  int line = 1;

  const char* start = src;
  const char* current = src;

#ifdef DEBUG
  printf("Lexing source code...\n");
#endif

while (*current != '\0' && *current != EOF) {
#ifdef DEBUG
    printf("  Char: '%c' (ASCII %d) at line %d\n", *current, *current, line);
#endif

    start = current;

    if (isspace(*current)) {
      if (*current == '\n') line++;
      current++;
      continue;
    }

    if (isalpha(*current) || *current == '_') {
      while (isalnum(*current) || *current == '_') current++;

      int length = current - start;
      char* temp = malloc(length + 1);
      strncpy(temp, start, length);
      temp[length] = '\0';

      TokenType type = check_keyword(temp);
      free(temp);
      
      tokens[count++] = make_token(type, start, current - start, line);
      continue;
    }

    if (isdigit(*current)) {
      while (isdigit(*current)) current++;
      tokens[count++] = make_token(TOKEN_INT, start, current - start, line);
      continue;
    }
    // Handle line comments (//...)
    if (*current == '/' && *(current + 1) == '/') {
        while (*current != '\0' && *current != '\n') current++;
        continue;
    }
    switch (*current) {
      case '+': tokens[count++] = make_token(TOKEN_PLUS, current++, 1, line); break;
      case '-': tokens[count++] = make_token(TOKEN_MINUS, current++, 1, line); break;
      case '*': tokens[count++] = make_token(TOKEN_STAR, current++, 1, line); break;
      case '/': tokens[count++] = make_token(TOKEN_SLASH, current++, 1, line); break;

      case '=':
        if (*(current + 1) == '=') {
          tokens[count++] = make_token(TOKEN_EQUAL, current, 2, line);
          current += 2;
        } else {
          tokens[count++] = make_token(TOKEN_ASSIGN, current++, 1, line);
        }
        break;

      case '!':
        if (*(current + 1) == '=') {
          tokens[count++] = make_token(TOKEN_BANG_EQUAL, current, 2, line);
          current += 2;
        } else {
          tokens[count++] = make_token(TOKEN_BANG, current++, 1, line);
        }
        break;

      case '<':
        if (*(current + 1) == '=') {
          tokens[count++] = make_token(TOKEN_LESS_EQUAL, current, 2, line);
          current += 2;
        } else {
          tokens[count++] = make_token(TOKEN_LESS, current++, 1, line);
        }
        break;

      case '>':
        if (*(current + 1) == '=') {
          tokens[count++] = make_token(TOKEN_GREATER_EQUAL, current, 2, line);
          current += 2;
        } else {
          tokens[count++] = make_token(TOKEN_GREATER, current++, 1, line);
        }
        break;

      case ';': tokens[count++] = make_token(TOKEN_SEMICOLON, current++, 1, line); break;
      case ',': tokens[count++] = make_token(TOKEN_COMMA, current++, 1, line); break;
      case '(': tokens[count++] = make_token(TOKEN_LPAREN, current++, 1, line); break;
      case ')': tokens[count++] = make_token(TOKEN_RPAREN, current++, 1, line); break;
      case '{': tokens[count++] = make_token(TOKEN_LBRACE, current++, 1, line); break;
      case '}': tokens[count++] = make_token(TOKEN_RBRACE, current++, 1, line); break;

      default:
      if (unexpected_count < MAX_UNEXPECTED) {
        unexpected_tokens[unexpected_count].ch = *current;
        unexpected_tokens[unexpected_count].line = line;
        unexpected_count++;
      }
      tokens[count++] = make_token(TOKEN_UNKNOWN, current++, 1, line);
      break;
    
    }
  }

  tokens[count++] = make_token(TOKEN_EOF, current, 0, line);
  *token_count = count;

  if (unexpected_count > 0) {
    printf("\n!!! Lexing error: unexpected characters found in source code!\n");
    for (int i = 0; i < unexpected_count; i++) {
        printf("  -> Unexpected character '%c' at line %d\n",
               unexpected_tokens[i].ch, unexpected_tokens[i].line);
    }
    printf("\nHint: Check your source file.\n\n");
    exit(1); // Force exit because source is invalid
}


  return tokens;
}

void free_tokens(Token* tokens, int count) {
  for (int i = 0; i < count; i++) {
    free(tokens[i].lexeme);
  }
  free(tokens);
}

const char* token_type_to_string(TokenType type) {
  switch (type) {
    case TOKEN_LET: return "LET";
    case TOKEN_FN: return "FN";
    case TOKEN_YAP: return "YAP";
    case TOKEN_RETURN: return "RETURN";
    case TOKEN_IF: return "IF";
    case TOKEN_WHILE: return "WHILE";
    case TOKEN_ELSE: return "ELSE";
    case TOKEN_IDENTIFIER: return "IDENTIFIER";
    case TOKEN_INT: return "INT";
    case TOKEN_FLOAT: return "FLOAT";
    case TOKEN_ASSIGN: return "ASSIGN";
    case TOKEN_PLUS: return "PLUS";
    case TOKEN_MINUS: return "MINUS";
    case TOKEN_STAR: return "STAR";
    case TOKEN_SLASH: return "SLASH";
    case TOKEN_EQUAL: return "EQUAL";
    case TOKEN_BANG: return "BANG";
    case TOKEN_BANG_EQUAL: return "BANG_EQUAL";
    case TOKEN_LESS: return "LESS";
    case TOKEN_LESS_EQUAL: return "LESS_EQUAL";
    case TOKEN_GREATER: return "GREATER";
    case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
    case TOKEN_LPAREN: return "LPAREN";
    case TOKEN_RPAREN: return "RPAREN";
    case TOKEN_LBRACE: return "LBRACE";
    case TOKEN_RBRACE: return "RBRACE";
    case TOKEN_COMMA: return "COMMA";
    case TOKEN_SEMICOLON: return "SEMICOLON";
    case TOKEN_EOF: return "EOF";
    case TOKEN_UNKNOWN: return "UNKNOWN";
    default: return "???";
  }
}

void print_token(Token token) {
  printf("[Line %d] %-15s %s\n", token.line, token_type_to_string(token.type), token.lexeme);
}
