
// names: Trevor Greenside, Sammy Vowles
// date: 30 January 2018
// course: CPSC 326 - 01
// asgn: 2

#include <sstream>

#include "Lexer.h"

using namespace std;

Lexer::Lexer(std::istream &initIn): in(initIn) {
    // You shouldn't need to modify this function

    // If we are given an invalid input stream, throw an exception
    if (!in) {
        throw LexerException("Invalid input stream passed to Lexer");
    }
}

Lexeme Lexer::next() {
    eatWhitespace();
    char tmp = peek();
    int startLine = lineNum;
    int startCol = colNum;
    if (tmp == eof()) {
        return Lexeme(Token::EOS, "", lineNum, colNum);
    } else if ((tmp >= 'a' && tmp <= 'z') || (tmp >= 'A' && tmp <= 'Z')) {
        // TODO: This will be the beginning of an identifier or of some Token
        // that is represented by a series of letters (such as "while")

        // traverse until hit something not letter, number, underscore
        // save each char into string, then compare with all key words
        // not match, then other identifier
        string word = "";
        read();
        word += tmp;
        while ((peek() >= 'a' and peek() <= 'z') or (peek() >= 'A' and peek() <= 'Z') or (peek() >= '0' and peek() <= '9') or peek() == '_') {
            word += read();
        }
        // check each keyword against the string one by one
        if (word == "while") {
            return Lexeme(Token::WHILE, "while", startLine, startCol);
        } else if (word == "if") {
            return Lexeme(Token::IF, "if", startLine, startCol);
        } else if (word == "then") {
            return Lexeme(Token::THEN, "then", startLine, startCol);
        } else if (word == "else") {
            return Lexeme(Token::ELSE, "else", startLine, startCol);
        } else if (word == "elseif") {
            return Lexeme(Token::ELSEIF, "elseif", startLine, startCol);
        } else if (word == "do") {
            return Lexeme(Token::DO, "do", startLine, startCol);
        } else if (word == "end") {
            return Lexeme(Token::END, "end", startLine, startCol);
        } else if (word == "print") {
            return Lexeme(Token::PRINT, "print", startLine, startCol);
        } else if (word == "println") {
            return Lexeme(Token::PRINTLN, "println", startLine, startCol);
        } else if (word == "int") {
            return Lexeme(Token::INT, "int", startLine, startCol);
        } else if (word == "elseif") {
            return Lexeme(Token::ELSEIF, "elseif", startLine, startCol);
        } else if (word == "readint") {
            return Lexeme(Token::READINT, "readint", startLine, startCol);
        } else if (word == "readstr") {
            return Lexeme(Token::READSTR, "readstr", startLine, startCol);
        } else if (word == "false") {
            return Lexeme(Token::BOOL, "false", startLine, startCol);
        } else if (word == "true") {
            return Lexeme(Token::BOOL, "true", startLine, startCol);
        } else {
            return Lexeme(Token::ID, word, startLine, startCol);
        }
    }
    // check operators
    else if (tmp == '>') {
        read();
        if (peek() == '=') {
            read();
            return Lexeme(Token::GREATER_THAN_EQUAL, ">=", startLine, startCol);
        } else {
            return Lexeme(Token::GREATER_THAN, ">", startLine, startCol);
        }
    } else if (tmp == '<') {
        read();
        if (peek() == '=') {
            read();
            return Lexeme(Token::LESS_THAN_EQUAL, ">=", startLine, startCol);
        } else {
            return Lexeme(Token::LESS_THAN, ">", startLine, startCol);
        }
    }
    else if (tmp >= '0' && tmp <= '9') {
        string ans;
        while (peek() >= '0' && peek() <= '9') {
            ans += peek();
            read();
        }
        return Lexeme(Token::INT, ans, startLine, startCol);
    }
    // TODO: there are a ton of other cases to consider.
    else if (tmp == '+') {
        return makeSingleCharLexeme(Token::PLUS);
    }
    else if (tmp == '-') {
        return makeSingleCharLexeme(Token::MINUS);
    }
    else if (tmp == '*') {
        return makeSingleCharLexeme(Token::MULTIPLY);
    }
    else if (tmp == '/') {
        return makeSingleCharLexeme(Token::DIVIDE);
    }
    else if (tmp == '%') {
        return makeSingleCharLexeme(Token::MODULUS);
    }
    else if (tmp == '=') {
        read();
        if (peek() == '=') {
            read();
            return makeSingleCharLexeme(Token::EQUAL);
        } else {
            return Lexeme(Token::ASSIGN, "=", startLine, startCol);
        }
    }
    else if (tmp == '(') {
        return makeSingleCharLexeme(Token::LPAREN);
    }
    else if (tmp == ')') {
        return makeSingleCharLexeme(Token::RPAREN);
    }
    else if (tmp == '[') {
        return makeSingleCharLexeme(Token::LBRACKET);
    }
    else if (tmp == ']') {
        return makeSingleCharLexeme(Token::RBRACKET);
    }
    else if (tmp == ',') {
        return makeSingleCharLexeme(Token::COMMA);
    }
    else if (tmp == ';') {
        return makeSingleCharLexeme(Token::SEMICOLON);
    }
    else if (tmp == '"') {
        read();
        string theString = "";
        while (peek() != '"') {
            theString += peek();
            read();
            if (peek() == '\n') {
                // end of line midstring, throw error
                ostringstream oss;
                oss << "Multiline strings not supported.";
                throw LexerException(debugMessage(oss.str()));
            }
        }
        read();
        return Lexeme(Token::STRING, theString, startLine, startCol);
    }
    else {
        // You will not need to change the contents of this "else"
        ostringstream oss;
        oss << "Unexpected symbol: '" << peek() << "'";
        throw LexerException(debugMessage(oss.str()));
    }
    // Yes, you might get a warning about control reaching the end of a
    // non-void function.  Make sure you are returning a Lexeme from each
    // block (the code as distributed has one case with just a TODO).
    //
    // If this is an error in your compiler, you can put in a call to
    // abort() which will stop the program running if control reaches here.
}

bool Lexer::atEOF() {
    // You shouldn't need to modify this function
    return peek() == eof();
}

char Lexer::peek() {
    // You shouldn't need to modify this function
    return in.peek();
}

char Lexer::read() {
    // You shouldn't need to modify this function
    if (in.peek() == '\n') {
        colNum = 1;
        lineNum += 1;
    } else {
        colNum += 1;
    }
    return in.get();
}
void Lexer::eatWhitespace() {
    // TODO
    //
    // I would also suggest having this function detect comments and handle
    // them appropriately.
    while (peek() == ' ' or peek() == '#' or peek() == '\t' or peek() == '\r' or peek() == '\n') {
        if (peek() == ' ' or peek() == '\t' or peek() == '\r' or peek() == '\n') {
            read();
        } else if (peek() == '#') {
            while (peek() != '\n') {
                read();
            }
            read();
        }
    }
}

char Lexer::eof() {
    // You shouldn't need to modify this function
    return std::istream::traits_type::eof();
}

Lexeme Lexer::makeSingleCharLexeme(Token token) {
    // You shouldn't need to modify this function
    auto prelin = lineNum;
    auto precol = colNum;
    string str(1, read());
    return Lexeme(token, str, prelin, precol);
}

std::string Lexer::debugMessage(std::string msg) {
    // You shouldn't need to modify this function
    ostringstream oss;
    oss << msg << " " << lineNum << ":" << colNum;
    return oss.str();
}
