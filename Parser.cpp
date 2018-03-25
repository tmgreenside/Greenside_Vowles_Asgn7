//Sammy Vowles and Trevor Greenside
//February 22, 2018
//CPSC 326
//Assignment 5

#include "Parser.h"

#include <iostream>
#include <sstream>

using namespace std;

const bool USE_CONTEXTLOG = false;

class ContextLog {
public:
    ContextLog(string p, const Lexeme &lex):
    part(p),
    l(lex.line),
    c(lex.col) {
        if (USE_CONTEXTLOG)
            cout << "Entering " << part << "(" << l << ":" << c << ")" << endl;
    }
    ~ContextLog() {
        if (USE_CONTEXTLOG)
            cout << "Exiting " << part << "(" << l << ":" << c << ")" << endl;
    }
private:
    string part;
    int l;
    int c;
};

Parser::Parser(Lexer &lex): lexer(lex) {
    advance();
    syntaxTree = stmts();
}

shared_ptr<ASTNode> Parser::getAST() {
    return syntaxTree;
}

void Parser::advance() {
    currentLexeme = lexer.next();
}

void Parser::eat(Token token, std::string message) {
    if (currentLexeme.token == token) {
        advance();
    } else {
        error(message);
    }
}

void Parser::error(std::string message) {
    cout << "Error found" << endl;
    ostringstream oss;
    oss << message << ", found " << currentLexeme << " instead";
    throw ParserException(oss.str());
}

std::shared_ptr<ASTStatementList> Parser::stmts(std::shared_ptr<ASTStatementList> lst) {
    ContextLog clog("stmts", currentLexeme);
    if (!lst) lst = make_shared<ASTStatementList>();
    switch (currentLexeme.token) {
        case Token::PRINT:
        case Token::PRINTLN:
        case Token::ID:
        case Token::IF:
        case Token::WHILE:
            lst->statements.push_back(stmt());
            stmts(lst);
            break;
        default:
            // May be empty
            break;
    }
    return lst;
}

//calls the output, assignMethod, cond or loop methods if a Token of the
//corresponding method is called
std::shared_ptr<ASTStatement> Parser::stmt() {
    ContextLog clog("stmt", currentLexeme);
    switch (currentLexeme.token) {
        case Token::PRINT:
        case Token::PRINTLN:
            return output();
            break;
        case Token::ID:
            return assign();
            break;
        case Token::IF:
            return cond();
            break;
        case Token::WHILE:
            return loop();
            break;
        default:
            error("Expected statement");
    }
}

// Creates an AST Print Statement to link output with
// the correct print token.
std::shared_ptr<ASTPrintStatement> Parser::output() {
    ContextLog clog("output", currentLexeme);
    auto ans = std::make_shared<ASTPrintStatement>();
    if (currentLexeme.token == Token::PRINT) {
        eat(Token::PRINT, "Expected print");
        ans->isPrintln = false;
    } else {
        eat(Token::PRINTLN, "Expected print or println");
        ans->isPrintln = true;
    }
    eat(Token::LPAREN, "Expected '('");
    ans->expression = expr();
    eat(Token::RPAREN, "Expected ')'");
    eat(Token::SEMICOLON, "Expected ';'");
    return ans;
}

// Creates an AST Read Expression in memory to handle
// an input statement in the program to be compiled
std::shared_ptr<ASTReadExpression> Parser::input() {
    ContextLog clog("input", currentLexeme);
    auto ans = std::make_shared<ASTReadExpression>();
    // TODO

    if (currentLexeme.token == Token::READINT){
        ans->isReadInt = true;
        eat(Token::READINT, "Expected READINT");
        eat(Token::LPAREN, "Expected '('");
        ans->message = currentLexeme.text;
        eat(Token::STRING, "Expected string");
        eat(Token::RPAREN, "Expected ')'");
    }
    else if (currentLexeme.token == Token::READSTR){
        eat(Token::READSTR, "Expected READSTR");
        eat(Token::LPAREN, "Expected '('");
        ans->message = currentLexeme.text;
        eat(Token::STRING, "Expected string");
        eat(Token::RPAREN, "Expected ')'");
    }
    else{
        eat(Token::READINT, "Expected READINT or READSTR");
    }
    return ans;
}

// Handles an assignment statement by creating a AST
// Identifier and storing the identifier name (LHS) and
// its value, or whatever is on the RHS.
std::shared_ptr<ASTAssignmentStatement> Parser::assign() {
    ContextLog clog("assign", currentLexeme);
    auto ans = make_shared<ASTAssignmentStatement>();
    // TODO

    auto id = make_shared<ASTIdentifier>();
    if (currentLexeme.token == Token::ID) {
        // fill in here, see value() ID case
        id->name = currentLexeme.text;
        advance();
        id->indexExpression = listindex();
    } else {
        eat(Token::ID, "Expected identifier");
    }
    ans->identifier = id;
    eat(Token::ASSIGN, "Expected '='");
    ans->rhs = expr();//value();
    eat(Token::SEMICOLON, "Expected ';'");

    return ans;
}

// Stores an expression that is found encased in
// brackets.
std::shared_ptr<ASTExpression> Parser::listindex() {
    ContextLog clog("listindex", currentLexeme);
    // TODO

    if (currentLexeme.token == Token::LBRACKET) {
        auto ans = make_shared<ASTListLiteral>();
        advance();
        ans->expressions.push_back(expr());
        eat(Token::RBRACKET, "Expected ']'");
        return ans;
    }
    return nullptr;
}

// for complex expressions, stores the beginning and
// creates another AST node to handle the remaining part
// of the expression, handled in exprt().
std::shared_ptr<ASTExpression> Parser::expr() {
    ContextLog clog("expr", currentLexeme);
    // TODO

    auto ans = make_shared<ASTComplexExpression>();
    ans->firstOperand = value(); // value() returns an ASTExpression ptr.
    exprt(ans); // returns an ASTComplexExpression ptr.

    if(ans->rest != nullptr){
        return ans;
    }

    return ans->firstOperand;
}

// calls the mathRel and expr method if the next token is PLUS,MINUS,DIVIDE,
// MULTIPLY or MODULUS, and adds information to an existing AST Expression.
// Otherwise does nothing.
void Parser::exprt(std::shared_ptr<ASTComplexExpression> expression) {
    ContextLog clog("exprt", currentLexeme);
    // TODO

    switch(currentLexeme.token) {
        case Token::PLUS:
        case Token::MINUS:
        case Token::DIVIDE:
        case Token::MULTIPLY:
        case Token::MODULUS:
            expression->operation = currentLexeme.token;
            mathRel();
            expression->rest = expr();
            break;
        default:
            // may be empty
            break;
    }
}

//advances if the next token is PLUS,MINUS,DIVIDE,MULTIPLY,or MODULUS
//otherwise error, so an error message will be displayed
void Parser::mathRel() {
    ContextLog clog("math_rel", currentLexeme);
    switch(currentLexeme.token) {
        case Token::PLUS:
        case Token::MINUS:
        case Token::DIVIDE:
        case Token::MULTIPLY:
        case Token::MODULUS:
            advance();
            break;
        default:
            // No math operator found, throw error
            eat(Token::PLUS, "Expected '+' or '-' or '*' or '/' or '%'");
            break;
    }
}

// Returns true if the token represents a value, or an incoming one.
// Returns false otherwise.
bool isTokenAValue(Token token) {
    switch (token) {
        case Token::ID:
        case Token::STRING:
        case Token::INT:
        case Token::BOOL:
        case Token::READINT:
        case Token::READSTR:
        case Token::LBRACKET:
            return true;
            break;
        default:
            return false;
    }
}

// calls the advances ID and calls listindex method, advances STRING, advances INT,
// advances BOOL, calls the input method or advances LBRACKET, then calls the
// exprlist mehod then eats RBRACKET
std::shared_ptr<ASTExpression> Parser::value() {
    ContextLog clog("value", currentLexeme);
    switch (currentLexeme.token) {
        case Token::ID:
        {
            // TODO
            auto ans = make_shared<ASTIdentifier>();
            ans->name = currentLexeme.text;
            advance();
            ans->indexExpression = listindex();
            return ans;
            break;
        }
        case Token::STRING:
        {
            auto ans = make_shared<ASTLiteral>();
            ans->type = MPLType::STRING;
            ans->value = currentLexeme.text;
            advance();
            return ans;
            break;
        }
        case Token::INT:
        {
            auto ans = make_shared<ASTLiteral>();
            // TODO
            ans->type = MPLType::INT;
            ans->value = currentLexeme.text;
            advance();
            return ans;
            break;
        }
        // TODO
        case Token::BOOL:
        {
            auto ans = make_shared<ASTLiteral>();
            ans->type = MPLType::BOOL;
            ans->value = currentLexeme.text;
            advance();
            return ans;
            break;
        }
        case Token::READINT:
        case Token::READSTR:
        {
            return input();
            break;
        }
        case Token::LBRACKET:
        {
            advance();
            auto ans = exprlist();
            eat(Token::RBRACKET, "Expected ']'");
            return ans;
            break;
        }
        default:
            error("Expected a value");
    }
}

// While new expressions exist, adds them to an AST node representing
// a list.
std::shared_ptr<ASTListLiteral> Parser::exprlist() {
    ContextLog clog("exprlist", currentLexeme);
    auto ans = make_shared<ASTListLiteral>();
    // TODO

    switch (currentLexeme.token) {
        case Token::ID:
        case Token::STRING:
        case Token::INT:
        case Token::BOOL:
        case Token::READINT:
        case Token::READSTR:
        case Token::LBRACKET:
        case Token::RBRACKET:
            ans->expressions.push_back(expr()); //expression = type ASTExpression
            exprtail(ans); //returns void, takes in ASTExpression
            return ans;
            break;
        default:
            // May be empty
            break;
    }

    return ans;
}

// For comma separated expressions, adds them to the existing
// AST list literal passed in as an argument.
void Parser::exprtail(std::shared_ptr<ASTListLiteral> list) {
    ContextLog clog("exprtail", currentLexeme);
    // TODO
    if (currentLexeme.token == Token::COMMA) {
        advance();
        list->expressions.push_back(expr());//returns type ASTExpression
        exprtail(list); //takes type ASTListLiteral as a parameter
    }
}


std::shared_ptr<ASTIfStatement> Parser::cond() {
    ContextLog clog("cond", currentLexeme);
    auto ans = make_shared<ASTIfStatement>();
    // TODO

    eat(Token::IF, "Expected 'if'");
    ans->baseIf.expression = bexpr(); //returns type ASTBoolExpression
    eat(Token::THEN, "Expected 'then'");
    ans->baseIf.statementList = stmts(); //returns type ASTStatementList, takes in type ASTStatementList as parameter
    condt(ans); // returns void, takes ASTIfStatement as parameter
    eat(Token::END, "Expected 'end'");

    return ans;
}


void Parser::condt(std::shared_ptr<ASTIfStatement> statement) {
    ContextLog clog("condt", currentLexeme);
    // TODO

    switch (currentLexeme.token) {
        case Token::ELSEIF:
        {
            eat(Token::ELSEIF, "Expected elseif");
            ASTBasicIf nextElseif;
            nextElseif.expression = bexpr();
            eat(Token::THEN, "Expected then");
            //nextElseif.statementList = stmts(statement->baseIf.statementList); // not sure about this line???
            nextElseif.statementList = stmts();
            statement->elseifs.push_back(nextElseif);
            condt(statement);
            break;
        }
        case Token::ELSE:
        {
            eat(Token::ELSE, "Expected else");
            //statement->elseList = stmts(statement->baseIf.statementList); //takes in ASTStamentList, returns ASTStamentList
            statement->elseList = stmts();
            break;
        }
        default:
            // May be empty
            break;
    }
}

std::shared_ptr<ASTBoolExpression> Parser::bexpr() {

    ContextLog clog("bexpr", currentLexeme);
    bool negated = false;
    if (currentLexeme.token == Token::NOT) {
        advance();
        negated = true;
    }

    auto lhs = expr();

    switch (currentLexeme.token) {
        case Token::EQUAL:
        case Token::LESS_THAN:
        case Token::GREATER_THAN:
        case Token::LESS_THAN_EQUAL:
        case Token::GREATER_THAN_EQUAL:
        case Token::NOT_EQUAL: {
            auto ans = make_shared<ASTComplexBoolExpression>();
            ans->negated = negated;
            ans->first = lhs;
            bexprt(ans);
            return ans;
            break;
        }
        default: {
            auto ans = make_shared<ASTSimpleBoolExpression>();
            ans->negated = negated;
            ans->expression = lhs;
            return ans;
            break;
        }

    }

}

void Parser::bexprt(std::shared_ptr<ASTComplexBoolExpression> expression) {
    ContextLog clog("bexprt", currentLexeme);
    switch (currentLexeme.token) {
        case Token::EQUAL:
        case Token::LESS_THAN:
        case Token::GREATER_THAN:
        case Token::LESS_THAN_EQUAL:
        case Token::GREATER_THAN_EQUAL:
        case Token::NOT_EQUAL:
            expression->relation = currentLexeme.token;
            break;
        default:
            error("Internal error: expected relational operator");
    }
    advance();
    expression->second = expr();
    bconnect(expression);
}

void Parser::bconnect(std::shared_ptr<ASTComplexBoolExpression> expression) {
    ContextLog clog("bconnect", currentLexeme);
    // TODO !!!!!!!!!!! unfinished

    if (currentLexeme.token == Token::AND){
        expression->hasConjunction = true;
        advance();
        expression-> remainder = bexpr(); //return ASTBoolExpression
    } else if (currentLexeme.token == Token::OR){
        expression-> remainder = bexpr();
    }
}

std::shared_ptr<ASTWhileStatement> Parser::loop() {
    ContextLog clog("loop", currentLexeme);
    auto ans = make_shared<ASTWhileStatement>();

    // TODO
    eat(Token::WHILE, "Expected 'while'");
    ans->condition = bexpr();
    eat(Token::DO, "Expected 'do'");
    ans->statements = stmts();
    eat(Token::END, "Expected 'end'");
    return ans;
}
