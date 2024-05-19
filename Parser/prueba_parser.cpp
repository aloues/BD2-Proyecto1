#include "Parser.hpp"

int main() {
    const char* input = "select * from Customer where DNI = x";
    Scanner scanner(input);
    Parser parser(&scanner);

    parser.parse();

    return 0;
}
