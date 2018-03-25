#include <iostream>
#include <sstream>

#include "Interpreter.h"
#include "ASTNodes.h"

using namespace std;

void Interpreter::visit(ASTSimpleBoolExpression& simpleBoolExpr) {
    simpleBoolExpr.expression->accept(*this);
    // Note: after visiting the above expression, (assuming it's boolean)
    // it will set currentBool to be whatever boolean value it represents
    if (currentType != MPLType::BOOL) {
        throw InterpreterException("Expected boolean expression");
    }
    if (simpleBoolExpr.negated) {
        currentBool = !currentBool;
    }
    currentType = MPLType::BOOL;
}

void Interpreter::visit(ASTComplexBoolExpression& complexBoolExpr) {
    complexBoolExpr.first->accept(*this);
    auto lhsType = currentType;
    auto lhsInt = currentInt;
    auto lhsString = currentString;
    auto lhsBool = currentBool;
    complexBoolExpr.second->accept(*this);
    bool myResult;
    
    // TODO: figure out what comparison to make, do that comparison,
    // and store the result in myResult.
    
    if (complexBoolExpr.hasConjunction) {
        complexBoolExpr.remainder->accept(*this);
        if (complexBoolExpr.conjunction == Token::AND) {
            currentBool = myResult && currentBool;
        } else {
            currentBool = myResult || currentBool;
        }
    } else {
        currentBool = myResult;
    }
    if (complexBoolExpr.negated) {
        currentBool = !currentBool;
    }
    currentType = MPLType::BOOL;
}

void Interpreter::visit(ASTStatementList& statementList) {
    table.pushTable();
    for (auto &statement : statementList.statements) {
        statement->accept(*this);
    }
    table.popTable();
}

void Interpreter::visit(ASTBasicIf& basicIf) {
    // I strongly suggest not using this function and instead having
    // visit(ASTIfStatement) explicitly look inside the various baseIfs.
    // This is because once you enter a certain if statementList, you will
    // not want to enter any other statementLists.
    abort();
}

void Interpreter::visit(ASTIfStatement& ifStatement) {
    // TODO
}

void Interpreter::visit(ASTWhileStatement& whileStatement) {
    whileStatement.condition->accept(*this);
    while (currentBool) {
        whileStatement.statements->accept(*this);
        whileStatement.condition->accept(*this);
    }
}

void Interpreter::visit(ASTPrintStatement& printStatement) {
    // TODO
}

void Interpreter::visit(ASTAssignmentStatement& assignmentStatement) {
    // TODO
}

void Interpreter::visit(ASTIdentifier& identifier) {
    // TODO
}

void Interpreter::visit(ASTLiteral& literal) {
    currentType = literal.type;
    if (currentType == MPLType::INT) {
        // istringstream allows us to create something like "cin" but instead
        // of reading from the keyboard, it reads from the string that we give
        // it
        istringstream iss(literal.value);
        iss >> currentInt;
        // If there was an error reading the integer out of the istringstream,
        // it will evaluate to false when used in a boolean context.
        if (!iss) {
            throw InterpreterException("Bad integer literal: " + literal.value);
        }
    } else if (currentType == MPLType::BOOL) {
        // also could be written as
        // currentBool = literal.value == "true", but that looks confusing
        if (literal.value == "true") {
            currentBool = true;
        } else {
            currentBool = false;
        }
    } else {
        currentString = literal.value;
    }
}

void Interpreter::visit(ASTListLiteral& listLiteral) {
    // TODO? (only needed for extra credit)
}

void Interpreter::visit(ASTReadExpression& readExpression) {
    cout << readExpression.message;
    if (readExpression.isReadInt) {
        cin >> currentInt;
        currentType = MPLType::INT;
    } else {
        // getline will read everything up until a newline character, allowing
        // us to read in a string containing spaces
        getline(cin, currentString);
        currentType = MPLType::STRING;
    }
    // If there was an input problem (say, a user entering 'test' to readint),
    // cin will be false when used in a boolean context.  This way, if the
    // user enters something silly, we can throw an error.
    if (!cin) {
        throw InterpreterException("Invalid user input");
    }
    // If we read in a number, ignore everything else on the current line
    if (readExpression.isReadInt) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void Interpreter::visit(ASTComplexExpression& complexExpression) {
    // TODO
}
