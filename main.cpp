#include <iostream>
#include <string>

#include "Token.h"
#include "Lexeme.h"
#include "Lexer.h"
#include "Parser.h"
#include "ASTPrinter.h"
#include "Typechecker.h"

using namespace std;

int main(int argc, const char * argv[]) {
    string code = R"code(
x = readint("Enter an int: ");
y = readint("Enter an int: ");
if x > y then
    println("The first int was bigger than the second!");
    z = 1;
elseif y > x then
    println("The second int was bigger than the first!");
    z = "test";
else
    println("You entered the same value twice!");
    z = false;
end
y = 21;
while x > 0 do
    print(".");
    x = x - 1;
end
z = [1, 2, 3];
    )code";
    
    try {
        istringstream ins(code);
        Lexer lexer(ins);
        Parser parser(lexer);
        ASTPrinter printer;
        parser.getAST()->accept(printer);
        Typechecker checker;
        parser.getAST()->accept(checker);
    } catch (MyPLException &e) {
        cout << "Error encountered: " << e.what() << endl;
    }
    return 0;
}
