#include<iostream>
#include<string>
#include "Scanner.hpp"

using namespace std;

class Parser {
private:
    Scanner* scanner;
    Token *currentToken, *previousToken;

    bool match(Token::Type type);
    bool check(Token::Type type);
    bool advanceToken();
    bool endOfTokens();
public:
    explicit Parser(Scanner* scanner) : scanner(scanner), currentToken(nullptr), previousToken(nullptr) {}

    bool parse();
    bool processSelect();
    bool processInsert();
    bool processCreate();
    bool processDelete();
    bool displayError(const string& message);
};

bool Parser::match(Token::Type type) {
    if (check(type)) {
        advanceToken();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type type) {
    if (endOfTokens()) return false;
    return currentToken->type == type;
}

bool Parser::advanceToken() {
    if (!endOfTokens()) {
        Token* tempToken = currentToken;
        if (previousToken) delete previousToken;
        currentToken = scanner->nextToken();
        previousToken = tempToken;

        if (check(Token::ERR)) {
            cout << "Error de análisis: Carácter no reconocido: " << currentToken->lexema << endl;
            exit(0);
        }
        return true;
    }
    return false;
}

bool Parser::endOfTokens() {
    return currentToken->type == Token::END;
}

bool Parser::processCreate() {
    if (!match(Token::CREATE)) return false;
    if (!match(Token::TABLE)) return false;
    if (!match(Token::ID)) return false;
    string tableName = previousToken->lexema;
    if (!match(Token::FROM)) return false;
    if (!match(Token::FILE)) return false;
    if (!match(Token::STRING)) return false;
    string fileName = previousToken->lexema;
    if (!match(Token::USING)) return false;
    if (!match(Token::INDEX)) return false;
    if (!match(Token::HASH) && !match(Token::AVL) && !match(Token::BTREE)) return false;
    Token::Type indexType = previousToken->type;
    if (!match(Token::LPAREN)) return false;
    if (!match(Token::RPAREN)) return false;
    string keyField = previousToken->lexema;
    if (!match(Token::RPAREN)) return false;
    return true;
}

bool Parser::processSelect() {
    if (!match(Token::SELECT)) return false;
    if (!match(Token::ALL)) return false;
    if (!match(Token::FROM)) return false;
    if (!match(Token::ID)) return false;
    string tableName = previousToken->lexema;
    if (!match(Token::WHERE)) return false;
    if (!match(Token::ID)) return false;
    string keyField = previousToken->lexema;
    cout << "Campo clave: " << keyField << endl; // depuración
    if (!match(Token::EQUAL) && !match(Token::BETWEEN)) return false;
    return true;
}

bool Parser::processInsert() {
    if (!match(Token::INSERT)) return false;
    if (!match(Token::INTO)) return false;
    if (!match(Token::ID)) return false;
    string tableName = previousToken->lexema;
    if (!match(Token::VALUES)) return false;
    if (!match(Token::LPAREN)) return false;
    if (!match(Token::RPAREN)) return false;
    string values = previousToken->lexema;
    if (!match(Token::RPAREN)) return false;
    return true;
}

bool Parser::processDelete() {
    if (!match(Token::DELETE)) return false;
    if (!match(Token::FROM)) return false;
    if (!match(Token::ID)) return false;
    string tableName = previousToken->lexema;
    if (!match(Token::WHERE)) return false;
    if (!match(Token::ID)) return false;
    string keyField = previousToken->lexema;
    if (!match(Token::EQUAL)) return false;
    return true;
}

bool Parser::parse() {
    currentToken = scanner->nextToken();
    if (check(Token::ERR)) {
        cout << "Error en scanner: Carácter inválido detectado" << endl;
        exit(0);
    }
    if (check(Token::CREATE)) {
        return processCreate();
    } else if (check(Token::SELECT)) {
        return processSelect();
    } else if (check(Token::INSERT)) {
        return processInsert();
    } else if (check(Token::DELETE)) {
        return processDelete();
    } else {
        return false;
    }
}