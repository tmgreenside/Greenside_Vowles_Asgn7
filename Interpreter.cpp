#include <iostream>
#include <sstream>
#include <limits>

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
    bool myResult = false; // initialize with temp variable to suppress warnings

    // TODO: figure out what comparison to make, do that comparison,
    // and store the result in myResult.
    
    if (lhsType != currentType)
        throw InterpreterException("Invalid types: left and right hand side types must be the same");

    switch(complexBoolExpr.relation) {
        case Token::GREATER_THAN:
            if (currentType == MPLType::INT) {
                if (lhsInt > currentInt) {
                    myResult = true;
                } else {
                    myResult = false;
                }
            } else if (currentType == MPLType::STRING) {
                if (lhsString > currentString) {
                    myResult = true;
                } else {
                    myResult = false;
                }
            } else {
                myResult = false;
            }
            break;
        case Token::GREATER_THAN_EQUAL:
            if (currentType == MPLType::INT) {
                if (lhsInt >= currentInt) {
                    myResult = true;
                } else {
                    myResult = false;
                }
            } else if (currentType == MPLType::STRING) {
                if (lhsString >= currentString) {
                    myResult = true;
                } else {
                    myResult = false;
                }
            } else {
                myResult = false;
            }
            break;
        case Token::LESS_THAN:
            if (currentType == MPLType::INT) {
                if (lhsInt < currentInt) {
                    myResult = true;
                } else {
                    myResult = false;
                }
            } else if (currentType == MPLType::STRING) {
                if (lhsString < currentString) {
                    myResult = true;
                } else {
                    myResult = false;
                }
            } else {
                myResult = false;
            }
            break;
        case Token::LESS_THAN_EQUAL:
            if (currentType == MPLType::INT) {
                if (lhsInt <= currentInt) {
                    myResult = true;
                } else {
                    myResult = false;
                }
            } else if (currentType == MPLType::STRING) {
                if (lhsString <= currentString) {
                    myResult = true;
                } else {
                    myResult = false;
                }
            } else {
                myResult = false;
            }
            break;
        case Token::EQUAL:
            if (currentType == MPLType::INT) {
                if (lhsInt == currentInt)
                    myResult = true;
                else
                    myResult = false;
            } else if (currentType == MPLType::STRING) {
                if (lhsString == currentString)
                    myResult = true;
                else
                    myResult = false;
            } else if (currentType == MPLType::BOOL) {
                if (lhsBool == currentBool)
                    myResult = true;
                else
                    myResult = false;
            }
            break;
        case Token::NOT_EQUAL:
            if (currentType == MPLType::INT) {
                if (lhsInt == currentInt)
                    myResult = false;
                else
                    myResult = true;
            } else if (currentType == MPLType::STRING) {
                if (lhsString == currentString)
                    myResult = false;
                else
                    myResult = true;
            } else if (currentType == MPLType::BOOL) {
                if (lhsBool == currentBool)
                    myResult = false;
                else
                    myResult = true;
            }
            break;
        default:
            myResult = false;
            break;
    }

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
    // TODO completed Trevor 3-27 at 1:22pm
    ifStatement.baseIf.expression->accept(*this);
    if (currentBool) {
        ifStatement.baseIf.statementList->accept(*this);
        return;
    } else {
        for (int i = 0; i < ifStatement.elseifs.size(); i++) {
            ifStatement.elseifs[i].expression->accept(*this);
            if (currentBool) {
                ifStatement.elseifs[i].statementList->accept(*this);
                return;
            }
        }
    }
    ifStatement.elseList->accept(*this);
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
    printStatement.expression->accept(*this);
    cout << currentString;
    if (printStatement.isPrintln)
        cout << endl;
}

void Interpreter::visit(ASTAssignmentStatement& assignmentStatement) {
    // TODO

    
    // create new identifier, push
    assignmentStatement.rhs->accept(*this);
    switch (currentType) {
        case MPLType::INT:
            table.storeIntVal(assignmentStatement.identifier->name, currentInt);
            break;
        case MPLType::BOOL:
            table.storeBoolVal(assignmentStatement.identifier->name, currentBool);
            break;
        case MPLType::STRING:
            table.storeStringVal(assignmentStatement.identifier->name, currentString);
            break;
        // If not going for extra credit, Dr. Schroeder says we can pretend arrays don't exist.
//        case MPLType::ARRAY:
//            // !!!!!!!!!!!!!!!!!!!!!!!! How to get current vector value to pass in?
//            table.storeVector(assignmentStatement.identifier->name, vector<Symbol>());
//            break;
        default:
            throw InterpreterException("Invalid type");
            break;
    }
    

}

void Interpreter::visit(ASTIdentifier& identifier) {
    // TODO
    if (table.doesSymbolExist(identifier.name)) {
        currentType = table.getSymbolType(identifier.name);
        switch(currentType) {
            case MPLType::INT:
                currentInt = table.getIntVal(identifier.name);
                break;
            case MPLType::STRING:
                currentString = table.getStringVal(identifier.name);
                break;
            case MPLType::BOOL:
                currentBool = table.getBoolVal(identifier.name);
                break;
            default:
                throw InterpreterException("Invalid type");
        }
    } else {
        throw InterpreterException("Identifier " + identifier.name + " used before defined");
    }
    if (identifier.indexExpression && currentType != MPLType::ARRAY) {
        throw InterpreterException("Identifier " + identifier.name + " given an index when not an array");
    }


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
    // TODO  started Trevor 3-25 at 1:12 pm
    complexExpression.firstOperand->accept(*this);
    MPLType firstType = currentType;
    auto lhsInt = currentInt;
    auto lhsString = currentString;
    complexExpression.rest->accept(*this); // change current type for switch
    if (firstType != currentType) {
        throw InterpreterException("Second and First Types must be the same");
    }
    if (firstType == MPLType::INT) {
        switch (complexExpression.operation) {
            case Token::MODULUS:
                currentInt = lhsInt % currentInt;
                break;
            case Token::MINUS:
                currentInt = lhsInt - currentInt;
                break;
            case Token::MULTIPLY:
                currentInt = lhsInt * currentInt;
                break;
            case Token::DIVIDE:
                currentInt = lhsInt / currentInt;
                break;
            case Token::PLUS:
                currentInt = lhsInt + currentInt;
                break;
            default:
                break;
        }
    } else if (firstType == MPLType::STRING) {
        switch (complexExpression.operation) {
            case Token::PLUS:
                lhsString += currentString;
                break;
            default:
                throw InterpreterException("Invalid operator on type STRING");
                break;
        }
        
    }
}
