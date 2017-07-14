/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "error.h"

extern SymTab* symtab;
extern Token* currentToken;

Object* lookupObject(char *name) {
    //tim kiem object, bao gom trong va ngoai
  Scope* scope = symtab->currentScope;
  Object* obj;

  while (scope != NULL) {
    obj = findObject(scope->objList, name);
    if (obj != NULL) return obj;
    scope = scope->outer;
  }
  obj = findObject(symtab->globalObjectList, name);
  if (obj != NULL) return obj;
  return NULL;
}

void checkFreshIdent(char *name) {
    //khai bao chi 1 lan duy nhat
  if (findObject(symtab->currentScope->objList, name) != NULL)
    error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
}

Object* checkDeclaredIdent(char* name){
    //kiem tra xem ten da khai bao chua
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_IDENT,currentToken->lineNo, currentToken->colNo);
  }
  return obj;
}

Object* checkDeclaredConstant(char* name) {
    //kiem tra bien hang da duoc khai bao chua
  Object* obj = lookupObject(name);
  if (obj == NULL)
    error(ERR_UNDECLARED_CONSTANT,currentToken->lineNo, currentToken->colNo);
  if (obj->kind != OBJ_CONSTANT) // xem no co phai hang so hay khong
    error(ERR_INVALID_CONSTANT,currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object* checkDeclaredType(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL)
    error(ERR_UNDECLARED_TYPE,currentToken->lineNo, currentToken->colNo);
  if (obj->kind != OBJ_TYPE)
    error(ERR_INVALID_TYPE,currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object* checkDeclaredVariable(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL)
    error(ERR_UNDECLARED_VARIABLE,currentToken->lineNo, currentToken->colNo);
  if (obj->kind != OBJ_VARIABLE)
    error(ERR_INVALID_VARIABLE,currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object* checkDeclaredFunction(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL)
    error(ERR_UNDECLARED_FUNCTION,currentToken->lineNo, currentToken->colNo);
  if (obj->kind != OBJ_FUNCTION)
    error(ERR_INVALID_FUNCTION,currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object* checkDeclaredProcedure(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL)
    error(ERR_UNDECLARED_PROCEDURE,currentToken->lineNo, currentToken->colNo);
  if (obj->kind != OBJ_PROCEDURE)
    error(ERR_INVALID_PROCEDURE,currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object* checkDeclaredLValueIdent(char* name) {
    //ki tu dung truoc trong phep gan
  Object* obj = lookupObject(name);
  if (obj == NULL)
    error(ERR_UNDECLARED_IDENT,currentToken->lineNo, currentToken->colNo);

  switch (obj->kind) {
  case OBJ_VARIABLE:
  case OBJ_PARAMETER:
    break;
  case OBJ_FUNCTION:
    if (obj != symtab->currentScope->owner)
      error(ERR_INVALID_IDENT,currentToken->lineNo, currentToken->colNo);
    break;
  default:
    error(ERR_INVALID_IDENT,currentToken->lineNo, currentToken->colNo);
  }

  return obj;
}


void checkIntType(Type* type) {
  // TODO
  if ((type != NULL) &&(type -> typeClass == TP_INT))
      return;
  else error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);

}

void checkCharType(Type* type) {
  // TODO
  if ((type != NULL) &&(type -> typeClass == TP_CHAR))
      return;
  else error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkBasicType(Type* type) {
  // TODO
  //Kiem tra xem phai int or char hay khong
  if ((type != NULL) &&((type -> typeClass == TP_INT)||(type -> typeClass == TP_CHAR)))
      return;
  else error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkArrayType(Type* type) {
  // TODO
  if ((type != NULL) &&(type -> typeClass == TP_ARRAY))
    return;
  else error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);

}


void checkTypeEquality(Type* type1, Type* type2) {
  // TODO
  //So sanh 2 kieu voi nhau thoi , ko co gi dau
  if (compareType(type1, type2) == 1) return;
  else error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);

}


