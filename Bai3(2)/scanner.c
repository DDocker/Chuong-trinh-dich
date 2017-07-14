/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"
#include "scanner.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_SPACE))
    readChar();
}

void skipComment() {
  int state = 0;
  while ((currentChar != EOF) && (state < 2)) {
    switch (charCodes[currentChar]) {
    case CHAR_TIMES:
      state = 1;
      break;
    case CHAR_RPAR:
      if (state == 1) state = 2;
      else state = 0;
      break;
    default:
      state = 0;
    }
    readChar();
  }
  if (state != 2)
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
}

Token* readIdentKeyword(void) {
  Token *token = makeToken(TK_NONE, lineNo, colNo);
  int count = 1;

  token->string[0] = (char)currentChar;
  readChar();

  while ((currentChar != EOF) &&
	 ((charCodes[currentChar] == CHAR_LETTER) || (charCodes[currentChar] == CHAR_DIGIT))) {
    if (count <= MAX_IDENT_LEN) token->string[count++] = (char)currentChar;
    readChar();
  }

  if (count > MAX_IDENT_LEN) {
    error(ERR_IDENTTOOLONG, token->lineNo, token->colNo);
    return token;
  }

  token->string[count] = '\0';
  token->tokenType = checkKeyword(token->string);

  if (token->tokenType == TK_NONE)
    token->tokenType = TK_IDENT;

  return token;
}

Token* readNumber(void) {
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
  int count = 0;

  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_DIGIT)) {
    token->string[count++] = (char)currentChar;
    readChar();
  }

  token->string[count] = '\0';
  token->value = atoi(token->string);
  return token;
}

Token* readConstChar(void) {
  Token *token = makeToken(TK_CHAR, lineNo, colNo);

  readChar();
  if (currentChar == EOF) {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }

  token->string[0] = currentChar;
  token->string[1] = '\0';

  readChar();
  if (currentChar == EOF) {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }

  if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
    readChar();
    return token;
  } else {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
}

Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF)
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_PLUS:
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_TIMES:
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SLASH:
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    return token;
  case CHAR_LT:
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return makeToken(SB_LE, ln, cn);
    } else return makeToken(SB_LT, ln, cn);
  case CHAR_GT:
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return makeToken(SB_GE, ln, cn);
    } else return makeToken(SB_GT, ln, cn);
  case CHAR_EQ:
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
  case CHAR_EXCLAIMATION:
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return makeToken(SB_NEQ, ln, cn);
    } else {
      token = makeToken(TK_NONE, ln, cn);
      error(ERR_INVALIDSYMBOL, ln, cn);
      return token;
    }
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
  case CHAR_PERIOD:
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_RPAR)) {
      readChar();
      return makeToken(SB_RSEL, ln, cn);
    } else return makeToken(SB_PERIOD, ln, cn);
  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COLON:
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return makeToken(SB_ASSIGN, ln, cn);
    } else return makeToken(SB_COLON, ln, cn);
  case CHAR_SINGLEQUOTE: return readConstChar();
  case CHAR_LPAR:
    ln = lineNo;
    cn = colNo;
    readChar();

    if (currentChar == EOF)
      return makeToken(SB_LPAR, ln, cn);

    switch (charCodes[currentChar]) {
    case CHAR_PERIOD:
      readChar();
      return makeToken(SB_LSEL, ln, cn);
    case CHAR_TIMES:
      readChar();
      skipComment();
      return getToken();
    default:
      return makeToken(SB_LPAR, ln, cn);
    }
  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    return token;
  }
}

Token* getValidToken(void) {
  Token *token = getToken();
  while (token->tokenType == TK_NONE) {
    free(token);
    token = getToken();
  }
  return token;
}


/******************************************************************/

void printToken(Token *token) {

 // printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf(" "); break;
  case TK_IDENT: printf("%s ", token->string); break;
  case TK_NUMBER: printf("%s ", token->string); break;
  case TK_CHAR: printf("\'%s\' ", token->string); break;
  case TK_EOF: printf("\n"); break;

  case KW_PROGRAM: printf("PROGRAM  "); break;
  case KW_CONST: printf("CONST  "); break;
  case KW_TYPE: printf("TYPE    "); break;
  case KW_VAR: printf("VAR  "); break;
  case KW_INTEGER: printf("INTEGER  "); break;
  case KW_CHAR: printf("CHAR    "); break;
  case KW_ARRAY: printf("ARRAY  "); break;
  case KW_OF: printf("OF    "); break;
  case KW_FUNCTION: printf("FUNCTION    "); break;
  case KW_PROCEDURE: printf("PROCEDURE  "); break;
  case KW_BEGIN: printf("BEGIN\n"); break;
  case KW_END: printf("END\n"); break;
  case KW_CALL: printf("CALL    "); break;
  case KW_IF: printf("IF    "); break;
  case KW_THEN: printf("THEN\n"); break;
  case KW_ELSE: printf("ELSE    "); break;
  case KW_WHILE: printf("WHILE  "); break;
  case KW_DO: printf("DO\n"); break;
  case KW_FOR: printf("FOR  "); break;
  case KW_TO: printf("TO    "); break;

  case SB_SEMICOLON: printf(";\n"); break;
  case SB_COLON: printf(": "); break;
  case SB_PERIOD: printf(". "); break;
  case SB_COMMA: printf(", "); break;
  case SB_ASSIGN: printf(" := "); break;
  case SB_EQ: printf(" = "); break;
  case SB_NEQ: printf(" != "); break;
  case SB_LT: printf(" < "); break;
  case SB_LE: printf(" <= "); break;
  case SB_GT: printf(" > "); break;
  case SB_GE: printf(" >= "); break;
  case SB_PLUS: printf(" + "); break;
  case SB_MINUS: printf(" - "); break;
  case SB_TIMES: printf(" * "); break;
  case SB_SLASH: printf(" / "); break;
  case SB_LPAR: printf("("); break;
  case SB_RPAR: printf(")"); break;
  case SB_LSEL: printf("(."); break;
  case SB_RSEL: printf(".)n"); break;
  }
}

