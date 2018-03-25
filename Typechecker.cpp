// names: Trevor Greenside, Sammy Vowles
// date: 13 March 2018
// course: CPSC 326 - 01
// asgn: 6

#include "Typechecker.h"
#include "ASTNodes.h"

using namespace std;

void Typechecker::visit(ASTSimpleBoolExpression& simpleBoolExpr) {
    simpleBoolExpr.expression->accept(*this);
    if (currentType != MPLType::BOOL) {
        throw TypecheckerException("Expected boolean expression");
    }
    currentType = MPLType::BOOL;
}

void Typechecker::visit(ASTComplexBoolExpression& complexBoolExpr) {
    // TODO check
    currentType = MPLType::BOOL;
    complexBoolExpr.first->accept(*this);
    MPLType firstType = currentType;
    complexBoolExpr.second->accept(*this);
    if (firstType != currentType) {
        throw TypecheckerException("Second and First Types not matching");
    }
    switch (complexBoolExpr.relation) {
        case Token::GREATER_THAN:
        case Token::GREATER_THAN_EQUAL:
        case Token::LESS_THAN:
        case Token::LESS_THAN_EQUAL:
            if (currentType != MPLType::INT and currentType != MPLType::STRING)
                throw TypecheckerException("Operator valid only on integer");
        case Token::EQUAL:
        case Token::NOT_EQUAL:
            break;
        default:
            throw TypecheckerException("Invalid operator");
            break;
    }
    if (complexBoolExpr.hasConjunction) {
        complexBoolExpr.remainder->accept(*this);
    }
    currentType = MPLType::BOOL;
}

void Typechecker::visit(ASTStatementList& statementList) {
    table.pushTable();
    for (auto &statement : statementList.statements) {
        statement->accept(*this);
    }
    table.popTable();
}

void Typechecker::visit(ASTBasicIf& basicIf) {
    // TODO check
    basicIf.expression->accept(*this);
    basicIf.statementList->accept(*this);
}

void Typechecker::visit(ASTIfStatement& ifStatement) {
    // TODO check
    ifStatement.baseIf.accept(*this);
    for (int i = 0; i < ifStatement.elseifs.size(); i++) {
        ifStatement.elseifs[i].accept(*this);
    }
    ifStatement.elseList->accept(*this);
}

void Typechecker::visit(ASTWhileStatement& whileStatement) {
    // TODO check
    whileStatement.condition->accept(*this);
    whileStatement.statements->accept(*this);
}

void Typechecker::visit(ASTPrintStatement& printStatement) {
    printStatement.expression->accept(*this);
}

void Typechecker::visit(ASTAssignmentStatement& assignmentStatement) {
    // TODO  // Trevor 3 12 2018 at 12:10 pm
    if (!table.doesSymbolExist(assignmentStatement.identifier->name)) {
        // create new identifier, push
        assignmentStatement.rhs->accept(*this);
        switch (currentType) {
            case MPLType::INT:
                table.storeInt(assignmentStatement.identifier->name);
                break;
            case MPLType::BOOL:
                table.storeBool(assignmentStatement.identifier->name);
                break;
            case MPLType::STRING:
                table.storeString(assignmentStatement.identifier->name);
                break;
            case MPLType::ARRAY:
                table.storeVector(assignmentStatement.identifier->name);
                break;
            default:
                throw TypecheckerException("Invalid type");
                break;
        }
    }
    assignmentStatement.identifier->accept(*this);
    MPLType firstType = currentType;
    assignmentStatement.rhs->accept(*this);
    if (firstType != currentType) {
        throw TypecheckerException("Invalid type: identifier must be set to value of same type");
    }
}

void Typechecker::visit(ASTIdentifier& identifier) {
    if (table.doesSymbolExist(identifier.name)) {
        currentType = table.getSymbolType(identifier.name);
    } else {
        throw TypecheckerException("Identifier " + identifier.name + " used before defined");
    }
    if (identifier.indexExpression && currentType != MPLType::ARRAY) {
        throw TypecheckerException("Identifier " + identifier.name + " given an index when not an array");
    }
}

void Typechecker::visit(ASTLiteral& literal) {
    currentType = literal.type;
}

void Typechecker::visit(ASTListLiteral& listLiteral) {
    // TODO
    for (int i = 0; i < listLiteral.expressions.size(); i++) {
        listLiteral.expressions[i]->accept(*this);
    }
}

void Typechecker::visit(ASTReadExpression& readExpression) {
    if (readExpression.isReadInt) {
        currentType = MPLType::INT;
    } else {
        currentType = MPLType::STRING;
    }
}

void Typechecker::visit(ASTComplexExpression& complexExpression) {
    // TODO
    complexExpression.firstOperand->accept(*this);
    MPLType firstType = currentType;
    complexExpression.rest->accept(*this); // change current type for switch
    if (firstType != currentType) {
        throw TypecheckerException("Second and First Types must be the same");
    }
    switch (complexExpression.operation) {
        case Token::MODULUS:
        case Token::MINUS:
        case Token::MULTIPLY:
        case Token::DIVIDE:
            if (firstType == MPLType::STRING) {
                throw TypecheckerException("Cannot perform math operations other than + on string");
            }
        case Token::PLUS:
            break;
        default:
            throw TypecheckerException("Expected a Math operator");
            break;
    }
    
}




