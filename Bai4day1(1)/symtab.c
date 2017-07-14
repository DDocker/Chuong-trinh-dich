/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object* obj);
void freeScope(Scope* scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab* symtab;
Type* intType;
Type* charType;

/******************* Type utilities ******************************/

Type* makeIntType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_INT;
  return type;
}

Type* makeCharType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_CHAR;
  return type;
}

Type* makeArrayType(int arraySize, Type* elementType) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_ARRAY;
  type->arraySize = arraySize;
  type->elementType = elementType;
  return type;
}

Type* duplicateType(Type* type) {
  // TODO
  Type * newType = (Type*) malloc(sizeof(Type));
    newType->typeClass = type->typeClass;
    newType->arraySize = type->arraySize;
    newType->elementType = type->elementType;

  return newType;
}

int compareType(Type* type1, Type* type2) {
  // TODO
  if (type1->typeClass == type2->typeClass) {
        if (type1->typeClass == TP_ARRAY) {
            if (!(type1->arraySize == type2->arraySize
                    && compareType(type1->elementType, type2->elementType)))
                return 0;
        }
        return 1;
    }

  return 0;
}

void freeType(Type* type) {
  // TODO
  if (type->elementType != NULL)
      freeType(type->elementType);
    free(type);
  type = NULL;
}

/******************* Constant utility ******************************/

ConstantValue* makeIntConstant(int i) {
  // TODO
  ConstantValue * constValue = (ConstantValue*) malloc(sizeof(ConstantValue));
    constValue->type = TP_INT;
    constValue->intValue = i;

  return constValue;
}

ConstantValue* makeCharConstant(char ch) {
  // TODO
  ConstantValue * constValue = (ConstantValue*) malloc(sizeof(ConstantValue));
    constValue->type = TP_CHAR;
    constValue->charValue = ch;

  return constValue;
}

ConstantValue* duplicateConstantValue(ConstantValue* v) {
  // TODO
  ConstantValue * constValue = (ConstantValue*) malloc(sizeof(ConstantValue));
    constValue->type = v->type;
    switch (v -> type){
        case TP_INT : constValue->intValue = v->intValue;
            break;
        case TP_CHAR : constValue->charValue = v->charValue;
            break;
        default : break;
    }
    return constValue;
}

/******************* Object utilities ******************************/

Scope* createScope(Object* owner, Scope* outer) {
  Scope* scope = (Scope*) malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

Object* createProgramObject(char *programName) {
  Object* program = (Object*) malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes*) malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = createScope(program,NULL);
  symtab->program = program;

  return program;
}

Object* createConstantObject(char *name) {
  // TODO
   Object* constant = (Object *) malloc(sizeof(Object));
    strcpy(constant->name, name);
    constant->kind = OBJ_CONSTANT;
    constant->constAttrs = (ConstantAttributes *) malloc(sizeof(ConstantAttributes));
    constant->constAttrs->value = NULL;

  return constant;
}

Object* createTypeObject(char *name) {
  // TODO
  Object* type = (Object*) malloc(sizeof(Object));
    strcpy(type->name, name);
    type->kind = OBJ_TYPE;
    type->typeAttrs = (TypeAttributes*) malloc(sizeof(TypeAttributes));
    type->typeAttrs->actualType = NULL;

  return type;
}

Object* createVariableObject(char *name) {
  // TODO
  Object* variable = (Object*) malloc(sizeof(Object));
    strcpy(variable->name, name);
    variable->kind = OBJ_VARIABLE;
    variable->varAttrs = (VariableAttributes*) malloc(sizeof(VariableAttributes));
    // FIXME
    variable->varAttrs->type = NULL;
    variable->varAttrs->scope = symtab->currentScope;

  return variable;
}

Object* createFunctionObject(char *name) {
  // TODO
  Object* function = (Object*) malloc(sizeof(Object));
    strcpy(function->name, name);
    function->kind = OBJ_FUNCTION;

    function->funcAttrs = (FunctionAttributes*) malloc(sizeof(FunctionAttributes));
    function->funcAttrs->paramList = NULL;
    function->funcAttrs->returnType = NULL;
    function->funcAttrs->scope = createScope(function, symtab->currentScope);

  return function;
}

Object* createProcedureObject(char *name) {
  // TODO
  Object* procedure = (Object*) malloc(sizeof(Object));
    strcpy(procedure->name, name);
    procedure->kind = OBJ_PROCEDURE;

    procedure->procAttrs = (ProcedureAttributes*) malloc(sizeof(ProcedureAttributes));
    procedure->procAttrs->paramList = NULL;
    procedure->procAttrs->scope = createScope(procedure, symtab->currentScope);

  return procedure;
}

Object* createParameterObject(char *name, enum ParamKind kind, Object* owner) {
  // TODO
  Object* param = (Object*) malloc(sizeof(Object));
    strcpy(param->name, name);
    param->kind = OBJ_PARAMETER;

    param->paramAttrs = (ParameterAttributes*) malloc(sizeof(ParameterAttributes));
    param->paramAttrs->function = owner;
    param->paramAttrs->kind = kind;
    param->paramAttrs->type = NULL; // FIXME

return param;
}

void freeObject(Object* obj) {
  // TODO
  if (obj != NULL) {
        if (obj->constAttrs != NULL) {
            switch (obj->kind) {
            case OBJ_CONSTANT:
                if (obj->constAttrs->value != NULL) {
                    free(obj->constAttrs->value);
                    obj->constAttrs->value = NULL;
                }
                break;
            case OBJ_VARIABLE:
                if (obj->varAttrs->type != NULL) {
                    free(obj->varAttrs->type);
                    obj->varAttrs->type = NULL;
                }
                break;
            case OBJ_TYPE:
                if (obj->typeAttrs->actualType != NULL) {
                    free(obj->typeAttrs->actualType);
                    obj->typeAttrs->actualType = NULL;
                }
                break;
            case OBJ_PROGRAM:
                if (obj->progAttrs->scope != NULL) {
                    freeScope(obj->progAttrs->scope);
                    obj->progAttrs->scope = NULL;
                }
                break;
            case OBJ_FUNCTION:
                freeScope(obj->funcAttrs->scope); // Free the param list
                break;
            case OBJ_PROCEDURE:
                freeScope(obj->procAttrs->scope); // Free the param list
                break;
            case OBJ_PARAMETER:
                if (obj->paramAttrs->type != NULL) {
                    free(obj->paramAttrs->type);
                    obj->paramAttrs->type = NULL;
                }
                break;
            default:
                break;
            }

            free(obj->constAttrs);
            obj->constAttrs = NULL;
        }
        free(obj);
        obj = NULL;
  }
}

void freeScope(Scope* scope) {
  // TODO
  if (scope != NULL) {
        freeObjectList(scope->objList);
        free(scope);
        scope = NULL;
  }
}

void freeObjectList(ObjectNode *objList) {
  // TODO
  if (objList != NULL) {
        freeObject(objList->object);
        freeObjectList(objList->next);
        objList = NULL;
  }
}

void freeReferenceList(ObjectNode *objList) {
  // TODO
  if (objList != NULL) {
        freeObject(objList->object);
        freeReferenceList(objList->next);
        objList = NULL;
  }
}

void addObject(ObjectNode **objList, Object* obj) {
  ObjectNode* node = (ObjectNode*) malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  if ((*objList) == NULL)
    *objList = node;
  else {
    ObjectNode *n = *objList;
    while (n->next != NULL)
      n = n->next;
    n->next = node;
  }
}

Object* findObject(ObjectNode *objList, char *name) {
  // TODO
  ObjectNode * currentNode = objList;
  while(currentNode != NULL) {
      if (strcmp(currentNode->object->name, name) == 0) {
           return currentNode->object;
      }
      currentNode = currentNode->next;
  }

return NULL;
}

/******************* others ******************************/

void initSymTab(void) {
  Object* obj;
  Object* param;

  symtab = (SymTab*) malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;

  obj = createFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEI");
  param = createParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEC");
  param = createParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITELN");
  addObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void) {
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope* scope) {
  symtab->currentScope = scope;
}

void exitBlock(void) {
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object* obj) {
  if (obj->kind == OBJ_PARAMETER) {
    Object* owner = symtab->currentScope->owner;
    switch (owner->kind) {
    case OBJ_FUNCTION:
      addObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      addObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }

  addObject(&(symtab->currentScope->objList), obj);
}


