/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

Token *currentToken;
Token *lookAhead;

void scan(void)
{
    Token* tmp = currentToken;
    currentToken = lookAhead;
    lookAhead = getValidToken();
    free(tmp);
}

void eat(TokenType tokenType)   // Kiem tra token, dung thi tiep tuc, sai tra ve loi missingToken
{
    if (lookAhead->tokenType == tokenType)
    {
        printToken(lookAhead);
        scan();
    }
    else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void)
{
    assert("Parsing a Program ....");
    eat(KW_PROGRAM);
    eat(TK_IDENT);
    eat(SB_SEMICOLON);
    compileBlock();
    eat(SB_PERIOD);
    assert("Program parsed!");
}

void compileBlock(void)
{
    assert("Parsing a Block ....");
    if (lookAhead->tokenType == KW_CONST)
    {
        eat(KW_CONST);
        compileConstDecl();
        compileConstDecls();
        compileBlock2();
    }
    else compileBlock2();
    assert("Block parsed!");
}

void compileBlock2(void)
{
    if (lookAhead->tokenType == KW_TYPE)
    {
        eat(KW_TYPE);
        compileTypeDecl();
        compileTypeDecls();
        compileBlock3();
    }
    else compileBlock3();
}

void compileBlock3(void)
{
    if (lookAhead->tokenType == KW_VAR)
    {
        eat(KW_VAR);
        compileVarDecl();
        compileVarDecls();
        compileBlock4();
    }
    else compileBlock4();
}

void compileBlock4(void)
{
    compileSubDecls();
    compileBlock5();
}

void compileBlock5(void)
{
    eat(KW_BEGIN);
    compileStatements();
    eat(KW_END);
}

void compileConstDecls(void)
{
    // TODO
    assert("Parsing some constants ....");
    while(lookAhead -> tokenType == TK_IDENT)
    {
        compileConstDecl();
    }
}

void compileConstDecl(void)
{
    // TODO
    assert("Parsing a constant ....");
    eat(TK_IDENT);
    eat(SB_EQ);
    compileConstant();
    eat(SB_SEMICOLON);
    assert("Constant parsed!");


}

void compileTypeDecls(void)
{
    // TODO
    assert("Passing some types ....");
    while (lookAhead -> tokenType == TK_IDENT)
    {
        compileTypeDecl();
    }


}

void compileTypeDecl(void)
{
    // TODO
    assert("Passing a type ....");
    eat(TK_IDENT);
    eat(SB_EQ);
    compileType();
    eat(SB_SEMICOLON);
    assert("Type parsed!");

}

void compileVarDecls(void)
{
    // TODO
    assert("Parsing some variables declare....");
    while (lookAhead -> tokenType == TK_IDENT)
    {
        compileVarDecl();
    }

}

void compileVarDecl(void)
{
    // TODO
    assert("Parsing a variable declare ....");
    eat(TK_IDENT);
    eat(SB_COLON);
    compileType();
    eat(SB_SEMICOLON);
    assert("Variable declare parsed!");


}

void compileSubDecls(void)   //chuyen huong, bao gom goi ham va goi thu tuc
{
    assert("Parsing subtoutines ....");
    // TODO
    if (lookAhead -> tokenType == KW_FUNCTION)  //thay tu khoa FUNCTION
    {
        compileFuncDecl();
        compileSubDecls();
    }
    else if (lookAhead -> tokenType == KW_PROCEDURE)
    {
        compileProcDecl();
        compileSubDecls();
    }
    assert("Subtoutines parsed !");
}

void compileFuncDecl(void)
{
    assert("Parsing a function ....");
    // TODO
    eat(KW_FUNCTION);
    eat(TK_IDENT);
    compileParams();
    eat(SB_COLON);
    compileBasicType();
    eat(SB_SEMICOLON);
    compileBlock();
    eat(SB_SEMICOLON);
    assert("Function parsed !");
}

void compileProcDecl(void)
{
    assert("Parsing a procedure ....");
    // TODO
    eat(KW_PROCEDURE);
    eat(TK_IDENT);
    compileParams();
    eat(SB_SEMICOLON);
    compileBlock();
    eat(SB_SEMICOLON);
    assert("Procedure parsed !");
}

void compileUnsignedConstant(void)
{
    // TODO
    assert("Parsing UnsignedConstant ....");
    if(lookAhead -> tokenType == TK_NUMBER) eat(TK_NUMBER);
    else if (lookAhead -> tokenType == TK_IDENT) eat(TK_IDENT);
    else if (lookAhead -> tokenType == TK_CHAR) eat(TK_CHAR);
    else error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
    assert("UnsignedConstant parsed !");
}

void compileConstant(void)
{
    // TODO
    assert("Parsing Constant ....");
    if(lookAhead -> tokenType == SB_PLUS)
    {
        eat(SB_PLUS);
        compileCondition2();
    }
    else if(lookAhead -> tokenType == SB_MINUS)
    {
        eat(SB_MINUS);
        compileConstant2();
    }
    else if (lookAhead -> tokenType == TK_CHAR)
    {
        eat(TK_CHAR);
    }
    else
    {
        compileConstant2();
    }
    assert("Constant parsed!");
}

void compileConstant2(void)
{
    // TODO
    if(lookAhead -> tokenType == TK_IDENT)
    {
        eat(TK_IDENT);
    }
    else if (lookAhead -> tokenType == TK_NUMBER)
    {
        eat(TK_NUMBER);
    }
    else
    {
        error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
    }

}

void compileType(void)
{
    // TODO
    assert("Parsing Type ....");
    if(lookAhead -> tokenType == KW_INTEGER)
    {
        eat(KW_INTEGER);
    }
    else if(lookAhead -> tokenType == KW_CHAR)
    {
        eat(KW_CHAR);
    }
    else if(lookAhead -> tokenType == TK_IDENT)
    {
        eat(TK_IDENT);
    }
    else if (lookAhead -> tokenType == KW_ARRAY)
    {
        eat(KW_ARRAY);
        eat(SB_LSEL);
        eat(TK_NUMBER);
        eat(SB_RSEL);
        eat(KW_OF);
        compileType();
    }
    else
    {
        error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);
    }
    assert("Type parsed!");
}
void compileBasicType(void)
{
    // TODO
    assert("Parsing BasicType ....");
    if(lookAhead -> tokenType == KW_INTEGER)
    {
        eat(KW_INTEGER);
    }
    else if(lookAhead -> tokenType == KW_CHAR)
    {
        eat(KW_CHAR);
    }
    else
    {
        error(ERR_INVALIDBASICTYPE, lookAhead->lineNo, lookAhead->colNo);
    }
    assert("Type parsed!");
}

void compileParams(void)
{
    // TODO
    assert("Parsing Params ....");
    switch(lookAhead -> tokenType)
    {
    case SB_LPAR:
        eat(SB_LPAR);
        compileParam();
        compileParams2();
        eat(SB_RPAR);
        break;
    case SB_COLON :
    case SB_SEMICOLON:
        break;
    default:
        error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
        break;
    }
    assert("Params parsed!");
}

void compileParams2(void)
{
    // TODO
    switch (lookAhead->tokenType)
    {
    case SB_SEMICOLON:
        eat(SB_SEMICOLON);
        compileParam();
        compileParams2();
        break;
    case SB_RPAR:
        break;
    default:
        error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
        break;
    }

}

void compileParam(void)
{
    // TODO
    if(lookAhead -> tokenType == TK_IDENT)
    {
        eat(TK_IDENT);
        eat(SB_COLON);
        compileBasicType();
    }
    else if (lookAhead -> tokenType == KW_VAR)
    {
        eat(KW_VAR);
        eat(TK_IDENT);
        eat(SB_COLON);
        compileBasicType();
    }
    else
    {
        error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
    }
}

void compileStatements(void)
{
    // TODO
    assert("Parsing Statements ....");
    compileStatement();
    compileStatements2();
    assert("Statements parsed! ");
}

void compileStatements2(void)
{
    // TODO
    switch (lookAhead->tokenType)
    {
    case SB_SEMICOLON:
        eat(SB_SEMICOLON);
        compileStatement();
        compileStatements2();
        break;
    case KW_END: //them follow
        break;
    default:
        eat(SB_SEMICOLON); //phat hien thieu dau ; hay khong
        error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
        break;
    }
}

void compileStatement(void)
{
    switch (lookAhead->tokenType)
    {
    case TK_IDENT:
        compileAssignSt();
        break;
    case KW_CALL:
        compileCallSt();
        break;
    case KW_BEGIN:
        compileGroupSt();
        break;
    case KW_IF:
        compileIfSt();
        break;
    case KW_WHILE:
        compileWhileSt();
        break;
    case KW_FOR:
        compileForSt();
        break;
    // EmptySt needs to check FOLLOW tokens
    case SB_SEMICOLON: //follow cua statement bao gom ;, end, else
    case KW_END:
    case KW_ELSE:
        break;
    // Error occurs
    default:
        error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
        break;
    }
}

void compileAssignSt(void)
{
    assert("Parsing an assign statement ....");
    // TODO
    eat(TK_IDENT);
    if (lookAhead->tokenType == SB_LSEL)   //o dang chi so cua mang, vi du a(.5.) := ...
    {
        compileIndexes();
    }
    eat(SB_ASSIGN);
    compileExpression();
    assert("Assign statement parsed ....");
}

void compileCallSt(void)
{
    assert("Parsing a call statement ....");
    // TODO
    eat(KW_CALL);
    eat(TK_IDENT);
    compileArguments();
    assert("Call statement parsed ....");
}

void compileGroupSt(void)
{
    assert("Parsing a group statement ....");
    // TODO
    eat(KW_BEGIN);
    compileStatements();
    eat(KW_END);
    assert("Group statement parsed ....");
}

void compileIfSt(void)
{
    assert("Parsing an if statement ....");
    eat(KW_IF);
    compileCondition();
    eat(KW_THEN);
    compileStatement();
    if (lookAhead->tokenType == KW_ELSE)
        compileElseSt();
    assert("If statement parsed ....");
}

void compileElseSt(void)
{
    eat(KW_ELSE);
    compileStatement();
}

void compileWhileSt(void)
{
    assert("Parsing a while statement ....");
    // TODO
    eat(KW_WHILE);
    compileCondition();
    eat(KW_DO);
    compileStatement();
    assert("While statement pased ....");
}

void compileForSt(void)
{
    assert("Parsing a for statement ....");
    // TODO
    eat(KW_FOR);
    eat(TK_IDENT);
    eat(SB_ASSIGN);
    compileExpression();
    eat(KW_TO);
    compileExpression();
    eat(KW_DO);
    compileStatement();
    assert("For statement parsed ....");
}

void compileArguments(void)
{
    // TODO
    switch (lookAhead->tokenType)
    {
    case SB_LPAR:
        eat(SB_LPAR);
        compileExpression();
        compileArguments2();
        eat(SB_RPAR);
        break;
    //FOLLOW
    case SB_SEMICOLON:   // FOLLOW cua statement
    case KW_END:
    case KW_ELSE:

    case SB_TIMES:// FOLLOW cua term2
    case SB_SLASH:

    case KW_TO: //FOLLOW cua for
    case KW_DO:

    case SB_COMMA:  // FOLLOW cua argument2

    case SB_EQ: // FIRST - epxilon cua condition2
    case SB_NEQ:
    case SB_LE:
    case SB_LT:
    case SB_GE:
    case SB_GT:

    case SB_RPAR: // FOLLOW cua factor --> co LPAR phai them RPAR neu ko se nhay sang loi

    case SB_RSEL: // FOLLOW cua indexes, giong factor

    case KW_THEN:   // FOLLOW cua if
        break;
    default:   // Error
        error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
        break;
    }
}

void compileArguments2(void)
{
    // TODO
    if(lookAhead->tokenType == SB_COMMA)
    {
        eat(SB_COMMA);
        compileExpression();
        compileArguments2();
    }
    if (lookAhead->tokenType == SB_RPAR)
    {
        //Khong lam gi vi neu phat hien dau dong ngoac thi dua ve ki tu ket thuc
    }
    else
    {
        error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
    }
}

void compileCondition(void)
{
    // TODO
    compileExpression();
    compileCondition2();
}

void compileCondition2(void)
{
    // TODO
    switch (lookAhead->tokenType)
    {
    case SB_EQ:
        eat(SB_EQ);
        compileExpression();
        break;
    case SB_NEQ:
        eat(SB_NEQ);
        compileExpression();
        break;
    case SB_LE:
        eat(SB_LE);
        compileExpression();
        break;
    case SB_LT:
        eat(SB_LT);
        compileExpression();
        break;
    case SB_GE:
        eat(SB_GE);
        compileExpression();
        break;
    case SB_GT:
        eat(SB_GT);
        compileExpression();
        break;
    default:
        error(ERR_INVALIDCOMPARATOR, lookAhead->lineNo, lookAhead->colNo);
        break;
    }
}

void compileExpression(void)
{
    assert("Parsing an expression");
    // TODO
    if(lookAhead -> tokenType == SB_PLUS)
    {
        eat(SB_PLUS);
        compileExpression2();
    }
    else if(lookAhead -> tokenType == SB_MINUS)
    {
        eat(SB_MINUS);
        compileExpression2();
    }
    else
    {
        compileExpression2();
    }

    assert("Expression parsed");
}

void compileExpression2(void)
{
    // TODO
    compileTerm();
    compileExpression3();
}


void compileExpression3(void)
{
    // TODO
    switch(lookAhead->tokenType)
    {
    case SB_PLUS:
        eat(SB_PLUS);
        compileTerm();
        compileExpression3();
        break;
    case SB_MINUS:
        eat(SB_MINUS);
        compileTerm();
        compileExpression3();
        break;
    // FOLLOW
    case SB_SEMICOLON: //FOLLOW cua statement
    case KW_END:
    case KW_ELSE:

    case KW_TO: // FOLLOW cua for
    case KW_DO:

    case SB_COMMA:// FOLLOW cua arguments2

    case SB_EQ:  // FOLLOW cua condition2
    case SB_NEQ:
    case SB_LE:
    case SB_LT:
    case SB_GE:
    case SB_GT:

    case SB_RPAR:  // FOLLOW cua factor)

    case SB_RSEL:  // FOLLOW indexes

    case KW_THEN:  // FOLLOW cua if
        break;
    // Error
    default:
        error(ERR_INVALIDEXPRESSION, lookAhead->lineNo, lookAhead->colNo);
        break;
    }
}

void compileTerm(void)
{
    // TODO
    compileFactor();
    compileTerm2();
}

void compileTerm2(void)
{
    // TODO
    switch(lookAhead -> tokenType )
    {
    case SB_TIMES:
        eat(SB_TIMES);
        compileFactor();
        compileTerm2();
        break;

    case SB_SLASH:
        eat(SB_SLASH);
        compileFactor();
        compileTerm2();
        break;

    //them FOLOW
    case SB_PLUS:
    case SB_MINUS:
    case SB_SEMICOLON: //FOLLOW cua statement
    case KW_END:
    case KW_ELSE:

    case KW_TO: // FOLLOW cua for
    case KW_DO:

    case SB_COMMA:// FOLLOW cua arguments2

    case SB_EQ:  // FOLLOW cua condition2
    case SB_NEQ:
    case SB_LE:
    case SB_LT:
    case SB_GE:
    case SB_GT:

    case SB_RPAR:  // FOLLOW cua factor)

    case SB_RSEL:  // FOLLOW indexes

    case KW_THEN:  // FOLLOW cua if
        break;
    default :
        error(ERR_INVALIDTERM, lookAhead->lineNo, lookAhead->colNo);
    }
}

void compileFactor(void)
{
    // TODO
    switch (lookAhead->tokenType)
    {
    case TK_NUMBER:
    case TK_CHAR:
        compileUnsignedConstant();
        break;
    case SB_LPAR:
        eat(SB_LPAR);
        compileExpression();
        eat(SB_RPAR);
        break;
    case TK_IDENT:
        eat(TK_IDENT);
        switch(lookAhead->tokenType)
        {
        case SB_LSEL:
            compileIndexes();
            break;
        case SB_LPAR:
            compileArguments();
            break;
        default:
            break;
        }
        break;
    default:
        error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
        break;
    }
}

void compileIndexes(void)
{
    // TODO
    if (lookAhead->tokenType == SB_LSEL)
    {
        eat(SB_LSEL);
        compileExpression();
        eat(SB_RSEL);
        compileIndexes();
    }
}

int compile(char *fileName)
{
    if (openInputStream(fileName) == IO_ERROR)
        return IO_ERROR;

    currentToken = NULL;
    lookAhead = getValidToken();

    compileProgram();

    free(currentToken);
    free(lookAhead);
    closeInputStream();
    return IO_SUCCESS;

}

