/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_SPACE)) //gap ki tu ket thuc file hoac khoang trang
    readChar(); // tiep tuc doc
}

void skipComment() {
  int state = 0; //trang thai ban dau bang 0
  while ((currentChar != EOF) && (state < 2)) {
    switch (charCodes[currentChar])
    {
    case CHAR_TIMES : //khi gap ki tu *
      state = 1; // chuyen sang trang thai 1
      break;
    case CHAR_RPAR: // khi gap ki tu dong ngoac )
      if (state == 1) state = 2; // neu dang o trang thai 1 thi chuyen sang trang thai 2
      else state = 0; // neu khong chuyen ve trang thai 0
      break;
    default:
      state = 0;
    }
    readChar(); // tiep tuc doc tiep
  }
  if (state != 2) //neu doc het file ma van khong chuyen ve trang thai 2 thi bao loi ERR_ENDOFCOMMENT
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
}

Token* readIdentKeyword(void) {
  Token *token = makeToken(TK_NONE, lineNo, colNo);//tao token moi
  int count;
  count = 1;

  token->string[0] = toupper((char)currentChar); //chuyen ve viet hoa
  readChar(); //doc tiep

  while ((currentChar != EOF) && ((charCodes[currentChar] == CHAR_LETTER) )) { // gap ki tu ket thuc file hoac chu cai
    if (count <= MAX_IDENT_LEN) token->string[count++] = toupper((char)currentChar);
    readChar();
  }

  if (count > MAX_IDENT_LEN) {
    error(ERR_IDENTTOOLONG, token->lineNo, token->colNo); // neu ki tu qua dai, tra ve loi ERR_IDENTTOOLONG
    return token;
  }

  token->string[count] = '\0';
  token->tokenType = checkKeyword(token->string); //khi gap ki tu ket thuc thi thuc hien kiem tra xem no phai keyword hay khong

  if (token->tokenType == TK_NONE)
    token->tokenType = TK_IDENT;

  return token;
}

Token* readNumber(void) {
  Token *token = makeToken(TK_NUMBER, lineNo, colNo); //tao token moi
  int count = 0;

  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_DIGIT)) { //gap ki tu ket thuc file hoac gap chu so
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
    token = makeToken(SB_PLUS, lineNo, colNo); // phat hien ki tu cong
    readChar();
    return token;
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo); // phat hien ki tu dau tru
    readChar();
    return token;
  case CHAR_TIMES: //phat hien ki tu *
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SLASH: //phat hien ki tu /
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    return token;
  case CHAR_LT: //phat hien ki tu <
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) { //neu chua ket thuc file va gap them dau bang thi chuyen no thanh dau <=
      readChar();
      return makeToken(SB_LE, ln, cn);
    } else return makeToken(SB_LT, ln, cn); //neu khong thi van giu dau >
  case CHAR_GT:
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) { //neu chua ket thuc file va gap them dau bang thi chuyen no thanh dau >=
      readChar();
      return makeToken(SB_GE, ln, cn);
    } else return makeToken(SB_GT, ln, cn);//neu khong thi van giu dau >
  case CHAR_EQ: //phat hien dau =
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
  case CHAR_EXCLAIMATION : //phat hien dau !
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {//neu chua ket thuc file va gap them dau bang thi chuyen no thanh dau khac
      readChar();
      return makeToken(SB_NEQ, ln, cn);
    } else {
      token = makeToken(TK_NONE, ln, cn);
      error(ERR_INVALIDSYMBOL, ln, cn);
      return token;
    }
  case CHAR_COMMA: //phat hien dau phay
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
  case CHAR_PERIOD: // phat hien dau cham
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_RPAR)) { //neu chua ket thuc file va gap them dau bang thi chuyen no thanh dau ket thuc cua chi so mang
      readChar();
      return makeToken(SB_RSEL, ln, cn);
    } else return makeToken(SB_PERIOD, ln, cn);
  case CHAR_SEMICOLON: //phat hien dau ;
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COLON: // phat hien dau hai cham
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return makeToken(SB_ASSIGN, ln, cn);
    } else return makeToken(SB_COLON, ln, cn);
  case CHAR_SINGLEQUOTE: return readConstChar(); //phat hien dau gach cheo
  case CHAR_LPAR: //phat hien dau mo ngoac
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
  case CHAR_RPAR: //phat hien dau dong ngoac
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;
    // ....

  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}
